#include "spline.h"

//базисные ф-ии в общем виде
inline float phi1(float x){ return 1-3*x*x+2*x*x*x; }
inline float phi2(float x){ return x-2*x*x+x*x*x;   }
inline float phi3(float x){ return 3*x*x-2*x*x*x;   }
inline float phi4(float x){ return -x*x+x*x*x;      }

//базисные функции, которые уже на конкретном k-м  Ё.
float psi1(float xx, int k, Spline& s)
{
	int k1 = s.nvtr[k][0], k2 = s.nvtr[k][1];
	float h = s.x[k2] - s.x[k1];
	float ksi = (xx - s.x[k1])/h;
	return phi1(ksi);
}
float psi2(float xx, int k, Spline& s)
{
	int k1 = s.nvtr[k][0], k2 = s.nvtr[k][1];
	float h = s.x[k2] - s.x[k1];
	float ksi = (xx - s.x[k1])/h;
	return h*phi2(ksi);
}
float psi3(float xx, int k, Spline& s)
{
	int k1 = s.nvtr[k][0], k2 = s.nvtr[k][1];
	float h = s.x[k2] - s.x[k1];
	float ksi = (xx - s.x[k1])/h;
	return phi3(ksi);
}
float psi4(float xx, int k, Spline& s)
{
	int k1 = s.nvtr[k][0], k2 = s.nvtr[k][1];
	float h = s.x[k2] - s.x[k1];
	float ksi = (xx - s.x[k1])/h;
	return h*phi4(ksi);
}

//производные базисных функций
float dpsi1(float xx, int k, Spline& s)
{
	int k1 = s.nvtr[k][0], k2 = s.nvtr[k][1];
	float h = s.x[k2] - s.x[k1];
	float dx = (xx - s.x[k1]);
	return -6.0*dx/(h*h) + 6*dx*dx/(h*h*h);
}
float dpsi2(float xx, int k, Spline& s)
{
	int k1 = s.nvtr[k][0], k2 = s.nvtr[k][1];
	float h = s.x[k2] - s.x[k1];
	float dx = (xx - s.x[k1]);
	return 1-4*dx/h+3*dx*dx/(h*h);
}
float dpsi3(float xx, int k, Spline& s)
{
	int k1 = s.nvtr[k][0], k2 = s.nvtr[k][1];
	float h = s.x[k2] - s.x[k1];
	float dx = (xx - s.x[k1]);
	return 6*dx/(h*h)-6*dx*dx/(h*h*h);
}
float dpsi4(float xx, int k, Spline& s)
{
	int k1 = s.nvtr[k][0], k2 = s.nvtr[k][1];
	float h = s.x[k2] - s.x[k1];
	float dx = (xx - s.x[k1]);
	return -2*dx/h+3*dx*dx/(h*h);
}

//вклад от слагаемого с beta(формула 4.110)
float Spline::Mk(int i, int j, int kk)
{
	int k1 = nvtr[kk][0], k2 = nvtr[kk][1];
	float h = x[k2] - x[k1];
	float q = ((i==0||i==2)&&(j==0||j==2))?	1/(h*h*h):
				((i==1||i==3)&&(j==1||j==3))?	1/h:
												1/(h*h);
	return M[i][j]*q;
}
//вклад от слагаемого с alpha(формула 4.110)
float Spline::Mk2(int i, int j, int kk)
{
	int k1 = nvtr[kk][0], k2 = nvtr[kk][1];
	float h = x[k2] - x[k1];
	float q = ((i==0||i==2)&&(j==0||j==2))?	1/(h*h*h):
				((i==1||i==3)&&(j==1||j==3))?	1/h:
												1/(h*h);
	return M2[i][j]*q;
}

// расчет локальной матрицы.
float Spline::Ak(int i, int j, int kk)
{
	float Aij = 0;
    int ind = nvtr[kk][0];

    func psii, psij;
    switch(i)
    {
    case 0: psii = psi1; break;
    case 1: psii = psi2; break;
    case 2: psii = psi3; break;
    case 3: psii = psi4; break;
    default: break;
    }
    switch(j)
    {
    case 0: psij = psi1; break;
    case 1: psij = psi2; break;
    case 2: psij = psi3; break;
    case 3: psij = psi4; break;
    default: break;
    }
	//дл€ всех элементов, кроме последнего, последний узел не учитываем.
	for(int o = nvtr[kk][0],
			oe = ((k-1)!=kk) ? (nvtr[kk][1]-1) : nvtr[kk][1];
			o <= oe; o++, ind++) 
		Aij += omega[o]*psii(x[ind], kk, *this)*psij(x[ind], kk, *this);
	return Aij + beta*Mk(i,j,kk) + alpha*Mk2(i,j,kk);
}

float Spline::Fk(int i, int kk)
{
	float Fi = 0;
	int ind = nvtr[kk][0];
    func psii;
    switch(i)
    {
    case 0: psii = psi1; break;
    case 1: psii = psi2; break;
    case 2: psii = psi3; break;
    case 3: psii = psi4; break;
    default: break;
    }
	//дл€ всех элементов, кроме последнего, последний узел не учитываем.
	for(int o = nvtr[kk][0],
			oe = ((k-1)!=kk) ? (nvtr[kk][1]-1) : nvtr[kk][1];
			o <= oe; o++, ind++)
		Fi += omega[o]*psii(x[ind], kk, *this)*f[o];
	return Fi;
}

void  Spline::compile_a()//сборка глобальной матрицы
{
	for(int kk = 0; kk < k; kk++)// проход по  Ё
	{
		int ij0 = kk*2;
		for(int ii = 0, i = ij0; ii < 4; ii++, i++)
		for(int jj = 0, j = ij0; jj < 4; jj++, j++)
			A[i][j] += Ak(ii,jj,kk);
	}
}
void  Spline::compile_f()//сборка глобальной матрицы
{
	for(int i = 0; i < n; i++) b[i] = 0;
	for(int kk = 0; kk < k; kk++)// проход по  Ё
	{
		int i0 = kk*2;
		for(int ii = 0, i = i0; ii < 4; ii++, i++) b[i] += Fk(ii,kk);
	}
}

void  Spline::solve_slau()
{
	//√ј”——
	float Eps = 1e-20;
	// приведение к треугольному виду
	for (int j = 0; j < n - 1; j++)
    {
		int w = j;	// выбор главного элемента
		for (int i = j + 1; i < n; i++)	
			if (fabs(A[i][j]) > fabs(A[w][j])) 
				w = i;
		if(w != j)
		{
			for (int jj = j; jj < n; jj++)
			{	
				float x = A[j][jj]; 
				A[j][jj] = A[w][jj]; 
				A[w][jj] = x; 
			}
			float x = b[j]; 
			b[j] = b[w]; 
			b[w] = x;
		}
		float bj = b[j]; //вычитание строки из нижележащих
	    for (int i = j + 1; i < n; i++)
        {
			if(fabs(A[j][j]) < Eps) {printf("error.");return;}	//аварийный выход
            float t = A[i][j] / A[j][j];		
			b[i] -= t*bj;
            for (int jj = j; jj < n; jj++)	
				A[i][jj] -= t*A[j][jj];
        }
		//
	}
	// обратный ход
	for(int j = n-1; j>=0; j--)
	{
		if(fabs(A[j][j]) < Eps) {printf("error.");return;}	//аварийный выход
		q[j] = b[j] / A[j][j];	
		float qj = q[j];
		for(int i = j - 1; i >= 0; i--)	 
			b[i] -= A[i][j]*qj;
	}
}

void  Spline::out(vector<fl2> &spline, vector<fl2> &dif_spline)
{
	float ff;
    float df;
	int n = 0;
	for(int i = 0; i < k; i++)
	{
		
		int i1 = nvtr[i][0], i2 = nvtr[i][1];
		for(int j = i1; j < i2; j++)
		{
			float x1 = x[j], x2 = x[j+1];
			float h = (x2 - x1)/20;
			for(int j = 0; j < 20; j++, x1+=h)
			{
				int p = i*2;
				ff = 0;
                df = 0;
				ff +=   q[p  ]*psi1(x1,i,*this) +
                        q[p+1]*psi2(x1,i,*this) +
                        q[p+2]*psi3(x1,i,*this) +
                        q[p+3]*psi4(x1,i,*this) ;
                df +=   q[p  ]*dpsi1(x1,i,*this) +
                        q[p+1]*dpsi2(x1,i,*this) +
                        q[p+2]*dpsi3(x1,i,*this) +
                        q[p+3]*dpsi4(x1,i,*this) ;
                spline.push_back(fl2(x1,ff));
                dif_spline.push_back(fl2(x1,df));
			}
		}
	}
}

void  Spline::get(set<fl2>      points    , 
                  vector<fl2> & spline    ,
                  vector<fl2> & dif_spline,
                  float         beta      ,
                  float         alpha     ,
                  vector<float> omega     ,
                  set<float>    nvtr      )
{
    spline.clear();
    dif_spline.clear();
    
    k = nvtr.size()+1;
    this->nvtr.resize(k);
    this->nvtr[0].m[0] = 0;
    this->nvtr[k-1].m[1] = points.size()-1;

    int m = 0;
    for(auto x : nvtr)
    {
        int num = 0;
        int i = 0;
        float fmindistance = 10000;
        for(auto b : points)
        {
            float a = abs(x-b.m[0]);
            if(a < fmindistance)
            {
                fmindistance = a;
                num = i;
            }
            i++;
        }
        this->nvtr[m].m[1] = num;
        this->nvtr[m+1].m[0] = num;
        m++;
    }

    n = k*2+2;
    x.resize(points.size());
    f.resize(points.size());
    {
            auto it = begin(points);
        for(auto it1 = begin(x), it2 = begin(f); 
            it1 != end(x); it++, it1++, it2++)
        {
            *it1 = it->m[0];
            *it2 = it->m[1];
        }
    }
    A = new float*[n];
    b = new float [n];
    q = new float [n];
    for(int i = 0; i < n; i++)
    {
        A[i] = new float[n];
        for(int j = 0; j < n; j++)
        {
            A[i][j] = 0;
        }
    }
    this->beta = beta;
    this->alpha = alpha;
    this->omega = omega;

	compile_a();
	compile_f();
	solve_slau();
	out(spline, dif_spline);
}