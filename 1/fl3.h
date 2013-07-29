#ifndef __FL3_H
#define __FL3_H
#include "pch.h"

//декартовы координаты

class fl2
{
public:
	GLfloat m[2];
	void operator = (fl2 r)   { m[0] = r.m[0]; m[1] = r.m[1]; }
    fl2(float _1,  float _2)  { m[0] = _1    ; m[1] = _2    ; }
    fl2(double _1, double _2) { m[0] = _1    ; m[1] = _2    ; }
    fl2(int _1, int _2)       { m[0] = _1    ; m[1] = _2    ; }
	fl2()                     { m[0] = 0     ; m[1] = 0     ; }
    float operator[](int i)
    {
        if(i == 1 || i == 0)
            return m[i];
        else
            return 0;
    }
};

class fl3
{
public:
	GLfloat m[3];
	void operator= (fl3 r)
	{
		this->m[0] = r.m[0];
		this->m[1] = r.m[1];
		this->m[2] = r.m[2];
	}
	fl3(float _1, float _2, float _3 = 0)
	{
		m[0] = _1; m[1] = _2; m[2] = _3;
	}
	fl3()
	{
		m[0] = 0; m[1] = 0; m[2] = 0;
	}
    fl3(const fl2 a)
    {
        m[0] = a.m[0]; m[1] = a.m[1]; m[2] = 0;
    }
    fl3(const fl2 a, float z)
    {
        m[0] = a.m[0]; m[1] = a.m[1]; m[2] = z;
    }
	void operator += (const fl3 r);
	void operator -= (const fl3 r);
	void operator *= (const double r);
	void operator *= (const GLfloat r);
	void operator /= (const double r);
	void operator /= (const GLfloat r);
};



fl2   operator +  (const fl2     l, const fl2     r);
fl2   operator *  (const fl2     a, const double  b);
fl2   operator *  (const fl2     a, const GLfloat b);
fl2   operator *  (const double  a, const fl2     b);
fl2   operator *  (const GLfloat a, const fl2     b);
bool  operator == (const fl2 a, const fl2 b);
bool  operator <  (const fl2 a, const fl2 b);

fl3   operator +  (const fl3     l, const fl3     r);
fl3   operator -  (const fl3     a                 );
fl3   operator -  (const fl3     l, const fl3     r);
fl3   operator *  (const fl3     a, const double  b);
fl3   operator *  (const fl3     a, const GLfloat b);

#endif