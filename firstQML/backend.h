#ifndef BACKEND_H
#define BACKEND_H

#include "definitions.h"

#include <QObject>
#include <QMessageBox>
#include <QDebug>
#include <QtNetwork>
#include <QDataStream>
#include <QTcpSocket>
#include <QTimer>
#include <QQuickItem>
class Backend: public QObject
{
    Q_OBJECT
public:
    Backend(QObject* obj=nullptr);
    ~Backend();
    const QString& getState();
        void connectToServer();
        void disconnectFromServer();
        void sendToServer(QByteArray);
        bool IsConnected{false};
        void setQmlObjectLog(QObject&);
        void setLabelVoltageAmplitude(QObject& obj);
        void setLabelVoltageMean(QObject& obj);
        void setLabelVoltageState(QObject& obj);
        void setLabelAmperage(QObject& obj);
        void setCheckBox_5mA(QObject& obj);
        void setCheckBox_10mA(QObject& obj);
        void setCheckBox_20mA(QObject& obj);
        void setCheckBox_50mA(QObject& obj);
        void setAlternating_png(QObject& obj);
        void setAlternating_png_anim(QObject& obj);
        void setDirect_png(QObject& obj);
        void setDirect_png_anim(QObject& obj);
        void setComModel(QObject& obj);
signals:
    void connectionSignal();
    void log(QString);
    void sendCom(QVariant);
public slots:
    void connectFromQml();
    void disconnectFromQml();
    void onButton_5mA();
    void onButton_10mA();
    void onButton_20mA();
    void onButton_50mA();
    void onSwitch_mA(int x);
    void onResBut(int x);
private slots:
    void isConnected();
    void readFromServer();
    void slotError(QAbstractSocket::SocketError err);
    void timeOut();
    /*!< передем серверу протокол событий от мыши и клавиатуры >*/
private:
    QTcpSocket* client{nullptr};
    QString* state{nullptr};
    QTimer* timer{nullptr};
    QObject* labelLog{nullptr};
    QObject* labelVoltageAmplitude{nullptr};
    QObject* labelVoltageMean{nullptr};
    QObject* labelVoltageState{nullptr};
    QObject* labelAmperage{nullptr};
    QObject* checkBox_5mA{nullptr};
    QObject* checkBox_10mA{nullptr};
    QObject* checkBox_20mA{nullptr};
    QObject* checkBox_50mA{nullptr};
    QObject* alternating_png{nullptr};
    QObject* alternating_png_anim{nullptr};
    QObject* direct_png{nullptr};
    QObject* direct_png_anim{nullptr};
    QObject* comModel{nullptr};

    VoltageState voltageState = VoltageState::Alternating;
    AmperageState amperageState = AmperageState::Alternating;
    AmperageLimit amperageLimit = AmperageLimit::mA5;
    ResistorNominal resistorNominal = ResistorNominal::r0_7;

    //uint8_t who=0;

    void init();
    void setLabelLogText(const QString& str);
    void setLabelVoltageAmplitudeText(const QString& str);
    void setLabelVoltageMeanText(const QString& str);
    void setLabelVoltageStateText(const QString& str);
    void setLabelAmperageText(const QString& str);
    void setCheckBox_5mA_state(bool x);
    void setCheckBox_10mA_state(bool x);
    void setCheckBox_20mA_state(bool x);
    void setCheckBox_50mA_state(bool x);
};

#endif // BACKEND_H
