#include "pch.h"
#include "fl3.h"
#include "pp.h"
#include "spline.h"

int width  = 600,
    height = 600,
    zoom   = 0,
    eyex   = 0,
    eyey   = 0;
bool b_MBPressed = false;

fl2           curPoint;//текущая нововведенная точка
set<fl2>      points;//массив точек, на основе которых строится сплайн
vector<fl2>   spline;//массив точек - сплайн
float         beta       = 1;//коэф. при вторых производных
float         alpha      = 1;//коэф. при первых производных
vector<float> omega;//веса точек
set<float>    nvtr;//массив, используемый для разделения на конечные элементы
vector<fl2>   dif_spline;//график производной сплайна
Spline        SPLINE;//класс-решатель. формирует сплайн.
//близость точек с заданной погрешностью.
bool Near (const fl2 a, const fl2 b, float EPS)
{
    return abs(a.m[0]-b.m[0]) < EPS && abs(a.m[1]-b.m[1]) < EPS;
}
//режим отрисовки графиков
enum DrawMode
{
    DRM_ALL = 1,
    DRM_POINTS,
    DRM_SPLINE,
    DRM_DIF,
    DRM_DIF_SPLINE,
    DRM_DIF_POINTS,
    DRM_POINTS_SPLINE
} drawMode = DRM_ALL;
bool bDrawPointLines = false;//соединять ли задаваемые точки линиями
//сетка
void glDrawGridXY( )
{
    glColor3fv(fl3(0.7,0.8,0.7).m );
    glLineWidth(1);
    glBegin(GL_LINES);
	for(float i = -10000; i < 10000; i+=10) 
    {
        glVertex3fv( (fl3(i,-10000,zoom)+fl3(eyex,eyey)).m );
        glVertex3fv( (fl3(i, 10000,zoom)+fl3(eyex,eyey)).m );
        glVertex3fv( (fl3(-10000,i,zoom)+fl3(eyex,eyey)).m );
        glVertex3fv( (fl3( 10000,i,zoom)+fl3(eyex,eyey)).m );
    }
	glEnd();
    glLineWidth(2);
    glBegin(GL_LINES);
	    glVertex3fv( (fl3(0,-10000,zoom)+fl3(eyex,eyey)).m );
        glVertex3fv( (fl3(0, 10000,zoom)+fl3(eyex,eyey)).m );
        glVertex3fv( (fl3(-10000,0,zoom)+fl3(eyex,eyey)).m );
        glVertex3fv( (fl3( 10000,0,zoom)+fl3(eyex,eyey)).m );
	glEnd();
    
}
void display( )
{
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT);

    glDrawGridXY();

    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(0.3,0.3,0.3);
    auto a = nvtr.begin();
    int i = 0;
    if(points.size()>1)
    {
        fl3 p1 = fl3(fl2(*(points.begin())*10.0)+fl2(eyex,eyey),zoom);
        glVertex3f( p1.m[0], -10000, p1.m[2] );
        glVertex3f( p1.m[0], 10000, p1.m[2] );
        fl3 p2 = fl3(fl2(*(points.rbegin())*10.0)+fl2(eyex,eyey),zoom);
        glVertex3f( p2.m[0], -10000, p2.m[2] );
        glVertex3f( p2.m[0], 10000, p2.m[2] );
    }
    for(auto b : nvtr)
    {
        fl3 p = fl3(fl2(b*10.0,0.0)+fl2(eyex,eyey),zoom);
        glVertex3f( p.m[0], -10000, p.m[2] );
        glVertex3f( p.m[0], 10000, p.m[2] );
    }
    glEnd();

    glLineWidth(1);
    glColor3f(0.7,0.8,0.7);
    glBegin(GL_LINE_STRIP);
    if( drawMode==DRM_POINTS        || 
        drawMode==DRM_DIF_POINTS    ||
        drawMode==DRM_POINTS_SPLINE || drawMode==DRM_ALL)
	for(auto it : points) 
    {
        glVertex3fv( (fl3(fl2(it*10.0)+fl2(eyex,eyey),zoom)).m );
    }
    glEnd();

    glColor3f(0.3,0.5,0.3);
    glPointSize(3);
    glBegin(GL_QUADS);
    if( drawMode==DRM_POINTS        || 
        drawMode==DRM_DIF_POINTS    ||
        drawMode==DRM_POINTS_SPLINE || drawMode==DRM_ALL)
	for(auto it : points) 
    {
        fl3 point = fl3(fl2(it*10.0)+fl2(eyex,eyey),zoom);
        float z = (1-zoom/100)*0.5;
        glVertex3fv( (point+fl3(-z,-z)).m ); 
        glVertex3fv( (point+fl3(z,-z)).m );
        glVertex3fv( (point+fl3(z,z)).m ); 
        glVertex3fv( (point+fl3(-z,z)).m );
    }

    glColor3f(0.9,0.5,0.3);
    if(b_MBPressed)
    {
        fl3 point = fl3(fl2(curPoint*10.0)+fl2(eyex,eyey),zoom);
        float z = (1-zoom/100)*0.5;
        glVertex3fv( (point+fl3(-z,-z)).m ); 
        glVertex3fv( (point+fl3(z,-z)).m );
        glVertex3fv( (point+fl3(z,z)).m ); 
        glVertex3fv( (point+fl3(-z,z)).m );
    }
	glEnd();

    glLineWidth(3);
    glColor3f(0.5,0.6,0.5);
    glBegin(GL_LINE_STRIP);
    if( drawMode==DRM_SPLINE        || 
        drawMode==DRM_DIF_SPLINE    ||
        drawMode==DRM_POINTS_SPLINE || drawMode==DRM_ALL)
	for(auto it : spline)
        glVertex3fv( fl3(fl2(it*10.0) + fl2(eyex,eyey),zoom).m );
    glEnd();

    glLineWidth(3);
    glColor3f(0.0,0.3,0.0);
    glBegin(GL_LINE_STRIP);
    if( drawMode==DRM_DIF        || 
        drawMode==DRM_DIF_SPLINE ||
        drawMode==DRM_DIF_POINTS || drawMode==DRM_ALL)
	for(auto it : dif_spline)
        glVertex3fv( fl3(fl2(it*10.0) + fl2(eyex,eyey),zoom).m );
    glEnd();

	glFinish();
}
void print( )
{
    system("cls");
    cout << "Spline.Diff.Points."               << endl
         << "==============================="   << endl
         << " Keys:"                            << endl
         << "   u/j  - beta +/-"                << endl
         << "   y/h  - alpha +/-"               << endl
         << "   1-7  - drawMode:"               << endl
         << "           1 - All"                << endl
         << "           2 - Points"             << endl
         << "           3 - Spline"             << endl
         << "           4 - SplineDiff"         << endl
         << "           5 - Spline and Diff"    << endl
         << "           6 - Points and Diff"    << endl
         << "           7 - Points and Spline"  << endl
         << "   wasd - Navigation"              << endl
         << "   +/-  - Graphic zoom"            << endl;
    
}
void reshape( GLint w, GLint h )
{
	width = w; height = h;

    pp::height = h;
    pp::update(); 

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 0.001f, 40000.0f );
    gluLookAt(0,0,100, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    print();
}
void keyboard( unsigned char key, int x, int y )
{
#define ESCAPE '\033'
	switch( key ) {	
	case ESCAPE: exit(0);
    case '-': 
    {
        zoom -= 100; 
        pp::zoom = zoom; pp::update();
        
    }
    break;
    case '+': 
    { 
        if(zoom < 0) 
        { 
            zoom += 100; 
            pp::zoom = zoom; pp::update(); 
        } 
    }
    break;
    case 'w':
    { 
        eyey -= 10; 
        pp::eyey = eyey/10; pp::update(); 
    }
    break;
    case 's': 
    {
        eyey += 10; 
        pp::eyey = eyey/10; pp::update(); 
    }
    break;
    case 'd':
    { 
        eyex -= 10; 
        pp::eyex = eyex/10; pp::update(); 
    }
    break;
    case 'a':
    { 
        eyex += 10; 
        pp::eyex = eyex/10; pp::update(); 
    }
    break;
    case 'c': 
    {
        points.clear(); 
        spline.clear();
        dif_spline.clear();
        nvtr.clear();
    }
    break;
    case 'u':
    {
        beta *= 2;
        SPLINE.get(points,spline,dif_spline,beta,alpha,omega,nvtr);
    }
    break;
    case 'j':
    {
        if(beta > 0.00005)
        {
            beta *= 0.5;
            SPLINE.get(points,spline,dif_spline,beta,alpha,omega,nvtr);
        }
    }
    break;
    case 'y':
    {
        alpha *= 2;
        SPLINE.get(points,spline,dif_spline,beta,alpha,omega,nvtr);
    }
    break;
    case 'h':
    {
        if(alpha > 0.00005)
        {
            alpha *= 0.5;
            SPLINE.get(points,spline,dif_spline,beta,alpha,omega,nvtr);
        }
    }
    break;
    default: break;
    }
    if(key < '8' && key > '0')
        drawMode = static_cast<DrawMode>(key - '0');
    glutPostRedisplay();
}
void mouse( int button, int state, int x, int y )
{
	if(button==GLUT_LEFT_BUTTON)
    { 
        fl2 xy = pp::rp(x-(width>>1),-y+(height>>1));
        switch(state)
        {
        case GLUT_DOWN: 
        {
            float z = (1-zoom/100)*0.05;
            for(auto x : points)
            {
                if(Near(x,xy,z))
                {
                    curPoint = x;
                    b_MBPressed = true;
                    break;
                }
            }
        }
           break;
        case GLUT_UP: b_MBPressed = false; break;
        default: break;
        }
        if(state==GLUT_DOWN)
	    {
            points.insert(xy);
            omega.push_back(1);
        }
        if(points.size() > 1)
        SPLINE.get(points,spline,dif_spline,beta,alpha,omega,nvtr);
        glutPostRedisplay();
    }
    if(button==GLUT_RIGHT_BUTTON)
    {
        float xx = (pp::rp(x-(width>>1),-y+(height>>1))).m[0];
        if(xx > points.begin()->m[0] && xx < points.rbegin()->m[0])
        {
            nvtr.insert(xx);
            auto xit = nvtr.find(xx);
            float x1,x2,x3;
            x2 = *xit;
            if(*xit != *nvtr.begin())
            { 
                xit--;
                x1 = *xit;
                xit++;
            }
            else
                x1 = points.begin()->m[0];
            
            if(*xit != *nvtr.rbegin())
            {
                xit++;
                x3 = *xit;
            }
            else
                x3 = points.rbegin()->m[0];
            bool b1 = false, b2 = false;
            for(auto v : points)
            {
                int X = v.m[0];
                if(!b1 && (x1 < X && X < x2)) b1 = true;
                if(!b2 && (x2 < X && X < x3)) b2 = true;
            }
            if(!(b1&&b2)) nvtr.erase(xx);
            if(points.size() > 1)
                SPLINE.get(points,spline,dif_spline,beta,alpha,omega,nvtr);
            glutPostRedisplay();
        }
    }
}
void motion( int x, int y )
{
    
    if(b_MBPressed)
    {
        float z = (1-zoom/100)*0.4;
        fl2 xy = pp::rp(x-(width>>1),-y+(height>>1));
        points.erase(curPoint);
        points.insert(xy);
        curPoint = xy;
        if(points.size() > 1)
        SPLINE.get(points,spline,dif_spline,beta,alpha,omega,nvtr);
    }
    glutPostRedisplay();
}
void main( int argc, char **argv)
{
    print();
	glutInit			( &argc, argv );
	glutInitDisplayMode ( GLUT_RGB );
	glutInitWindowSize	( width, height );
	glutCreateWindow	( "rgr" );
	glutDisplayFunc		( display );
	glutReshapeFunc		( reshape );
	glutKeyboardFunc	( keyboard );
	glutMouseFunc		( mouse );
    glutMotionFunc      ( motion );
    glPolygonMode       (GL_FRONT_AND_BACK, GL_LINE);
	glutMainLoop		( );
}