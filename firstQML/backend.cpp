#include "backend.h"

Backend::Backend(QObject* obj): QObject(obj)
{
    init();
}

Backend::~Backend() {
    delete client;
    delete state;
    delete timer;
}
void Backend::init()
{
    state = new QString("Disconnected");
        timer = new QTimer(this);
        connect(timer,&QTimer::timeout, this, &Backend::timeOut);
        client = new QTcpSocket(this);
        connect(client,&QTcpSocket::connected,this,&Backend::isConnected);
        connect(client,&QTcpSocket::readyRead,this,&Backend::readFromServer);
        connect(client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
        connect(client,&QTcpSocket::disconnected,[this]() {
            *state="Disconnected";
            IsConnected=false;
            emit connectionSignal();
            timer->stop();
        });
}

void Backend::setLabelLogText(const QString &str) {
    labelLog->setProperty("text", str);
}

void Backend::setLabelVoltageAmplitudeText(const QString &str) {
    if(labelVoltageAmplitude) {
        labelVoltageAmplitude->setProperty("text",str);
    }
}

void Backend::setLabelVoltageMeanText(const QString &str) {
    if(labelVoltageMean) {
        labelVoltageMean->setProperty("text",str);
    }
}

void Backend::setLabelVoltageStateText(const QString &str) {
    if(labelVoltageState) {
        labelVoltageState->setProperty("text",str);
    }
}

void Backend::setLabelAmperageText(const QString &str) {
    if(labelAmperage) {
        labelAmperage->setProperty("text",str);
    }
}

void Backend::setCheckBox_5mA_state(bool x) {
    qDebug()<<"setCheckBox_5mA_state"<<x;
    if(x) {
        checkBox_5mA->setProperty("checked",true);
        setCheckBox_10mA_state(false); setCheckBox_20mA_state(false); setCheckBox_50mA_state(false);
    } else {
        checkBox_5mA->setProperty("checked",false);
    }
}
void Backend::setCheckBox_10mA_state(bool x) {
    if(x) {
        checkBox_10mA->setProperty("checked",true);
        setCheckBox_5mA_state(false); setCheckBox_20mA_state(false); setCheckBox_50mA_state(false);
    } else {
        checkBox_10mA->setProperty("checked",false);
    }
}
void Backend::setCheckBox_20mA_state(bool x) {
    if(x) {
        checkBox_20mA->setProperty("checked",true);
        setCheckBox_5mA_state(false); setCheckBox_10mA_state(false); setCheckBox_50mA_state(false);
    } else {
        checkBox_20mA->setProperty("checked",false);
    }
}
void Backend::setCheckBox_50mA_state(bool x) {
    if(x) {
        checkBox_50mA->setProperty("checked",true);
        setCheckBox_5mA_state(false); setCheckBox_10mA_state(false); setCheckBox_20mA_state(false);
    } else {
        checkBox_50mA->setProperty("checked",false);
    }
}

void Backend::connectFromQml() {
    //setCheckBox_5mA_state(true);
    qDebug()<<"connectFromQml";
    labelLog->setProperty("text", "connectFromQml");
    connectToServer();
}

void Backend::disconnectFromQml() {
    qDebug()<<"disconnectFromQml()";
    labelLog->setProperty("text", "disconnectFromQml");
    disconnectFromServer();
}

void Backend::onButton_5mA() {
    amperageLimit = AmperageLimit::mA5;
    setCheckBox_5mA_state(true);
    QByteArray data;
    data.append("mA"); data.append((uint8_t)amperageLimit); data.append((uint8_t)amperageState);
    sendToServer(data);
}
void Backend::onButton_10mA() {
    amperageLimit = AmperageLimit::mA10;
    setCheckBox_10mA_state(true);
    QByteArray data;
    data.append("mA"); data.append((uint8_t)amperageLimit); data.append((uint8_t)amperageState);
    sendToServer(data);
}
void Backend::onButton_20mA() {
    amperageLimit = AmperageLimit::mA20;
    setCheckBox_20mA_state(true);
    QByteArray data;
    data.append("mA"); data.append((uint8_t)amperageLimit); data.append((uint8_t)amperageState);
    sendToServer(data);
}
void Backend::onButton_50mA() {
    amperageLimit = AmperageLimit::mA50;
    setCheckBox_50mA_state(true);
    QByteArray data;
    data.append("mA"); data.append((uint8_t)amperageLimit); data.append((uint8_t)amperageState);
    sendToServer(data);
}

void Backend::onSwitch_mA(int x) {
    AmperageState a;
    if(x){
        a = AmperageState::Direct;
        //setCheckBox_mA_state_state(AmperageState::Direct);//(const AmperageState&)(x));

    } else {
        a = AmperageState::Alternating;
        //setCheckBox_mA_state_state(AmperageState::Alternating);
    }
    QByteArray data;
    data.append("mA"); data.append((uint8_t)amperageLimit); data.append((uint8_t)a);
    sendToServer(data);
}

void Backend::onResBut(int x) {
    resistorNominal = static_cast<ResistorNominal>(x);
    qDebug()<<(int)resistorNominal;
}

void Backend::isConnected() {
    *state = "Connected";
        IsConnected = true;
        emit connectionSignal();
        timer->start(500);
        qDebug() << "isConnected";
        labelLog->setProperty("text", "Client isConnected to server");
}
/*!< Slot started when received data from server >*/
void Backend::readFromServer() {
    if((client->state() == QAbstractSocket::ConnectedState)) {
        QDataStream in(client);
        in.setVersion(QDataStream::Qt_5_11);
        QByteArray receivedBytes;
        uint8_t data_size=0;
        if(0 == data_size){
            in>>data_size; // get first byte with size of received packet
        }
        if(client->bytesAvailable() < data_size) {
            return;
        }
        uint8_t count = client->bytesAvailable();
        uint8_t temp = 0;
        for(int i=0;i<count;i++) {
            in>>temp;
            receivedBytes.append(temp);
        }
        qDebug()<<receivedBytes;
        setLabelLogText(receivedBytes);
        QString amp;
        for(int i = 0; i<8; i++){
            amp += (receivedBytes[i]);
        }
        QString volt;
        for(int i = 0; i<7; i++){
            volt += (receivedBytes[i+14]);
        }
        if(amp == "amperage" && volt == "voltage") {
            uint32_t a = ((uint8_t)receivedBytes[8] | (uint8_t)receivedBytes[9]<<8 |
                           (uint8_t)receivedBytes[10]<<16 | (uint8_t)receivedBytes[11]<<24);
            uint8_t a_state = (uint8_t)receivedBytes[12];

            uint8_t a_limit = (uint8_t)receivedBytes[13];

            uint32_t v_a = ((uint8_t)receivedBytes[21] | (uint8_t)receivedBytes[22]<<8 |
                           (uint8_t)receivedBytes[23]<<16 | (uint8_t)receivedBytes[24]<<24);

            uint32_t v_m = ((uint8_t)receivedBytes[25] | (uint8_t)receivedBytes[26]<<8 |
                           (uint8_t)receivedBytes[27]<<16 | (uint8_t)receivedBytes[28]<<24);
            float amperage = *reinterpret_cast<float*>(&a);
            float voltage_amp = *reinterpret_cast<float*>(&v_a);
            float voltage_mean = *reinterpret_cast<float*>(&v_m);
            setLabelVoltageAmplitudeText(QString::number(voltage_amp,'f',2));
            setLabelVoltageMeanText(QString::number(voltage_mean,'f',2));
            setLabelAmperageText(//QString::number((uint8_t)receivedBytes[19]) + " " +
                                //QString::number((uint8_t)receivedBytes[20]) + " " +
                                //QString::number((uint8_t)receivedBytes[21]) + " " +
                                //QString::number((uint8_t)receivedBytes[22]) + " " +
                                QString::number(amperage,'f',2));
            if(amperageState != static_cast<AmperageState>(a_state)) {
                amperageState = static_cast<AmperageState>(a_state);
                if(a_state) {
                    qDebug()<<"direct_png";
                    alternating_png->setProperty("visible", false);
                    direct_png->setProperty("visible", true);
                    direct_png_anim->setProperty("running", true);
                } else {
                    qDebug()<<"alternate_png";
                    direct_png->setProperty("visible",false);
                    alternating_png->setProperty("visible", true);
                    alternating_png_anim->setProperty("running", true);
                }
            }
            uint8_t y = static_cast<uint8_t>(amperageState);
            qDebug()<< "a_state=" <<a_state<<"amperageState= "<<y;
            uint8_t x = static_cast<uint8_t>(amperageLimit);
            qDebug()<< "a_limit=" <<a_limit<<"amperageLimit= "<<x;
            if(amperageLimit != static_cast<AmperageLimit>(a_limit)) {
                switch(a_limit) {
                    case 0: setCheckBox_5mA_state(true);  amperageLimit = AmperageLimit::mA5; break;
                    case 1: setCheckBox_10mA_state(true); amperageLimit = AmperageLimit::mA10; break;
                    case 2: setCheckBox_20mA_state(true); amperageLimit = AmperageLimit::mA20; break;
                    case 3: setCheckBox_50mA_state(true); amperageLimit = AmperageLimit::mA50; break;
                    default: break;
                }
            }
            if((uint8_t)receivedBytes[29] == 0) {
                setLabelVoltageStateText("Переменное");
            } else {
                setLabelVoltageStateText("Постоянное");
            }
            //qDebug() << receivedBytes;
        }
    }
}

void Backend::slotError(QAbstractSocket::SocketError err) {
    QString strError = "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                                "The host was not found." :
                                                err == QAbstractSocket::RemoteHostClosedError ?
                                                    "The remote host is closed." :
                                                    err == QAbstractSocket::ConnectionRefusedError?
                                                        "The connection was refused." :
                                                        QString(client->errorString()));
    state->append(strError);
}

void Backend::timeOut() {
    if((client->state() == QAbstractSocket::ConnectedState)) {
            QByteArray arr;
        }
}

void Backend::connectToServer() {
    qDebug()<< "connectToServer";
    labelLog->setProperty("text", "connectToServer");
    client->connectToHost("169.254.176.140",55555);
}

void Backend::disconnectFromServer() {
    qDebug()<< "disconnectFromServer";
    labelLog->setProperty("text", "disconnectFromServer");
    if(client->state() == QAbstractSocket::ConnectedState) {
        client->disconnectFromHost();
        IsConnected = false;
    }
}

void Backend::sendToServer(QByteArray data) {
    if(client->state() == QAbstractSocket::ConnectedState) {
        client->write(data);
        qDebug()<<data;
    }
}

void Backend::setQmlObjectLog(QObject & log) {
    labelLog = &log;
}

void Backend::setLabelAmperage(QObject &obj) {
    labelAmperage =&obj;
}


void Backend::setCheckBox_5mA(QObject &obj) {
    checkBox_5mA = &obj;
}
void Backend::setCheckBox_10mA(QObject &obj) {
    checkBox_10mA = &obj;
}
void Backend::setCheckBox_20mA(QObject &obj) {
    checkBox_20mA = &obj;
}
void Backend::setCheckBox_50mA(QObject &obj) {
    checkBox_50mA = &obj;
}

void Backend::setAlternating_png(QObject &obj) {
    alternating_png = &obj;
}

void Backend::setAlternating_png_anim(QObject &obj) {
    alternating_png_anim = &obj;
}

void Backend::setDirect_png(QObject &obj) {
    direct_png = &obj;
}

void Backend::setDirect_png_anim(QObject &obj) {
    direct_png_anim = &obj;
}

void Backend::setComModel(QObject &obj) {
    comModel = &obj;
    QString str = "Com1";
    emit sendCom(QVariant(str));
}

void Backend::setLabelVoltageAmplitude(QObject& obj) {
    labelVoltageAmplitude = &obj;
}
void Backend::setLabelVoltageMean(QObject& obj) {
    labelVoltageMean = &obj;
}

void Backend::setLabelVoltageState(QObject &obj) {
    labelVoltageState = &obj;
}
