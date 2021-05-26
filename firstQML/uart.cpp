#include "uart.h"

Uart::Uart(QStringListModel& model, QObject* parent) :QObject(parent), comModel(&model) {

}

Uart::~Uart(){
}

void Uart::initSlot() {
    qDebug()<<"uart thread: "<< QThread::currentThreadId();
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

QStringList Uart::getPorts() {
    QStringList list;
    for(auto i : comInfo->availablePorts()) {
           list.append(i.portName());
    }
    return list;
}
