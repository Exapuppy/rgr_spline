# include "fl3.h"

fl2  operator +  (const fl2 l, const fl2 r) 
{
	return fl2(l.m[0] + r.m[0],l.m[1] + r.m[1]); 
}
fl2  operator *  (const fl2 a, const double b)
{
	return fl2(b*a.m[0],b*a.m[1]);
}
fl2  operator *  (const fl2 a, const GLfloat b) { return a*(double)b; }
fl2  operator *  (const double a, const fl2 b)  { return b*(double)a; }
fl2  operator *  (const GLfloat a, const fl2 b) { return b*(double)a; }
bool  operator == (const fl2 a, const fl2 b)
{
    return abs(a.m[0]-b.m[0]) < eps;
}
bool  operator <  (const fl2 a, const fl2 b)
{
    return a.m[0]<b.m[0];
}


fl3  operator +  (const fl3 l, const fl3 r) 
{
	return fl3(l.m[0] + r.m[0],l.m[1] + r.m[1],l.m[2] + r.m[2]); 
}
fl3  operator -  (const fl3 a)
{
	return fl3(-a.m[0],-a.m[1],-a.m[2]);
}
fl3  operator -  (const fl3 l, const fl3 r)		{ return l + (-r); }
fl3  operator *  (const fl3 a, const double b)
{
	return fl3(b*a.m[0],b*a.m[1],b*a.m[2]);
}
fl3  operator *  (const fl3 a, const GLfloat b) { return a*(double)b; }
fl3  operator *  (const double a, const fl3 b)  { return b*(double)a; }
fl3  operator *  (const GLfloat a, const fl3 b) { return b*(double)a; }