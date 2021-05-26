#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QObject>

enum class VoltageState {
    Alternating = 0,
    Direct = 1
};

enum class AmperageState {
    Alternating = 0,
    Direct = 1
};

enum class AmperageLimit {
    mA5 = 0,
    mA10 = 1,
    mA20 = 2,
    mA50 = 3
};

#endif // DEFINITIONS_H
