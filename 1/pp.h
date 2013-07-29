#include "pch.h"
#include "fl3.h"

class pp
{
    static float z;
public:
    static int height;
    static int zoom;
    static int eyex, eyey;

    pp(int height, int zoom = 0, int eyex = 0, int eyey = 0)
    {
        this->height = height;
        this->zoom = zoom;
        this->eyex = eyex;
        this->eyey = eyey;

        z = (height>>1)*(0.24)/(1.0 - zoom*0.01);
    };
    static const void update()
    {
        z = (height>>1)*(0.24)/(1.0 - zoom*0.01);
    }
    static const fl2 p(int x, int y)
    {
        return fl2((x+eyex)*z,(y+eyey)*z);
    };
    static const fl2 p(fl2 xy)
    {
        return fl2(xy.m[0]*z,xy.m[1]*z);
    };
    static const fl2 rp(int x, int y)
    {
        return fl2(x/z-eyex , y/z-eyey);
    };
};

float pp::z      = 0;
int   pp::height = 0;
int   pp::zoom   = 0;
int   pp::eyex   = 0;
int   pp::eyey   = 0;