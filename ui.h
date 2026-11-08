#pragma once

#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdlib>

#include "renderer.h"
#include "drawer.h"

class UI {
private:
    termios origTerm;
    int _kbhit();
    void krya();
public:
    void runUI();
};