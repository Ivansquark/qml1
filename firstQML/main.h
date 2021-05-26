#ifndef MAIN_H
#define MAIN_H

#include "uart.h"
#include "backend.h"
#include "definitions.h"
//#include "main.moc"

#include <QThread>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QMessageBox>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickView>
#include <QtQuick>
#include <QObject>


Q_DECLARE_METATYPE(VoltageState)
Q_DECLARE_METATYPE(AmperageState)
Q_DECLARE_METATYPE(AmperageLimit)
class Sender: public QObject {
    Q_OBJECT
public:
    Sender(QObject* parent = nullptr);
    virtual ~Sender();
signals:
    void setProp(QVariant x);
};

#endif // MAIN_H
