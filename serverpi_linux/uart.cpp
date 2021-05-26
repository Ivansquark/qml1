#include "uart.h"

constexpr uint8_t Uart::requestLimit[];
constexpr uint8_t Uart::requestFormVoltage[];
constexpr uint8_t Uart::measureRequest[];

Uart::Uart(QObject* parent):QObject(parent) {

}

Uart::~Uart() {
    delete com;
    delete timerMeasure;
}

void Uart::bytesHandle(qint64 bytes) {
    qDebug()<<"bytesHandle="<<bytes;
    Written=true;
}

void Uart::initSlot() {
    com = new QSerialPort(this);
    com->setPortName("/dev/ttyAMA1"); ///dev/ttyS0 - miniUart
    com->setBaudRate(QSerialPort::Baud9600);
    com->setParity(QSerialPort::NoParity);
    com->setDataBits(QSerialPort::Data8);
    com->setStopBits(QSerialPort::OneStop);
    //com->setFlowControl(QSerialPort::NoFlowControl);
    qDebug()<<"UART thread: "<< QThread::currentThreadId();
    if(com->open(QIODevice::ReadWrite)){
        emit sendText("UART started");
        timerMeasure = new QTimer(this);
        connect(timerMeasure,&QTimer::timeout,this,[this]{
            QByteArray data;
            for(int i =0; i<11;i++){
                data.append(measureRequest[i]);
            }
            write_to_com(data);
            data.clear();
            QThread::msleep(20);
            //if(flag) {
            //    setForm(1); flag = false;
            //} else {
            //    setForm(0); flag = true;
            //}

        });
        connect(com,&QSerialPort::bytesWritten,this, &Uart::bytesHandle);
        connect(com,&QSerialPort::readyRead,this,&Uart::read_from_com);
        wiringPiSetup();
        pinMode(29,OUTPUT);        
    } else {
        emit sendText("unable to start UART");
    }
}

void Uart::amperage_measuring_start() {
    timerMeasure->start(200);
}

void Uart::amperage_measuring_stop() {
    timerMeasure->stop();
}

void Uart::receiveAmperageState(const AmperageState &as, const AmperageLimit &al) {
    if(amperageState != as) {
        //amperageState = as;
        setAmperageState(as);
    }
    if(amperageLimit != al) {
        //amperageLimit = al;
        setAmperageLimit(al);
    }
    //uint8_t x = (uint8_t)as;
    //uint8_t y = (uint8_t)al;
    //qDebug()<<"Receive amperage state" <<x<<y;
}

void Uart::setAmperageState(const AmperageState &as) {
    //amperageState = as;
    setForm(static_cast<bool>(as));
}

void Uart::setAmperageLimit(const AmperageLimit &al) {
    setLimit(static_cast<uint8_t>(al));
}

void Uart::write_to_com(const QByteArray &data) {
    if(com->isOpen()) {

        // pin rts on
        com->write(data);
        qDebug()<<"write_to_com"<<data;
        //Written = false;
        com->waitForBytesWritten(1000);
        digitalWrite(29,HIGH); // (pin,value)
        QThread::msleep(11);
        QThread::usleep(150);
        digitalWrite(29,LOW); // (pin,value)
        // pin rts off


    }
}

void Uart::read_from_com() {

    if(com->isOpen()) {
        qint64 count = com->bytesAvailable();
        qDebug()<<"read from com bytes"<<count;
        if(count>14) {
            QByteArray receivedData = com->readAll();
            //for(int i=0;i<15;i++){
            //    uint8_t x = static_cast<uint8_t>(receivedData[i]);
            //    qDebug()<<"i= "<<i<<"x= "<<x;
            //}
            if(static_cast<uint8_t>(receivedData[0]) != 0x10){
                receivedData.remove(0,1);
                //qDebug()<<"remove";
            }
            if(static_cast<uint8_t>(receivedData[0]) != 0x10){
                receivedData.remove(0,1);
                //qDebug()<<"remove";
            }
            //for(int i=0;i<13;i++){
            //    uint8_t x = static_cast<uint8_t>(receivedData[i]);
            //    qDebug()<<"i= "<<i<<"x= "<<x;
            //}
            if(static_cast<uint8_t>(receivedData[0]) == 0x10) {
                uint16_t EXP = static_cast<uint16_t>((static_cast<uint8_t>(receivedData[10])<<8) |
                                (static_cast<uint8_t>(receivedData[9])));
                uint32_t Mant = static_cast<uint16_t>((static_cast<uint8_t>(receivedData[8])<<8) |
                                (static_cast<uint8_t>(receivedData[7])) |
                                ((static_cast<uint8_t>(receivedData[6])<<8) |
                                (static_cast<uint8_t>(receivedData[5]))));
                Amperage = static_cast<float>(Mant/pow(2,EXP));
                //qDebug()<<"Amperage"<<Amperage;
                emit sendAmperageMesurements(Amperage);
                uint8_t stateWord = static_cast<uint8_t>(receivedData[3]);
                AmperageLimit al = static_cast<AmperageLimit>(stateWord & 0x3);
                AmperageState as = static_cast<AmperageState>(stateWord>>7);
                //uint8_t a_l = static_cast<uint8_t>(al);
                //uint8_t a_s = static_cast<uint8_t>(as);
                //qDebug()<<"AmperageLimit"<<a_l<<
                //          "AmperageState"<<a_s;
                if(al != amperageLimit) {
                    amperageLimit = al;
                    emit sendAmperageState(amperageState, amperageLimit);
                }
                if(as != amperageState) {
                    amperageState = as;
                    emit sendAmperageState(amperageState, amperageLimit);
                }
                //com->clear();
            }
        }
    }
}

void Uart::setLimit(uint8_t limit) {
    QByteArray data;
    for(int i =0; i<11;i++){
        data.append(requestLimit[i]);
    }
    data[3] = limit;
    data[9] = 0x51 + limit;
    write_to_com(data);
}

void Uart::setForm(bool direct) {
    QByteArray data;
    for(int i =0; i<11;i++){
        data.append(requestFormVoltage[i]);
    }
    if(direct) {
        data[3] = 0x80; data[9] = 0xCE;
    } else {
        data[3] = 0x00; data[9] = 0x4E;
    }
    write_to_com(data);
}
