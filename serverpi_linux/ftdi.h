#ifndef FTDI_H
#define FTDI_H

//#include "Windows.h"
#include "ftd2xx.h"
#include "definitions.h"

#include <cmath>
#include <QObject>
#include <QDebug>
#include <QThread>

class Ftdi:public QObject
{
    Q_OBJECT
public:
    explicit Ftdi(QObject* obj = nullptr);
    virtual ~Ftdi();
    Ftdi(const Ftdi& usb) = delete;
    const Ftdi& operator=(const Ftdi&) = delete;
    Ftdi(Ftdi&& usb) = delete;
    Ftdi&& operator=(Ftdi&&) = delete;
signals:
    void sendText(const QString& str);
    //void sendAmplitude(const QString& str);
    //void sendMean(const QString& str);
    //void sendCoef(const QString& str);
    //void sendOffset(const QString& str);
    void connectedSignal(const bool& flag);
    void sendVoltageState(const VoltageState& vs);
public slots:
    void initSlot();
    void protocolInitSlot();
    void protocolDataInitSlot();
    void getRawDataSlot();
public:
    bool Connected = false;
    bool protocolInitialized = false;
    //uint32_t Coef = 0;
    QString Coef="";
    QString Offset="";
    QString Amplitude_str="";
    QString Mean_str="";
    float Amplitude = 0;
    float Mean = 0;

    void getRawData();
    void protocolInit();
    void protocolDataInit();
private:
    VoltageState voltageState = VoltageState::Alternating;
    VoltageState voltageStateSet;

    //!__________________ FTDI LIB ______________________________________________________________


    char    UserDevice0_TransmitterBuffer[5]={0};        //Буфер передатчика
    char    UserDevice0_ReceiverBuffer[16386] = {0};           //Буфер приемника

    FT_STATUS ftStatus;                             //Результат работы функции
    DWORD dwNumDevs=0;                              //Количество устройств, подключенных к ПК
    FT_DEVICE_LIST_INFO_NODE devInfo;              //Структура, описывающая устройство
    FT_STATUS ftStatus_GetInfoDetail;               //Результат работы функции FT_GetDeviceInfoDetail
    FT_HANDLE ftHandleTemp;                         //Переменные, в которые помещают поля структуры devInfo
    DWORD Flags;
    DWORD ID;
    DWORD Type;
    DWORD LocId;
    char SerialNumber[16];
    char Description[64];

    FT_HANDLE       UserDevice0_Handle;             //Дескриптор нашего устройства
    char UserDevice0_SN[16]="ST2T5R1H";             //Серийный номер устройства

    DWORD   UserDevice0_BytesWritten;               //Фактически записано байтов в устройство
    DWORD   UserDevice0_BytesRead;                  //Фактически прочитано байтов из устройства
    FT_STATUS       UserDevice0_WriteStatus;
    FT_STATUS       UserDevice0_ReadStatus;

    void usb_serial_init();


    void resetRequest();
    void sendInfoRequest();
    void readInfoAnswer();
    void sendRawDataRequest();
    void readRawDataAnswer();
    void send_42_04_d1_Request();
    void read_42_04_d1_Answer();
    void send_42_04_d2_Request();
    void read_42_04_d2_Answer();
    void send_42_04_d3_Request();
    void read_42_04_d3_Answer();
    void send_42_04_d4_Request();
    void read_42_04_d4_Answer();
    void send_42_04_d5_Request();
    void read_42_04_d5_Answer();
    void send_42_04_d6_Request();
    void read_42_04_d6_Answer();
    void send_42_04_d7_Request();
    void read_42_04_d7_Answer();
    void clearReceivedBuffer() {
        memset(UserDevice0_ReceiverBuffer,0,sizeof(UserDevice0_ReceiverBuffer));
    }
    void clearTransmittedBuffer() {
        memset(UserDevice0_TransmitterBuffer,0,sizeof(UserDevice0_TransmitterBuffer));
    }
    void countOffset(); //d2
    void countCoef();   //d3
    void countAmplitude();   // max value in array
    void countMeanAverage(); //integral of abs values
};

#endif // FTDI_H
