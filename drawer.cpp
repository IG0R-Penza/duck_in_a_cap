#include "drawer.h"
#include "math.h"
#include "string.h"
#include <algorithm>
#include <utility>

#include <linux/kd.h>

bool Drawer::putPixel(int x, int y, int color){
    if (x>=0 && y >=0 && x<width && y<height) {
        size_t offset = (size_t)(y + fb_yOffset) * lineLength + (size_t)(x + fb_xOffset) * bytepp;
        fb[offset/4] = color;
        return true;
    }
    return false;
}

Drawer::Drawer()
{
    triangles = std::vector<Triangle>({
        Triangle(std::vector<double>({
            -20, -20, -10, 1,
            20, -20, -10, 1,
            0, 20, -10, 1
        }), 0x00FF0000),
        Triangle(std::vector<double>({
            -20, -20, 10, 1,
            20, -20, 10, 1,
            0, 20, 10, 1
        }), 0x0000FF00),
        Triangle(std::vector<double>({
            -30, 0, -15, 1,
            30, 0, -15, 1,
            0, 0, 30, 1
        }), 0x000000FF)
    });

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

Drawer::~Drawer()
{
    if (fb) ::munmap(fb, fb_size);
    if (descriptor) ::close(descriptor);
    fb = nullptr;
    descriptor = -1;
    ::ioctl(console_fd, KDSETMODE, KD_TEXT);
}

void Drawer::draw()
{
    ::memset(fb, 0, fb_size);

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

    for (Triangle& triangle : triangles){
        triangle.restore();
        Matrix& current = *triangle.getCurrent();
        current = current*scale;
        current = current*rotationX;
        current = current*rotationY;
        current = current*rotationZ;
        current = current*move;
    }
    sort(triangles.begin(), triangles.end(), [](Triangle& left, Triangle& right){return left.getAvgZ() < right.getAvgZ();});
    for (Triangle& triangle : triangles){
        Matrix& current = *triangle.getCurrent();
        int p0 = 0;
        int p1 = 1;
        int p2 = 2;

        int color = triangle.getColor();

        //p0.y <= p1.y <= p2.y
        if (current.get(p1, 1) < current.get(p0, 1)) std::swap(p0, p1);
        if (current.get(p2, 1) < current.get(p0, 1)) std::swap(p0, p2);
        if (current.get(p2, 1) < current.get(p1, 1)) std::swap(p1, p2);

        int x0 = width/2+round(current.get(p0, 0));
        int y0 = height/2-round(current.get(p0, 1));
        int x1 = width/2+round(current.get(p1, 0));
        int y1 = height/2-round(current.get(p1, 1));
        int x2 = width/2+round(current.get(p2, 0));
        int y2 = height/2-round(current.get(p2, 1));

        int dxl = abs(x2-x0);
        int dxs = abs(x1-x0);
        int sxl = x0 < x2 ? 1 : -1;
        int sxs = x0 < x1 ? 1 : -1;
        int dyl = -abs(y2-y0);
        int dys = -abs(y1-y0);
        int sy = -1;
        int errl = dxl + dyl;
        int errs = dxs + dys;

        int y = y0;
        int xl = x0;
        int xs = x0;

        bool shortReady = false;
        while (!(xs==x1 && y==y1)) {
            /*
            если надо изменить х - проверить пиксель, закрасить, изменить х
            если надо изменить у - закрасить строку с попиксельной проверкой, крутить второй брезенхем, пока тот не захочет изменить у, изменить у
            */
            if (!shortReady){
                int doubleErr = 2*errs;
                if (doubleErr >= dys) {
                    putPixel(xs, y, color);
                    errs += dys;
                    xs += sxs;
                }
                if (doubleErr <= dxs) {
                    errs += dxs;
                    //y += sy;
                    shortReady = true;
                }
            }
            else {
                int doubleErr = 2*errl;
                if (doubleErr >= dyl) {
                    putPixel(xl, y, color);
                    errl += dyl;
                    xl += sxl;
                }
                if (doubleErr <= dxl) {
                    errl += dxl;
                    int left = xl < xs ? xl : xs;
                    int right = xl < xs ? xs : xl;
                    for (int x = left; x <= right; x++) {putPixel(x, y, color);}
                    y += sy;
                    shortReady = false;
                }
            }
        }
        dxs = abs(x2-x1);
        dys = -abs(y2-y1);
        sxl = xl < x2 ? 1 : -1;
        sxs = xs < x2 ? 1 : -1;
        errs = dxs + dys;
        shortReady = false;
        while (!(xs==x2 && y==y2)) {
            if (!shortReady){
                int doubleErr = 2*errs;
                if (doubleErr >= dys) {
                    putPixel(xs, y, color);
                    errs += dys;
                    xs += sxs;
                }
                if (doubleErr <= dxs) {
                    errs += dxs;
                    //y += sy;
                    shortReady = true;
                }
            }
            else {
                int doubleErr = 2*errl;
                if (doubleErr >= dyl) {
                    putPixel(xl, y, color);
                    errl += dyl;
                    xl += sxl;
                }
                if (doubleErr <= dxl) {
                    errl += dxl;
                    int left = xl < xs ? xl : xs;
                    int right = xl < xs ? xs : xl;
                    for (int x = left; x <= right; x++) {putPixel(x, y, color);}
                    y += sy;
                    shortReady = false;
                }
            }
        }
    }
}

void Drawer::changeScale(double x_scale, double y_scale, double z_scale) {
    if (this->x_scale+x_scale <= 0 || this->y_scale+y_scale <= 0 || this->z_scale+z_scale <= 0) return;

    this->x_scale += x_scale;
    this->y_scale += y_scale;
    this->z_scale += z_scale;
}

void Drawer::move(int x, int y, int z){
    xOffset += x;
    yOffset += y;
    zOffset += z;
}

void Drawer::rotateX(double radians) {
    this->yzTurn += radians;
}

void Drawer::rotateY(double radians) {
    this->xzTurn += radians;
}

void Drawer::rotateZ(double radians) {
    this->xyTurn += radians;
}

void Drawer::restore() {
    for (int i = 0; i <= triangles.size(); i++) {
        triangles[i].restore();
    }
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
