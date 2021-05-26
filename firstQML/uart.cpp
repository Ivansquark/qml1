#include "uart.h"

Uart::Uart(QStringListModel& model, QObject* parent) :QObject(parent), comModel(&model) {

}

Uart::~Uart(){
    delete portName;
}

void Uart::initSlot() {
    qDebug()<<"uart thread: "<< QThread::currentThreadId();
    portName = new QString;
    comInfo = new QSerialPortInfo;
    com = new QSerialPort(this);
    com->setBaudRate(QSerialPort::Baud9600);
    com->setDataBits(QSerialPort::Data8);
    com->setStopBits(QSerialPort::OneStop);
    com->setParity(QSerialPort::NoParity);
    com->setFlowControl(QSerialPort::NoFlowControl);
    comList = new QStringList;
    timerConnect = new QTimer(this);
    connect(timerConnect, &QTimer::timeout, this,[this](){
        QStringList tempList = getPorts();
        //qDebug()<<"tempList"<<tempList;
        //qDebug()<<"comList"<<*comList;
        if(*comList != tempList) {
            *comList = tempList;
            //comModel->setStringList(*comList);
            //qDebug()<<"comModel"<<*comList;
            emit sendModel(comModel, comList);
        }
    });
    timerConnect->start(1000);
}

void Uart::finishSlot() {

}

void Uart::connectSlot(bool isConnected) {
    if(isConnected) {
        com->setPortName(*portName);
        qDebug()<<"port name " << *portName;
        qDebug()<<"uart thread: "<< QThread::currentThreadId();
        if(com->open(QIODevice::ReadWrite)) {
            QString qs("UART Connected");
            QVariant qv(qs);
            emit setLog(qv);
            emit sendConnectedState(true);
        } else {
            qDebug()<<"unable open com port";
        }
    } else {
        if(com->isOpen()) {
            com->close();
            QString qs("UART Disonnected");
            QVariant qv(qs);
            emit setLog(qs);
            emit sendConnectedState(false);

        }
    }
}

void Uart::receivePortName(QVariant str) {
    *portName = str.toString();
}

QStringList Uart::getPorts() {
    QStringList list;
    for(auto i : comInfo->availablePorts()) {
           list.append(i.portName());
    }
    return list;
}
void Uart::writeToCom(const QByteArray &data) {
    if(com->isOpen()) {
        com->write(data);
        qDebug()<<"write_to_com"<<data;
        //Written = false;
        com->waitForBytesWritten(1000);
    }
}
void Uart::readFromCom() {
    if(com->isOpen()) {
        qint64 count = com->bytesAvailable();
        qDebug()<<"read from com bytes"<<count;
        QByteArray receivedData = com->readAll();
    }
}
