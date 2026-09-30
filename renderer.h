#pragma once
#include "matrix.h"

#include <cstdint>
#include <cstddef>
#include <vector>
#include <linux/fb.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

class Renderer{
private:
    Matrix initial;
    Matrix current;
    Matrix lines;
    int xOffset = 0;
    int yOffset = 0;
    double xyTurn = 0;
    double x_scale = 1;
    double y_scale = 1;

    int descriptor = -1;
    uint32_t *fb = nullptr;
    int width = 0;
    int height = 0;
    int bpp = 0;
    int bytepp = 0;
    int lineLength = 0;
    int fb_xOffset = 0;
    int fb_yOffset = 0;
    size_t fb_size = 0;
    int console_fd;
public:
    Renderer();
    ~Renderer();
    void draw();
    void erase();
    void rotate(double radians);
    void changeScale(double x_scale, double y_scale);
    void move(int x, int y);
    void restore();
};