#pragma once
#include "Mode.h"
#include "SelectMapMode.h"

class ModeFactory {
public:
    static Mode* CreateSelectMapMode() {
        return new SelectMapMode();
    }
};
