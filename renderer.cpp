#include "renderer.h"
#include "math.h"
#include "string.h"

#include <linux/kd.h>

Renderer::Renderer(){
    this->initial = Matrix(55, 4, std::vector<double>({
        -6, 11, 2, 1,
        -1, 11, 2, 1,
        -10, 9, 2, 1,
        -7, 9, 2, 1,
        -1, 9, 2, 1,
        -4, 8, 2, 1,
        -2, 8, 2, 1,
        -11, 7, 2, 1,
        -7, 7, 2, 1,
        -4, 6, 2, 1,
        -2, 6, 2, 1,
        -11, 5, 2, 1,
        -7, 5, 2, 1,
        -7, 4, 2, 1,
        -6, 4, 2, 1,
        -1, 4, 2, 1,
        11, 3, 0, 1,
        -8, 0, 4, 1,
        -2, 0, 4, 1,
        6, 0, 4, 1,
        11, 0, 4, 1,
        -2, -6, 7, 1,
        -5, -8, 4, 1,
        0, -8, 2, 1,
        5, -8, 4, 1,
        -3, -11, 4, 1,
        0, -11, 2, 1,
        1, -11, 2, 1,

        -6, 11, -2, 1,
        -1, 11, -2, 1,
        -10, 9, -2, 1,
        -7, 9, -2, 1,
        -1, 9, -2, 1,
        -4, 8, -2, 1,
        -2, 8, -2, 1,
        -11, 7, -2, 1,
        -7, 7, -2, 1,
        -4, 6, -2, 1,
        -2, 6, -2, 1,
        -11, 5, -2, 1,
        -7, 5, -2, 1,
        -7, 4, -2, 1,
        -6, 4, -2, 1,
        -1, 4, -2, 1,

        -8, 0, -4, 1,
        -2, 0, -4, 1,
        6, 0, -4, 1,
        11, 0, -4, 1,
        -2, -6, -7, 1,
        -5, -8, -4, 1,
        0, -8, -2, 1,
        5, -8, -4, 1,
        -3, -11, -4, 1,
        0, -11, -2, 1,
        1, -11, -2, 1,
    }));
    this->lines = Matrix(88, 2, std::vector<double>({
        0, 1,
        2, 3,
        3, 4,
        0, 3,
        1, 4,
        3, 8,
        8, 12,
        12, 13,
        13, 14,
        4, 15,
        7, 8,
        11, 12,
        7, 11,
        5, 6,
        6, 10,
        10, 9,
        9, 5,
        14, 17,
        15, 18,
        18, 19,
        18, 21,
        21, 19,
        19, 16,
        16, 20,
        20, 24,
        24, 22,
        22, 17,
        23, 26,
        26, 25,
        26, 27,

        28, 29,
        30, 31,
        31, 32,
        28, 31,
        29, 32,
        31, 36,
        36, 40,
        40, 41,
        41, 42,
        32, 43,
        35, 36,
        39, 40,
        35, 39,
        33, 34,
        34, 38,
        38, 37,
        37, 33,
        42, 44,
        43, 45,
        45, 46,
        45, 48,
        48, 46,
        46, 16,
        16, 47,
        47, 51,
        51, 49,
        49, 44,
        50, 53,
        53, 52,
        53, 54,

        0, 28,
        1, 29,
        2, 30,
        3, 31,
        4, 32,
        7, 35,
        8, 36,
        11, 39,
        12, 40,
        13, 41,
        14, 42,
        15, 43,
        17, 44,
        18, 45,
        19, 46,
        20, 47,
        22, 49,
        23, 50,
        24, 51,
        25, 52,
        26, 53,
        27, 54,

        14, 15,
        17, 18,
        19, 20,
        42, 43,
        44, 45,
        46, 47
    }));
    this->current = this->initial;
    this->x_scale = 1;
    this->y_scale = 1;
    this->z_scale = 1;
    this->xOffset = 0;
    this->yOffset = 0;
    this->zOffset = 0;
    this->xyTurn = 0;
    this->xzTurn = 0;
    this->yzTurn = 0;

    console_fd = ::open("/dev/tty3", O_RDWR);////??????
    ::ioctl(console_fd, KDSETMODE, KD_GRAPHICS);

    descriptor = ::open("/dev/fb0", O_RDWR);
    fb_var_screeninfo fb_var_screeninfo{};
    fb_fix_screeninfo fb_fix_screeninfo{};
    ::ioctl(descriptor, FBIOGET_VSCREENINFO, &fb_var_screeninfo);
    ::ioctl(descriptor, FBIOGET_FSCREENINFO, &fb_fix_screeninfo);

    width = fb_var_screeninfo.xres;
    height = fb_var_screeninfo.yres;
    bpp = fb_var_screeninfo.bits_per_pixel;
    lineLength = fb_fix_screeninfo.line_length;
    fb_xOffset = fb_var_screeninfo.xoffset;
    fb_yOffset = fb_var_screeninfo.yoffset;
    bytepp = bpp/8;
    fb_size = fb_fix_screeninfo.smem_len;

    fb = (uint32_t*)::mmap(nullptr, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);
}

Renderer::~Renderer() {
    if (fb) ::munmap(fb, fb_size);
    if (descriptor) ::close(descriptor);
    fb = nullptr;
    descriptor = -1;
    ::ioctl(console_fd, KDSETMODE, KD_TEXT);
}

void Renderer::draw() {
    Matrix rotationX = Matrix(4, 4, std::vector<double>({
        1, 0, 0, 0,
        0, cos(yzTurn), sin(yzTurn), 0,
        0, -sin(yzTurn), cos(yzTurn), 0,
        0, 0, 0, 1
    }));

    Matrix rotationY = Matrix(4, 4, std::vector<double>({
        cos(xzTurn), 0, -sin(xzTurn), 0,
        0, 1, 0, 0,
        sin(xzTurn), 0, cos(xzTurn), 0,
        0, 0, 0, 1
    }));

    Matrix rotationZ = Matrix(4, 4, std::vector<double>({
        cos(xyTurn), sin(xyTurn), 0, 0,
        -sin(xyTurn), cos(xyTurn), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    }));

    Matrix scale = Matrix(4, 4, std::vector<double>({
        x_scale, 0, 0, 0,
        0, y_scale, 0, 0,
        0, 0, z_scale, 0,
        0, 0, 0, 1
    }));

    Matrix move = Matrix(4, 4, std::vector<double>({
        1, 0, 0, (double)xOffset,
        0, 1, 0, (double)yOffset,
        0, 0, 1, (double)zOffset,
        0, 0, 0, 1
    }));

    current = initial*scale;
    current = current*rotationX;
    current = current*rotationY;
    current = current*rotationZ;
    current = current*move;

    try {
        ::memset(fb, 0, fb_size);
        for (int line = 0; line < lines.getLines(); line++){
            
            int x1 = width/2+round(current.get(lines.get(line, 0), 0));
            int y1 = height/2-round(current.get(lines.get(line, 0), 1));
            int x2 = width/2+round(current.get(lines.get(line, 1), 0));
            int y2 = height/2-round(current.get(lines.get(line, 1), 1));

            int dx = abs(x2-x1);
            int sx = x1 < x2 ? 1 : -1;
            int dy = -abs(y2-y1);
            int sy = y1 < y2 ? 1 : -1;
            int err = dx + dy;

            while (!(x1==x2 && y1==y2)){
                if (x1>=0 && y1 >=0 && x1<width && y1<height) {
                    size_t offset = (size_t)(y1 + fb_yOffset) * lineLength + (size_t)(x1 + fb_xOffset) * bytepp;
                    fb[offset/4] = 0x00FFFF00;
                }
                int doubleErr = 2*err;
                if (doubleErr >= dy) {
                    err += dy;
                    x1 += sx;
                }
                if (doubleErr <= dx) {
                    err += dx;
                    y1 += sy;
                }
            }
        }
    }
    catch (...){ ::ioctl(console_fd, KDSETMODE, KD_TEXT);}
}

void Renderer::changeScale(double x_scale, double y_scale, double z_scale) {
    if (this->x_scale+x_scale <= 0 || this->y_scale+y_scale <= 0 || this->z_scale+z_scale <= 0) return;

    this->x_scale += x_scale;
    this->y_scale += y_scale;
    this->z_scale += z_scale;
}

void Renderer::move(int x, int y, int z){
    xOffset += x;
    yOffset += y;
    zOffset += z;
}

void Renderer::rotateX(double radians) {
    this->yzTurn += radians;
}

void Renderer::rotateY(double radians) {
    this->xzTurn += radians;
}

void Renderer::rotateZ(double radians) {
    this->xyTurn += radians;
}

void Renderer::restore() {
    this->current = this->initial;
    this->x_scale = 1;
    this->y_scale = 1;
    this->z_scale = 1;
    this->xOffset = 0;
    this->yOffset = 0;
    this->zOffset = 0;
    this->xyTurn = 0;
    this->xzTurn = 0;
    this->yzTurn = 0;
}