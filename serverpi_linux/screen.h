#ifndef SCREEN_H
#define SCREEN_H

#include "backend.h"
#include "ftdi.h"
#include "uart.h"
#include "definitions.h"

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QThread>
#include <QTimer>


class Screen : public QDialog
{
    Q_OBJECT
public:
    Screen(QWidget *parent = nullptr);
    ~Screen();
signals:
    void sendVoltageState(const VoltageState& vs);
    void sendVoltageMeasurements(const float& volt_Amp, const float& volt_Mean);
    void sendAmperageStateToUart(const AmperageState& as, const AmperageLimit& al);
    void sendAmperageStateToBackend(const AmperageState& as, const AmperageLimit& al);
    void sendAmperageMesToBackend(const float&);
    void sendToClient();
    void protocolInitSignal(); //start signal usb initialization
    void protocolDataInitSignal();
    void getRawDataSignal();
    void amperage_measuring_start();
    void amperage_measuring_stop();
private slots:
    //void receiveVoltageState(const VoltageState& vs);
    //void receiveAmperageState(const AmperageState& as, const AmperageLimit& al);


private:
    void init();
    void setInfo(const QString& str);
    QPushButton* butExit{nullptr};
    QVBoxLayout* layout{nullptr};
    QThread* threadBackend{nullptr};
    QThread* threadUSB{nullptr};
    QThread* threadUART{nullptr};
    backend* back{nullptr};
    Ftdi* ftdi{nullptr};
    Uart* uart{nullptr};
    QTimer* timerUSB_connect{nullptr};
    QTimer* timerUSB_request{nullptr};
    QTimer* timerBackEnd{nullptr};
    QTimer* timerUart{nullptr};
    QLabel* labInfo{nullptr};
    QLabel* labVoltageState{nullptr};

    float voltage_Amp = 0;
    float voltage_Mean = 0;
    float amperage = 0;
    VoltageState voltageState;
    bool Ftdi_connected = false;
    float k_kV = 162.0F;
    float k_kV_mean = 162.0F;

    bool wifi_connected = false;
    //AmperageState amperageState;
    //AmperageLimit amperageLimit;
};
#endif // SCREEN_H
