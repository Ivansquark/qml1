#ifndef UART_H
#define UART_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QStringList>
#include <QStringListModel>
#include <QThread>
#include <QDebug>

class Uart: public QObject
{
    Q_OBJECT
public:
    Uart(QStringListModel& model, QObject* parent = nullptr);
    ~Uart();
    void action(void);
signals:
    void sendModel(QStringListModel* model, QStringList* list);
    void setLog(QVariant var);
    void sendConnectedState(QVariant);
public slots:
    void initSlot();
    void finishSlot();
    void connectSlot(bool isConnected);
    void receivePortName(QVariant str);
private:
    void init();
    QSerialPort* com{nullptr};
    QSerialPortInfo* comInfo{nullptr};
    QTimer* timerConnect{nullptr};
    QString* portName{nullptr};
    QStringList* comList{nullptr};
    QStringListModel* comModel{nullptr};
    QStringList getPorts();

    void writeToCom(const QByteArray &data);
    void readFromCom();
};

#endif // UART_H
