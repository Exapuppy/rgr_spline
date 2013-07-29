#ifndef __SPLINE_H
#define __SPLINE_H

#include "pch.h"
#include "fl3.h"

class Spline;

//матрица вторых производных
const static float M[4][4] = {{ 60, 30,-60, 30},{30,16,-30,14},
	                          {-60,-30, 60,-30},{30,14,-30,16}};

const static float M2[4][4] = 
{	{ 156, 22,  54, -13 },	{ 22,  4,  13, -3 },
	{ 54,  13, 156, -22 },	{ -13,-3, -22,  4 }
};

inline float phi1(float x);
inline float phi2(float x);
inline float phi3(float x);
inline float phi4(float x);
float psi1(float xx, int k, Spline& s);
float psi2(float xx, int k, Spline& s);
float psi3(float xx, int k, Spline& s);
float psi4(float xx, int k, Spline& s);
float dpsi1(float xx, int k, Spline& s);
float dpsi2(float xx, int k, Spline& s);
float dpsi3(float xx, int k, Spline& s);
float dpsi4(float xx, int k, Spline& s);
typedef float (*func)(float, int, Spline&);

class Spline
{
    friend float psi1(float,int,Spline&);
    friend float psi2(float,int,Spline&);
    friend float psi3(float,int,Spline&);
    friend float psi4(float,int,Spline&);
    friend float dpsi1(float,int,Spline&);
    friend float dpsi2(float,int,Spline&);
    friend float dpsi3(float,int,Spline&);
    friend float dpsi4(float,int,Spline&);

    float **A;
    float * b;
    int n, k;
    vector<fl2> nvtr;
    float * q;
    vector<float> x;
    vector<float> f;
    vector<float> omega;
    float beta;
    float alpha;

    //вклад от слагаемого с beta (формула 4.110)
    float Mk(int i, int j, int kk);
    //вклад от слагаемого с alpha(формула 4.110)
    float Mk2(int i, int j, int kk);

    // расчет локальной матрицы.
    float Ak(int i, int j, int kk);
    float Fk(int i, int kk);

    void compile_a();//сборка глобальной матрицы
    void compile_f();//сборка глобальной матрицы
    void solve_slau();
    void out(vector<fl2> &spline, vector<fl2> &dif_spline);
public:
    void get(set<fl2> points, vector<fl2> &spline, vector<fl2> &dif_spline,
             float beta, float alpha, vector<float> omega, set<float> nvtr);
};
#endif /*__SPLINE_H*/