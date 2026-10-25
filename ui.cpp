#include "ui.h"
#include <math.h>

int UI::_kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;
 
    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        origTerm = term;
        term.c_lflag &= ~ICANON;
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }
 
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void UI::runUI(){
    int mode = 0;//0 - move, 1 - rotate, 2 - scale
    bool drawerEnabled = false;
    Renderer renderer = Renderer();
    Drawer drawer = Drawer();
    renderer.draw();
    while (true){
        if (_kbhit()){
            char buffer[4];
            read(STDIN_FILENO, buffer, 4);
            switch (buffer[0]) {
                case 'm':
                    mode = 0;
                    break;
                case 'r':
                    mode = 1;
                    break;
                case 's':
                    mode = 2;
                    break;
                case '7':
                    switch (mode){
                        case 0:
                            renderer.move(-2, 0, 0);
                            drawer.move(-2, 0, 0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 1:
                            renderer.rotateX(-5*M_PI/180.0);
                            drawer.rotateX(-5*M_PI/180.0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 2:
                            renderer.changeScale(-2, 0, 0);
                            drawer.changeScale(-2, 0, 0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        default:
                            break;
                    }
                    break;
                case '9':
                    switch (mode){
                        case 0:
                            renderer.move(2, 0, 0);
                            drawer.move(2, 0, 0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 1:
                            renderer.rotateX(5*M_PI/180.0);
                            drawer.rotateX(5*M_PI/180.0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 2:
                            renderer.changeScale(2, 0, 0);
                            drawer.changeScale(2, 0, 0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        default:
                            break;
                    }
                    break;
                case '4':
                    switch (mode){
                        case 0:
                            renderer.move(0, -2, 0);
                            drawer.move(0, -2, 0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 1:
                            renderer.rotateY(-5*M_PI/180.0);
                            drawer.rotateY(-5*M_PI/180.0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 2:
                            renderer.changeScale(0, -2, 0);
                            drawer.changeScale(0, -2, 0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        default:
                            break;
                    }
                    break;
                case '6':
                    switch (mode){
                        case 0:
                            renderer.move(0, 2, 0);
                            drawer.move(0, 2, 0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 1:
                            renderer.rotateY(5*M_PI/180.0);
                            drawer.rotateY(5*M_PI/180.0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 2:
                            renderer.changeScale(0, 2, 0);
                            drawer.changeScale(0, 2, 0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        default:
                            break;
                    }
                    break;
                case '1':
                    switch (mode){
                        case 0:
                            renderer.move(0, 0, -2);
                            drawer.move(0, 0, -2);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 1:
                            renderer.rotateZ(-5*M_PI/180.0);
                            drawer.rotateZ(-5*M_PI/180.0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 2:
                            renderer.changeScale(0, 0, -2);
                            drawer.changeScale(0, 0, -2);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        default:
                            break;
                    }
                    break;
                case '3':
                    switch (mode){
                        case 0:
                            renderer.move(0, 0, 2);
                            drawer.move(0, 0, 2);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 1:
                            renderer.rotateZ(5*M_PI/180.0);
                            drawer.rotateZ(5*M_PI/180.0);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        case 2:
                            renderer.changeScale(0, 0, 2);
                            drawer.changeScale(0, 0, 2);
                            if (!drawerEnabled) renderer.draw();
                            else drawer.draw();
                            break;
                        default:
                            break;
                    }
                    break;
                case '-':
                    renderer.changeScale(-2, -2, -2);
                    drawer.changeScale(-2, -2, -2);
                    if (!drawerEnabled) renderer.draw();
                    else drawer.draw();
                    break;
                case '+':
                    renderer.changeScale(2, 2, 2);
                    drawer.changeScale(2, 2, 2);
                    if (!drawerEnabled) renderer.draw();
                    else drawer.draw();
                    break;
                case '`':
                    renderer.restore();
                    drawer.restore();
                    if (!drawerEnabled) renderer.draw();
                    else drawer.draw();
                    break;
                case 'p':
                    renderer.toggleProection();
                    if (!drawerEnabled) renderer.draw();
                    break;
                case 'f':
                    drawerEnabled = !drawerEnabled;
                    if (!drawerEnabled) renderer.draw();
                    else drawer.draw();
                    break;
                case 'q':
                    tcsetattr(STDIN_FILENO, TCSANOW, &origTerm);
                    return;
                default:
                    break;
            }
        }
    }
}