#include "screen.h"

Screen::Screen(QWidget *parent): QDialog(parent){
    init();
}

Screen::~Screen() {
    threadBackend->quit();
    threadBackend->wait();
    threadUSB->quit();
    threadUSB->wait();
    threadUART->quit();
    threadUART->wait();
    //delete threadBackend;
    //delete threadUSB;
    //delete threadUART;
}

void Screen::init() {
    setWindowTitle("Server Stend");
    butExit = new QPushButton("Exit",this);
    connect(butExit,&QPushButton::clicked,[this](){close();});
    labInfo = new QLabel("INFO:",this);
    labVoltageState = new QLabel("ALT",this);
    layout = new QVBoxLayout(this);
    layout->addWidget(labVoltageState);
    layout->addWidget(labInfo);
    layout->addWidget(butExit);
    setLayout(layout);
    //!_____________________ objects _________________________________
    back = new backend(this);
    ftdi = new Ftdi(this);
    uart = new Uart(this);
    //! ____________________ backend ________________________________
    connect(back,&backend::sendText,[this](const QString& str){
        QString s = labInfo->text();
        s += "\n" + str;
        labInfo->setText(s);
    });
    threadBackend = new QThread();
    connect(threadBackend,&QThread::started, back, &backend::initSlot);
    connect(threadBackend,&QThread::finished,back,&QObject::deleteLater);
    back->moveToThread(threadBackend);
    connect(this, &Screen::sendVoltageState, back, &backend::receiveVoltageState);
    connect(this, &Screen::sendAmperageStateToBackend, back, &backend::receiveAmperageState);
    connect(this, &Screen::sendAmperageMesToBackend, back, &backend::receiveAmperageMeasurements);

    connect(back, &backend::sendAmperageStateToScreen,this,[this](const AmperageState &as, const AmperageLimit& al){
        emit sendAmperageStateToUart(as,al);
    });
    connect(this, &Screen::sendVoltageMeasurements, back, &backend::receiveVoltageMeasurements);
    connect(this, &Screen::sendToClient, back, &backend::sendSlot);
    timerBackEnd = new QTimer(this);
    connect(timerBackEnd, &QTimer::timeout, this, [&]{
        emit sendToClient();
    });
    connect(this,&Screen::amperage_measuring_start,uart,&Uart::amperage_measuring_start);
    connect(this,&Screen::amperage_measuring_stop,uart,&Uart::amperage_measuring_stop);
    connect(back,&backend::clientConnectedSignal, this, [&]{
        timerBackEnd->start(200);
        wifi_connected=true;
        emit amperage_measuring_start();
    });
    connect(back,&backend::clientDisconnectedSignal, this, [&]{
        timerBackEnd->stop();
        wifi_connected=false;
        emit amperage_measuring_stop();
    });
    //!_____________________ VOLTMETER _______________________________

    threadUSB = new QThread(this);
    connect(threadUSB,&QThread::started,ftdi,&Ftdi::initSlot);
    connect(threadUSB,&QThread::finished,ftdi,&QObject::deleteLater);
    ftdi->moveToThread(threadUSB);
    connect(ftdi,&Ftdi::sendVoltageState,this, [this](const VoltageState& vs){
        emit sendVoltageState(vs);
    });
    connect(ftdi,&Ftdi::connectedSignal,[&](const bool& flag){
        Ftdi_connected = flag;                                          // receive connected signal from USB
    });
    connect(this,&Screen::protocolInitSignal,ftdi,&Ftdi::protocolInitSlot);         // usb interconnection
    connect(this,&Screen::protocolDataInitSignal,ftdi,&Ftdi::protocolDataInitSlot); // usb interconnection
    connect(this,&Screen::getRawDataSignal,ftdi,&Ftdi::getRawDataSlot);             // usb interconnection
    timerUSB_connect = new QTimer(this);
    timerUSB_request = new QTimer(this);
    connect(ftdi,&Ftdi::sendText,this, [this](const QString& str){
        QString s = labInfo->text();
        s += "\n" + str;
        labInfo->setText(s);
        if(str == "Ftdi started"){
            timerUSB_connect->start(5000);
        }
    });
    connect(timerUSB_connect, &QTimer::timeout, [this]{              // connected timer
        //qDebug()<<"Screen thread:"<<QThread::currentThreadId();
        if(Ftdi_connected) {
            timerUSB_connect->stop();
            timerUSB_request->start(200); return;
        } else {
            emit protocolInitSignal();
        }
        qDebug()<<"timerConnect";
    });
    connect(timerUSB_request, &QTimer::timeout, [&]{            // requesting timer (already connected)
        if(!Ftdi_connected) {
            timerUSB_request->stop();
            timerUSB_connect->start(200);
            return;
        }
        if(!ftdi->protocolInitialized) {
            //ftdi->protocolDataInit();
            emit protocolDataInitSignal();
            QThread::sleep(1);
        } else { // get raw ADC data (first info request, then 16386 bytes request, then reset request)
            //ftdi->getRawData();
            emit getRawDataSignal();
            float amp = ftdi->Amplitude;
            float mean = ftdi->Mean;
            voltage_Amp = amp/k_kV;
            voltage_Mean = mean/k_kV_mean;
            emit sendVoltageMeasurements(voltage_Amp, voltage_Mean);
            labInfo->setText("Действующее: " + QString::number(static_cast<double>(voltage_Mean),'g',2)
                             + "\n" +
                             "Амплитудное: " + QString::number(static_cast<double>(voltage_Amp),'g',2));
        }
    });
    //! ____________________ AMPERMETER ______________________________

    connect(uart,&Uart::sendText,[this](const QString& str){
        QString s = labInfo->text();
        s += "\n" + str;
        labInfo->setText(s);
    });
    threadUART = new QThread(this);
    connect(threadUART,&QThread::started,uart, &Uart::initSlot);
    connect(threadUART,&QThread::finished,uart, &Uart::deleteLater);
    uart->moveToThread(threadUART);
    connect(this,&Screen::sendAmperageStateToUart,uart, &Uart::receiveAmperageState);
    connect(uart,&Uart::sendAmperageState,this,
            [this](const AmperageState &as, const AmperageLimit &al){
        qDebug()<<"sendAmperageStateToBackend from uart";
        emit sendAmperageStateToBackend(as,al);
    });
    connect(uart,&Uart::sendAmperageMesurements,this,
            [this](const float& am){
        emit sendAmperageMesToBackend(am);
    });

    threadBackend->start();
    threadBackend->setPriority(QThread::LowestPriority);
    threadUSB->start();
    threadUSB->setPriority(QThread::LowPriority);
    threadUART->start();
    threadUART->setPriority(QThread::LowPriority);
    qDebug() << "Screen thread: "<<QThread::currentThreadId();
}

void Screen::setInfo(const QString &str) {
    labInfo->setText(str);
}
