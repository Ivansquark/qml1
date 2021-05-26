#ifndef UART_H
#define UART_H

#include "definitions.h"
#include <wiringPi.h>
#include <cmath>
//#include "bcm2835.h"

#include <QEventLoop>
#include <QTimer>
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>

class Uart: public QObject {
    Q_OBJECT
public:
    explicit Uart(QObject* parent = nullptr);
    virtual ~Uart();
signals:
    void sendText(const QString&);
    void sendAmperageState(const AmperageState& as, const AmperageLimit& al);
    void sendAmperageMesurements(const float&);
private slots:
    void bytesHandle(qint64 bytes);
    void read_from_com();
public slots:
    void initSlot();
    void amperage_measuring_start();
    void amperage_measuring_stop();
    void receiveAmperageState(const AmperageState& as, const AmperageLimit& al);
private:
    void setAmperageState(const AmperageState& as);
    void setAmperageLimit(const AmperageLimit& al);
    void write_to_com(const QByteArray& data);

    void setLimit(uint8_t limit);
    void setForm(bool direct);

    AmperageState amperageState = AmperageState::Alternating;
    AmperageLimit amperageLimit = AmperageLimit::mA5;
    QSerialPort* com{nullptr};
    QByteArray receivedData;
    QTimer* timerMeasure{nullptr};

    float Amperage = 0;
    bool Written = false;
    bool flag =false;

    static constexpr uint8_t measureRequest[11] {
      0x10, 0x01, 0x52, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x53, 0x16
    };
    static constexpr const uint8_t requestLimit[11] = {
        0x10, 0x01, 0x50, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x51, 0x16
    };
    static constexpr uint8_t requestFormVoltage[11] = {
        0x10, 0x01, 0x4D, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x4E, 0x16
    };
};

#endif // UART_H
