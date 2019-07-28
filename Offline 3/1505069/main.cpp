
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>


#include<vector>

#include "bitmap_image.hpp"

#define pi (acos(-1.0))

#define eps 0.0000001



#define FOV 90
#define window_height 500
#define window_width 500




int N;
int Light;
///vector<Object*> objects;
///vector<point> lights;
///int rlevel;

int image_width;
int image_height;

using namespace std;

struct point
{
    double x,y,z;
    point(double a,double b,double c)
    {
        x=a;
        y=b;
        z=c;
    }
    point()
    {
    }
    double len()
    {
        return sqrt(x*x+y*y+z*z);

    }
};

/// For camera part

point Normalized(point &a)
{
    double val=(a.x*a.x+a.y*a.y+a.z*a.z);
    val=sqrt(val);
    a.x/=val;
    a.y/=val;
    a.z/=val;
    return a;


}
void Rotate(point &a,point &b,double angle)
{
    a.x=a.x*cos(angle)+b.x*sin(angle);
    a.y=a.y*cos(angle)+b.y*sin(angle);
    a.z=a.z*cos(angle)+b.z*sin(angle);
    Normalized(a);

}
void Multiplication(point &a,point &b,point &c)
{

    a.x=(b.y*c.z-b.z*c.y);
    a.y=(b.z*c.x-b.x*c.z);
    a.z=(b.x*c.y-b.y*c.x);
    Normalized(a);

}

///end for camera part

int rlevel;
vector<point> lights;

struct Ray
{

    point Ro;
    point Rd;

    Ray(point s,point d)
    {
        Ro = s;
        Rd = d;
    }

};


struct Object
{
    int triangle,floor,sphere;
    point refpoint;
    double height, width, length;

    double color[3];
    double coefficients[4];
    int Exp;

    Object() { }
    virtual  point getNormal(point pointed)
    {

    }

    virtual void draw() {}
    virtual double getIntersectingT(Ray* r)
    {
    }
    virtual double intersect(Ray *r,point* Color, int level)
    {
    }

    point getReflection(Ray *r, point normal)
    {
        double adotn=dot(r->Rd, normal);
        point reflect(r->Rd.x - 2.0* normal.x * adotn,r->Rd.y - 2.0 *normal.y * adotn,r->Rd.z - 2.0 *normal.z * adotn);
        ///r = a  -  2 ( a . n) n


        return Normalized(reflect);
    }


    void setColor(double r, double g, double b)
    {
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
    };
    void setExp(int Exp)
    {
        this->Exp = Exp;
    };
    void setCoEfficients(double a, double b, double c, double d)
    {
        this->coefficients[0] = a;
        this->coefficients[1] = b;
        this->coefficients[2] = c;
        this->coefficients[3] = d;
    }
    double dot( point a, point b)
    {
        double val=a.x * b.x + a.y * b.y + a.z * b.z;
        return val;
    }

    point cross(point a,point b)
    {
        point c;
        c.x = (a.y * b.z - a.z * b.y);
        c.y = (a.z * b.x - a.x * b.z);
        c.z = (a.x * b.y - a.y * b.x);

        return c;
    }

};
vector<Object*> objects;

void Colouring(Object *obj,Ray *r, point* Color,int level,double t)
{
    point insect(r->Ro.x + r->Rd.x * t,r->Ro.y + r->Rd.y * t,r->Ro.z + r->Rd.z * t);
    point normal;
    if(obj->triangle!=1)
        normal= obj->getNormal(insect);
    else
        normal=obj->getNormal(r->Rd);
    point reflect = obj->getReflection(r, normal);


    Color->x = obj->color[0] * obj->coefficients[0];
    Color->y = obj->color[1] * obj->coefficients[0];
    Color->z = obj->color[2] * obj->coefficients[0];

    for(int i = 0; i < lights.size(); i++)
    {
        point lRd(lights[i].x - insect.x,lights[i].y - insect.y,lights[i].z - insect.z);


        double len = lRd.len();
        lRd = Normalized(lRd);

        point lRo(insect.x + lRd.x,insect.y + lRd.y,insect.z + lRd.z);


        Ray L(lRo, lRd);

        bool obscured = false;

        for(int j = 0; j < objects.size(); j++)
        {
            double isobscured = objects[j]->getIntersectingT(&L);
            if(isobscured > 0 && abs(isobscured) <= len)
            {
                obscured = true;
                break;
            }
        }

        if(!obscured)
        {
            ///slide 22 from color
            double lambert = obj->dot(L.Rd, normal);
            double phong = obj->dot(reflect, r->Rd)/2.0;

            lambert = max(lambert,0.0) ;
            phong = max(phong,0.0);

            Color->x +=  obj->color[0] * (lambert * obj->coefficients[1] + pow(phong, obj->Exp) * obj->coefficients[2]);
            Color->y +=  obj->color[1] * (lambert * obj->coefficients[1] + pow(phong, obj->Exp) * obj->coefficients[2]);
            Color->z +=  obj->color[2] * (lambert * obj->coefficients[1] + pow(phong, obj->Exp) * obj->coefficients[2]);
        }

        if(level < rlevel)
        {
            point rs(insect.x + reflect.x,insect.y + reflect.y,insect.z + reflect.z);

            Ray reflectionRay(rs, reflect);
            int nearest = -1;

            point reflectColor;
            double t_min = 1e4;
            for(int k = 0; k < objects.size(); k++)
            {
                double t = objects[k]->getIntersectingT(&reflectionRay);

                if(t <= 0)
                    continue;
                else if(t < t_min)
                {
                    t_min =min(t,t_min);
                    nearest = k;
                }
            }

            if(nearest != -1)
            {
                double t = objects[nearest]->intersect(&reflectionRay, &reflectColor, level + 1);
                if(t != -1)
                {
                    Color->x += reflectColor.x*obj->coefficients[3];
                    Color->y += reflectColor.y*obj->coefficients[3];
                    Color->z += reflectColor.z*obj->coefficients[3];
                }

            }
        }


        Color->x=max(0.0,min(Color->x,1.0));
        Color->y=max(0.0,min(Color->y,1.0));
        Color->z=max(0.0,min(Color->z,1.0));
    }

}

class Sphere: public Object
{
public:
    Sphere(point Center, double Radius)
    {
        refpoint = Center;
        height =length = Radius;

        sphere=1;
        triangle=floor=0;
    }

    void draw()
    {


        point points[100][100];
        int i,j;
        double h,r;
        int stacks = 20;
        int slices = 24;
        //generate points
        for(i=0; i<stacks; i++)
        {
            h=length*sin(((double)i/(double)stacks)*(pi/2.0));
            r=length*cos(((double)i/(double)stacks)*(pi/2.0));
            for(j=0; j<slices; j++)
            {
                points[i][j].x=r*cos(((double)j/(double)slices)*2.0*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2.0*pi);
                points[i][j].z=h;
            }
        }
        //draw quads using generated points
        glColor3f(color[0], color[1], color[2]);
        for(i=0; i<stacks; i++)
        {
            for(j=0; j<slices; j++)
            {
                glBegin(GL_QUADS);
                {
                    //upper hemisphere
                    glVertex3f(points[i][j].x + refpoint.x, points[i][j].y + refpoint.y, points[i][j].z + refpoint.z);
                    glVertex3f(points[i][j+1].x + refpoint.x, points[i][j+1].y + refpoint.y, points[i][j+1].z + refpoint.z);
                    glVertex3f(points[i+1][j+1].x + refpoint.x, points[i+1][j+1].y + refpoint.y, points[i+1][j+1].z + refpoint.z);
                    glVertex3f(points[i+1][j].x + refpoint.x, points[i+1][j].y + refpoint.y, points[i+1][j].z + refpoint.z);
                    //lower hemisphere
                    glVertex3f(points[i][j].x + refpoint.x,points[i][j].y + refpoint.y,-points[i][j].z + refpoint.z);
                    glVertex3f(points[i][j+1].x + refpoint.x,points[i][j+1].y + refpoint.y,-points[i][j+1].z + refpoint.z);
                    glVertex3f(points[i+1][j+1].x + refpoint.x,points[i+1][j+1].y + refpoint.y,-points[i+1][j+1].z + refpoint.z);
                    glVertex3f(points[i+1][j].x + refpoint.x,points[i+1][j].y + refpoint.y,-points[i+1][j].z + refpoint.z);
                }
                glEnd();
            }
        }
    }

    double getIntersectingT(Ray* r)
    {
        point p(r->Ro.x - refpoint.x,r->Ro.y - refpoint.y,r->Ro.z - refpoint.z);


        double a = dot(r->Rd, r->Rd);
        double b = 2.0 * dot(r->Rd, p);
        double c = dot(p, p) -length*length;
        double D = b*b - 4 * a * c;

        if(D < 0)
            return -1;

        double t1 = (-b + sqrt(D)) / (2 * a);
        double t2 = (-b - sqrt(D)) / (2 * a);

        return min(t1,t2);
    }

    point getNormal(point pointed)
    {
        ///slide 35
        point normal(pointed.x - refpoint.x,pointed.y - refpoint.y,pointed.z - refpoint.z);

        return Normalized(normal);
    }



    double intersect(Ray *r,point* Color, int level)
    {

        double t = getIntersectingT(r);

        if(t <= 0)
            return -1;

        if(level == 0)
            return t;

        /// P(t)=ro+t*rd
        Colouring(this,r,Color,level,t);

        return t;
    }
};

class Floor: public Object
{
public:

    Floor(point p,double base)
    {
        refpoint=p;
        length=base;
        floor=1;
        sphere=triangle=0;

    }

    Floor(double floor_width, double tile_width)
    {
        floor=1;
        sphere=triangle=0;
        refpoint.x = -floor_width/2.0;
        refpoint.y = -floor_width/2.0;
        refpoint.z = 0;
        length = tile_width;
        height = 0;


    }


    point getNormal(point pointed)
    {
        pointed.x = 0;
        pointed.y = 0;
        pointed.z = 1;
        return pointed;
    }

    double getIntersectingT(Ray *r)
    {
        point normal = getNormal(refpoint);
        double up=dot(normal, r->Ro);
        double down=dot(normal, r->Rd);
        return ((-1.0) *  up/down );
    }

    double intersect(Ray *r, point* Color, int level)
    {
        double t = getIntersectingT(r);

        point insect(r->Ro.x + r->Rd.x * t,r->Ro.y + r->Rd.y * t,r->Ro.z + r->Rd.z * t);


        if(insect.x < refpoint.x || insect.x > -refpoint.x || insect.y < refpoint.y || insect.y > -refpoint.y)
        {
            return -1;
        }
        if(level == 0)
            return t;

        int x = (insect.x - refpoint.x) / length;
        int y = (insect.y - refpoint.y) / length;

        if((x + y) % 2 == 0)
        {
            color[0] = color[1] = color[2] = 0;
        }
        else
        {
            color[0] = color[1] = color[2] = 1;
        }


        Colouring(this,r,Color,level,t);
        return t;
    }


    void draw()
    {
        point now = refpoint;

        int black = 0;
        for(now.y=refpoint.y; now.y < -refpoint.y; now.y += length)
        {
            for(now.x=refpoint.x; now.x < -refpoint.x; now.x += length)
            {
                if(black == 1)
                {
                    glColor3f(0, 0, 0);
                }
                else
                {
                    glColor3f(1, 1, 1);
                }


                glBegin(GL_QUADS);
                glVertex3f(now.x, now.y, now.z);
                glVertex3f(now.x, now.y + length, now.z);
                glVertex3f(now.x + length, now.y + length, now.z);
                glVertex3f(now.x + length, now.y, now.z);
                glEnd();
                black =  black^1;

            }

            black = black^1;
        }
    }
};

class Triangle: public Object
{
public:
    point a, b, c;
    Triangle(point a, point b, point c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
        triangle=1;
        sphere=floor=0;
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        }
        glEnd();
    }

    point getNormal(point pointed)
    {
        point normal;
        point u(b.x - a.x,b.y - a.y,b.z - a.z);

        point v(c.x - a.x, c.y - a.y,c.z - a.z);
        normal = cross(u, v);
        normal = Normalized(normal);

        if(dot(pointed, normal) > 0)
        {
            normal.x = -normal.x;
            normal.y = -normal.y;
            normal.z = -normal.z;
        }

        return normal;
    }

    double getIntersectingT(Ray *r)
    {

        point edge1, edge2,edge3;
        edge1.x=a.x-b.x;
        edge1.y=a.y-b.y;
        edge1.z=a.z-b.z;
        // Normalized(edge1);

        edge2.x=a.x-c.x;
        edge2.y=a.y-c.y;
        edge2.z=a.z-c.z;
        // Normalized(edge2);
        edge3.x=a.x-r->Ro.x;
        edge3.y=a.y-r->Ro.y;
        edge3.z=a.z-r->Ro.z;

        double A=0.0;

        A+=(edge1.x*(edge2.y*r->Rd.z-r->Rd.y*edge2.z));
        A-=(edge1.y*(edge2.x*r->Rd.z-r->Rd.x*edge2.z));
        A+=(edge1.z*(edge2.x*r->Rd.y-r->Rd.x*edge2.y));

        if(A<eps)
        {
            return -1;
        }

        double Gamma=0.0;

        Gamma+=(edge1.x*(edge3.y*r->Rd.z-r->Rd.y*edge3.z));
        Gamma-=(edge1.y*(edge3.x*r->Rd.z-r->Rd.x*edge3.z));
        Gamma+=(edge1.z*(edge3.x*r->Rd.y-r->Rd.x*edge3.y));
        Gamma/=A;
        if(Gamma<eps || Gamma>1.0)
        {
            return -1;
        }

        double Beta=0.0;

        Beta+=(edge3.x*(edge2.y*r->Rd.z-r->Rd.y*edge2.z));
        Beta-=(edge3.y*(edge2.x*r->Rd.z-r->Rd.x*edge2.z));
        Beta+=(edge3.z*(edge2.x*r->Rd.y-r->Rd.x*edge2.y));
        Beta/=A;
        if(Beta<eps || Beta>1-Gamma)
        {
            return -1;
        }

        double t=0.0;

        t+=(edge1.x*(edge2.y*edge3.z-edge3.y*edge2.z));
        t-=(edge1.y*(edge2.x*edge3.z-edge3.x*edge2.z));
        t+=(edge1.z*(edge2.x*edge3.y-edge3.x*edge2.y));
        t/=A;

        return t;


    }

    double intersect(Ray *r, point* Color, int level)
    {
        double t = getIntersectingT(r);
        if(t<=0)
        {
            return -1;
        }
        if(level == 0)
        {
            return t;
        }

        Colouring(this,r,Color,level,t);
        return t;
    }
};

class Pyramid: public Object
{
public:

    point p1;
    point p2;
    point p3;
    point p4;
    point p5;
    double base,height;
    vector<Triangle*>tri;
    Pyramid(point p,double b,double h)
    {
        this->p1=p;

        double base=b;
        double height=h;
        p2.x=p1.x+base;
        p2.y=p1.y;
        p2.z=p1.z;
        p3.x=p1.x+base;
        p3.y=p1.y+base;
        p3.z=p1.z;

        p4.x=p1.x;
        p4.y=p1.y+base;
        p4.z=p1.z;

        p5.x=p1.x+base/2.0;
        p5.y=p1.y+base/2.0;
        p5.z=p1.z+height;
        /// cout<<p5.x<<" "<<p5.y<<" "<<p5.z<<endl;

        Triangle *t ;
        t =new Triangle(p1,p5,p2);
        tri.push_back(t);
        t =new Triangle(p2,p5,p3);
        tri.push_back(t);
        t =new Triangle(p3,p5,p4);
        tri.push_back(t);

        t =new Triangle(p4,p5,p1);
        tri.push_back(t);

        t =new Triangle(p1,p2,p4);
        tri.push_back(t);

        t =new Triangle(p2,p3,p4);
        tri.push_back(t);








    }
    double getIntersectingT(Ray* r)
    {

        double t_min = 1e4;
        for(int i=0; i<tri.size(); i++)
        {
            double t=tri[i]->getIntersectingT(r);
            /// cout<<t<<endl;
            if(t <= 0)
                continue;
            t_min=min(t_min,t);
        }


        return t_min;

    }

    void draw()
    {
        for(int i=0; i<tri.size(); i++)
        {
            tri[i]->setColor(this->color[0], this->color[1], this->color[2]);
            tri[i]->setCoEfficients(this->coefficients[0],this->coefficients[1],this->coefficients[2],this->coefficients[3]);
            tri[i]->setExp(this->Exp);
        }

        for(int i=0; i<tri.size(); i++)
        {
            tri[i]->draw();
        }

    }
    double intersect(Ray *r, point* Color, int level)
    {

        double t_min = 1e4;
        for(int i=0; i<tri.size(); i++)
        {
            double t=tri[i]->intersect(r,Color,level);
            ///cout<<i<<" "<<t<<endl;
            if(t <= 0)
                continue;
            t_min=min(t_min,t);
        }
        return t_min;

    }

};


point pos(0,-100,50),u(0,0,1),r(1,0,0),l(0,1,0);

void capture()
{
    cout<<"Start capturing"<<endl;
    ///https://drive.google.com/file/d/1vRfbGWh2rH43bQVk61SWv31X1VU9F4Tl/view?usp=sharing
    bitmap_image image(image_width, image_height);


    ///tan(FOV/2)=(distance/(window_hight/2.0))
    double plane_distance = (window_height/2.0)/tan((FOV/2.0) * (pi / 180.0));

    ///get the topleft point of the view plane :  eye->l->r->u
    point topleft;
    topleft.x = pos.x + l.x * plane_distance - r.x * (window_width/2.0) + u.x * (window_height/2.0);
    topleft.y = pos.y + l.y * plane_distance - r.y * (window_width/2.0) + u.y * (window_height/2.0);
    topleft.z = pos.z + l.z * plane_distance - r.z * (window_width/2.0) + u.z * (window_height/2.0);


    double ratiox = (1.0*window_width)/image_width;
    double ratioy = (1.0*window_height)/image_height;


    for(int i = 0; i < image_height; i++)
    {
        for(int j = 0; j < image_width; j++)
        {
            point corner;
            /// get current view point
            corner.x = topleft.x + r.x * j * ratiox - u.x * i * ratioy;
            corner.y = topleft.y + r.y * j * ratiox - u.y * i * ratioy;
            corner.z = topleft.z + r.z * j * ratiox - u.z * i * ratioy;

            point diff(corner.x - pos.x,corner.y - pos.y,corner.z - pos.z);
            diff = Normalized(diff);

            /// create ray from the eye to view point
            Ray ray(pos, diff);

            int nearest = -1;

            point Color;
            double t_min = 1e4;
            for(int k = 0; k < objects.size(); k++)
            {
                double t = objects[k]->intersect(&ray, &Color, 0);

                if(t <= 0)
                    continue;
                else if(t < t_min)
                {
                    t_min = min(t,t_min);
                    nearest = k;
                }
            }

            if(nearest != -1)
            {
                double t = objects[nearest]->intersect(&ray, &Color, 1);

                /// update pixel with nearest intersection
                image.set_pixel(j, i, Color.x * 255, Color.y * 255, Color.z * 255);
            }
        }
    }

    image.save_image("out.bmp");
    cout<<"Image captured"<<endl;
}


void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {
    case '0':
        capture();
        break;

    case '1':
        Rotate(r,l,pi/60);
        Multiplication(l,u,r);

        break;
    case '2':
        Rotate(r,l,-pi/60);

        Multiplication(l,u,r);
        break;
    case '3':
        Rotate(l,u,pi/60);


        Multiplication(u,r,l);


        break;
    case '4':
        Rotate(l,u,-pi/60);


        Multiplication(u,r,l);

        break;
    case '5':
        Rotate(u,r,pi/60);


        Multiplication(r,l,u);

        break;
    case '6':
        Rotate(u,r,-pi/60);


        Multiplication(r,l,u);
        break;

    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        pos.x-=l.x*2;
        pos.y-=l.y*2;
        pos.z-=l.z*2;
        break;
    case GLUT_KEY_UP:		// up arrow key
        pos.x+=l.x*2;
        pos.y+=l.y*2;
        pos.z+=l.z*2;
        break;

    case GLUT_KEY_RIGHT:
        pos.x+=r.x*2;
        pos.y+=r.y*2;
        pos.z+=r.z*2;
        break;
    case GLUT_KEY_LEFT:
        pos.x-=r.x*2;
        pos.y-=r.y*2;
        pos.z-=r.z*2;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x+=u.x*2;
        pos.y+=u.y*2;
        pos.z+=u.z*2;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x-=u.x*2;
        pos.y-=u.y*2;
        pos.z-=u.z*2;
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:

        break;
    case GLUT_KEY_END:

        break;

    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        break;

    case GLUT_RIGHT_BUTTON:
        //........
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}







void display()
{

    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();

    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    //gluLookAt(100,100,100,	0,0,0,	0,0,1);
    //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    for(int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }

    for(int i = 0; i < lights.size(); i++)
    {
        glBegin(GL_POINTS);
        glColor3f(255, 255, 255);
        glVertex3f(lights[i].x, lights[i].y, lights[i].z);
        glEnd();
    }

    glutSwapBuffers();
}


void animate()
{
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //codes for initialization


    //clear the screen
    glClearColor(0,0,0,0);/// background color

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(90,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

void loadData()
{
    ifstream fin("description.txt");
    fin>>rlevel;
    fin>>image_width;
    image_height = image_width;

    fin>>N;
    Object *temp;
    for(int i = 0; i < N; i++)
    {
        string str;
        fin>>str;
        if(str.compare("sphere") == 0)
        {
            struct point center;
            double radius;
            fin>>center.x>>center.y>>center.z;
            fin>>radius;
            temp = new Sphere(center, radius);
            double r, g, b;
            double a, d, s, re;
            fin>>r>>g>>b;
            temp->setColor(r, g, b);
            fin>>a>>d>>s>>re;
            temp->setCoEfficients(a, d, s, re);
            int Exp;
            fin>>Exp;
            temp->setExp(Exp);
            objects.push_back(temp);
        }
        else if(str.compare("pyramid") == 0)
        {
            point p;

            fin>>p.x>>p.y>>p.z;
            double base,height;
            fin>>base>>height;
            temp = new Pyramid(p,  base,height);
            double r, g, b;
            double a, d, s, re;
            fin>>r>>g>>b;
            temp->setColor(r, g, b);
            fin>>a>>d>>s>>re;
            temp->setCoEfficients(a, d, s, re);
            int Exp;
            fin>>Exp;
            temp->setExp(Exp);
            objects.push_back(temp);
        }
    }
    fin>>Light;
    //cout<<Light<<endl;
    for(int i = 0; i < Light; i++)
    {
        struct point light;
        fin>>light.x>>light.y>>light.z;
        //cout<<light.x<<" "<<light.y<<" "<<light.z<<endl;
        lights.push_back(light);
    }

    temp = new Floor(1000, 30);
    temp->setCoEfficients(0.5, 0.4, 0.3, 0.1);
    temp->setExp(5);
    objects.push_back(temp);
}

int main(int argc, char **argv)
{
    loadData();
    glutInit(&argc,argv);
    glutInitWindowSize(window_height, window_width);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("Offline 3");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();		//The main loop of OpenGL

    return 0;
}
