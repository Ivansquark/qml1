#include "screen.h"
#include "stdio.h"
#include "definitions.h"
#include <QApplication>
Q_DECLARE_METATYPE(VoltageState)
Q_DECLARE_METATYPE(AmperageState)
Q_DECLARE_METATYPE(AmperageLimit)
int main(int argc, char *argv[]) {
    system("./script.sh");
    QApplication a(argc, argv);

    qRegisterMetaType<VoltageState>();
    qRegisterMetaType<AmperageState>();
    qRegisterMetaType<AmperageLimit>();
    Screen w;
    w.show();
    return a.exec();
}
