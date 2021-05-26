#include "main.h"


Sender::Sender(QObject* parent):QObject(parent){}
Sender::~Sender(){}

int main(int argc, char *argv[])
{
    Sender* sender = new Sender;
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
  //! create user types
    qRegisterMetaType<VoltageState>();
    qRegisterMetaType<AmperageState>();
    qRegisterMetaType<AmperageLimit>();
    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    //QQuickView* view = new QQuickView(url);
    //!____ Uart section ___________________________________
    QStringListModel listModel;
    engine.rootContext()->setContextProperty("listModel", &listModel); //binding QML and C++
    QThread* uartThread = new QThread;
    Uart* uart = new Uart(listModel);
    QObject::connect(uartThread,&QThread::started, uart, &Uart::initSlot);
    QObject::connect(uartThread,&QThread::finished, uart, &Uart::deleteLater);
    // get slot from another thread to main thread
    QObject::connect(uart,&Uart::sendModel,QCoreApplication::instance(),
                     [&](QStringListModel* model, QStringList* list){
        qDebug()<<"main thread: "<< QThread::currentThreadId();
        model->setStringList(*list);
        sender->setProp(0);
        //comIndex=0;
    },Qt::QueuedConnection);
    uart->moveToThread(uartThread);
    uartThread->start();

    //qDebug()<<"main thread: "<< QThread::currentThreadId();

    //QObject *item = static_cast<QObject*>(view->rootObject());

    QQmlComponent comp(&engine, QUrl(QStringLiteral("main.qml")));
    QObject* info = comp.create();


    Backend *backend = new Backend();

    QObject* connectButton = info->findChild<QObject*>("butConnect");
    QObject::connect(connectButton, SIGNAL(connect_tcp()),backend,SLOT(connectFromQml()));
    QObject::connect(connectButton, SIGNAL(disconnect_tcp()),backend,SLOT(disconnectFromQml()));
    QObject* labelLog = info->findChild<QObject*>("labelLog");
    backend->setQmlObjectLog(*labelLog);
    QObject* labelVoltageAmplitude = info->findChild<QObject*>("labelVoltageAmplitude");
    backend->setLabelVoltageAmplitude(*labelVoltageAmplitude);
    QObject* labelVoltageMean = info->findChild<QObject*>("labelVoltageMean");
    backend->setLabelVoltageMean(*labelVoltageMean);
    QObject* labelVoltageState = info->findChild<QObject*>("labelVoltageState");
    backend->setLabelVoltageState(*labelVoltageState);
    QObject* labelAmperage = info->findChild<QObject*>("labelAmperage");
    backend->setLabelAmperage(*labelAmperage);
    //! Amperage limit buttons and checkBoxes
    QObject* button_5mA = info->findChild<QObject*>("button_5mA");
    QObject::connect(button_5mA,SIGNAL(signal_5mA()),backend,SLOT(onButton_5mA()));
    QObject* button_10mA = info->findChild<QObject*>("button_10mA");
    QObject::connect(button_10mA,SIGNAL(signal_10mA()),backend,SLOT(onButton_10mA()));
    QObject* button_20mA = info->findChild<QObject*>("button_20mA");
    QObject::connect(button_20mA,SIGNAL(signal_20mA()),backend,SLOT(onButton_20mA()));
    QObject* button_50mA = info->findChild<QObject*>("button_50mA");
    QObject::connect(button_50mA,SIGNAL(signal_50mA()),backend,SLOT(onButton_50mA()));

    QObject* checkBox_5mA = info->findChild<QObject*>("checkBox_5mA");
    backend->setCheckBox_5mA(*checkBox_5mA);
    QObject* checkBox_10mA = info->findChild<QObject*>("checkBox_10mA");
    backend->setCheckBox_10mA(*checkBox_10mA);
    QObject* checkBox_20mA = info->findChild<QObject*>("checkBox_20mA");
    backend->setCheckBox_20mA(*checkBox_20mA);
    QObject* checkBox_50mA = info->findChild<QObject*>("checkBox_50mA");
    backend->setCheckBox_50mA(*checkBox_50mA);
    //! Amperage state button and images
    QObject* switch_mA = info->findChild<QObject*>("switch_mA");
    QObject::connect(switch_mA,SIGNAL(signal_switch_mA(int)),backend,SLOT(onSwitch_mA(int)));
    QObject* image_alternating = info->findChild<QObject*>("alternating_png");
    backend->setAlternating_png(*image_alternating);
    QObject* alternating_png_anim = info->findChild<QObject*>("alternating_png_anim");
    backend->setAlternating_png_anim(*alternating_png_anim);
    QObject* image_direct = info->findChild<QObject*>("direct_png");
    backend->setDirect_png(*image_direct);
    QObject* direct_png_anim = info->findChild<QObject*>("direct_png_anim");
    backend->setDirect_png_anim(*direct_png_anim);
    //! Resistor menu objects
    //QObject* resComboBox = info->findChild<QObject*>("resComboBox");
    QObject* resButSet = info->findChild<QObject*>("resButSet");
    QObject::connect(resButSet,SIGNAL(resBut_clicked(int)),backend,SLOT(onResBut(int)));
    //! UART CONNECTION OBJECTS
    QObject* comButConnect = info->findChild<QObject*>("comButConnect");
    //QObject::connect(comButConnect,SIGNAL(),backend,SLOT());
    QObject* comComboBox = info->findChild<QObject*>("comComboBox");
    QObject::connect(sender,SIGNAL(setProp(QVariant)), comComboBox, SLOT(setIndex(QVariant)));
    //QObject* comModel = info->findChild<QObject*>("comModel");
    //QObject::connect(backend, SIGNAL(sendCom(QVariant)),comModel,SLOT(addItems(QVariant)));
    //backend->setComModel(*comComboBox);
    //view->show();
    //test->msg();
    //engine.rootContext()->setContextProperty("backend", backend);
    //qmlRegisterType<Backend>("io.qt.Backend",1,0,"Backend");
    //engine.load(url);

    return app.exec();
}
