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

enum class ResistorNominal {
    r0_7    = 0,
    r1_5    = 1,
    r2      = 2,
    r6      = 3,
    r25     = 4,
    r60     = 5
};


#endif // DEFINITIONS_H
