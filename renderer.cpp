#include "renderer.h"
#include "math.h"
#include "string.h"

#include <linux/kd.h>

Renderer::Renderer(){
    this->initial = Matrix(28, 3, std::vector<double>({
        -6, 11, 1,
        -1, 11, 1,
        -10, 9, 1,
        -7, 9, 1,
        -1, 9, 1,
        -4, 8, 1,
        -2, 8, 1,
        -11, 7, 1,
        -7, 7, 1,
        -4, 6, 1,
        -2, 6, 1,
        -11, 5, 1,
        -7, 5, 1,
        -7, 4, 1,
        -6, 4, 1,
        -1, 4, 1,
        11, 3, 1,
        -8, 0, 1,
        -2, 0, 1,
        6, 0, 1,
        11, 0, 1,
        -2, -6, 1,
        -5, -8, 1,
        0, -8, 1,
        5, -8, 1,
        -3, -11, 1,
        0, -11, 1,
        1, -11, 1
    }));
    this->lines = Matrix(31, 2, std::vector<double>({
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
        24, 23,
        23, 22,
        22, 17,
        23, 26,
        26, 25,
        26, 27
    }));
    this->current = this->initial;
    this->x_scale = 1;
    this->y_scale = 1;
    this->xOffset = 0;
    this->yOffset = 0;
    this->xyTurn = 0;

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
    Matrix rotation = Matrix(3, 3, std::vector<double>({
        cos(xyTurn), sin(xyTurn), 0,
        -sin(xyTurn), cos(xyTurn), 0,
        0, 0, 1
    }));

    Matrix scale = Matrix(3, 3, std::vector<double>({
        x_scale, 0, 0,
        0, y_scale, 0,
        0, 0, 1
    }));

    Matrix move = Matrix(3, 3, std::vector<double>({
        1, 0, (double)xOffset,
        0, 1, (double)yOffset,
        0, 0, 1
    }));

    current = initial*scale;
    current = current*rotation;
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

void Renderer::changeScale(double x_scale, double y_scale) {
    if (this->x_scale+x_scale <= 0 || this->y_scale+y_scale <= 0) return;

    this->x_scale += x_scale;
    this->y_scale += y_scale;
}

void Renderer::move(int x, int y){
    xOffset += x;
    yOffset += y;
}

void Renderer::rotate(double radians) {
    this->xyTurn += radians;
}

void Renderer::restore() {
    this->current = this->initial;
    this->x_scale = 1;
    this->y_scale = 1;
    this->xOffset = 0;
    this->yOffset = 0;
    this->xyTurn = 0;
}