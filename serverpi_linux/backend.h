#ifndef BACKEND_H
#define BACKEND_H

#include "definitions.h"

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QtNetwork>
#include <QNetworkInterface>

class backend: public QObject
{
    Q_OBJECT
public:
    explicit backend(QObject* parent = nullptr);
    virtual ~backend();
signals:
    void sendText(const QString&);
    void sendAmperageStateToScreen(const AmperageState& as, const AmperageLimit& al);
    void clientConnectedSignal();
    void clientDisconnectedSignal();
public slots:
    void initSlot();
    void sendSlot();
    void receiveVoltageMeasurements(const float& vol_Amp, const float& vol_Mean);
    void receiveVoltageState(const VoltageState& vs);
    void receiveAmperageState(const AmperageState& as, const AmperageLimit& al);
    void receiveAmperageMeasurements(const float& amp);

private slots:
    void newConnection();
    void readClient();
private:
    void sendToClient(QTcpSocket& socket);

    QTcpServer* server{nullptr};
    QTcpSocket* client{nullptr};
    QString* IPs{nullptr};
    float voltage_Amp = 0;
    float voltage_Mean = 0;
    float amperage = 0;
    VoltageState voltageState;
    AmperageState amperageState = AmperageState::Direct;
    AmperageLimit amperageLimit = AmperageLimit::mA5;
    uint16_t Port = 55555;

};

#endif // BACKEND_H
