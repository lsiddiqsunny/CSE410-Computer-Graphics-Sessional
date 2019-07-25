#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>


#include<vector>

#include "bitmap_image.hpp"

#define AMBIENT 0
#define DIFFUSE 1
#define SPECULAR 2
#define REFLECTION 3

#define REFINDEX 1.7
#define pi (2*acos(0.0))

#define EPSILON 0.0000001
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
};

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

extern int recursion_level;
extern vector<point> lights;

struct Ray
{

    point start;
    point dir;

    Ray(point s,point d)
    {
        start = s;
        dir = d;
    }

};


struct Object
{
    double dotProduct( point a, point b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    point crossProduct(point a,point b)
    {
        point cross;
        cross.x = a.y * b.z - a.z * b.y;
        cross.y = a.z * b.x - a.x * b.z;
        cross.z = a.x * b.y - a.y * b.x;

        return cross;
    }

    point reference_point;
    double height, width, length, source_factor = 1.0;
    int Shine;
    double color[3];
    double co_efficients[4];

    Object() { }
    virtual  point getNormal(point pointed)
    {
        return Normalized(pointed);
    }

    virtual void draw() {}
    virtual double getT(Ray* r)
    {
        return -1;
    }
    virtual double intersect(Ray *r,point* current_color, int level)
    {
        return -1;
    }

    point getReflection(Ray *r, point pointed)
    {
        point reflection;
        double product=dotProduct(r->dir, pointed);
        reflection.x = r->dir.x - pointed.x * 2.0 *product ;
        reflection.y = r->dir.y - pointed.y * 2.0 * product;
        reflection.z = r->dir.z - pointed.z * 2.0 * product;

        return Normalized(reflection);
    }


    void setColor(double r, double g, double b)
    {
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
    };
    void setShine(int shine)
    {
        this->Shine = shine;
    };
    void setCoEfficients(double a, double b, double c, double d)
    {
        this->co_efficients[0] = a;
        this->co_efficients[1] = b;
        this->co_efficients[2] = c;
        this->co_efficients[3] = d;
    }

};
extern vector<Object*> objects;

class Sphere: public Object
{
public:
    Sphere(point Center, double Radius)
    {
        reference_point = Center;
        length = Radius;
        height = Radius;
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);

        point points[100][100];
        int i,j;
        double h,r;
        int stacks = 20;
        int slices = 24;
        //generate points
        for(i=0; i<=stacks; i++)
        {
            h=length*sin(((double)i/(double)stacks)*(pi/2.0));
            r=length*cos(((double)i/(double)stacks)*(pi/2.0));
            for(j=0; j<=slices; j++)
            {
                points[i][j].x=r*cos(((double)j/(double)slices)*2.0*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2.0*pi);
                points[i][j].z=h;
            }
        }
        //draw quads using generated points
        for(i=0; i<stacks; i++)
        {
            for(j=0; j<slices; j++)
            {
                glBegin(GL_QUADS);
                {
                    //upper hemisphere
                    glVertex3f(points[i][j].x + reference_point.x, points[i][j].y + reference_point.y, points[i][j].z + reference_point.z);
                    glVertex3f(points[i][j+1].x + reference_point.x, points[i][j+1].y + reference_point.y, points[i][j+1].z + reference_point.z);
                    glVertex3f(points[i+1][j+1].x + reference_point.x, points[i+1][j+1].y + reference_point.y, points[i+1][j+1].z + reference_point.z);
                    glVertex3f(points[i+1][j].x + reference_point.x, points[i+1][j].y + reference_point.y, points[i+1][j].z + reference_point.z);
                    //lower hemisphere
                    glVertex3f(points[i][j].x + reference_point.x,points[i][j].y + reference_point.y,-points[i][j].z + reference_point.z);
                    glVertex3f(points[i][j+1].x + reference_point.x,points[i][j+1].y + reference_point.y,-points[i][j+1].z + reference_point.z);
                    glVertex3f(points[i+1][j+1].x + reference_point.x,points[i+1][j+1].y + reference_point.y,-points[i+1][j+1].z + reference_point.z);
                    glVertex3f(points[i+1][j].x + reference_point.x,points[i+1][j].y + reference_point.y,-points[i+1][j].z + reference_point.z);
                }
                glEnd();
            }
        }
    }

    double getT(Ray* r)
    {
        point s;
        s.x = r->start.x - reference_point.x;
        s.y = r->start.y - reference_point.y;
        s.z = r->start.z - reference_point.z;

        double A = dotProduct(r->dir, r->dir);
        double B = 2.0 * dotProduct(r->dir, s);
        double C = dotProduct(s, s) -length*length;
        double d = B*B - 4 * A * C;

        if(d < 0)
            return -1;

        double t1 = (-B + sqrt(d)) / (2 * A);
        double t2 = (-B - sqrt(d)) / (2 * A);

        return t1<t2?t1:t2;
    }

    point getNormal(point pointed)
    {
        point normal;
        normal.x = pointed.x - reference_point.x;
        normal.y = pointed.y - reference_point.y;
        normal.z = pointed.z - reference_point.z;

        return Normalized(normal);
    }



    double intersect(Ray *r,point* current_color, int level)
    {
        //printf("%f %f %f ", r->dir.x, r->dir.y, r->dir.z);

        double t = getT(r);

        //printf("here0\n");
        if(t <= 0)
            return -1;

        //printf("here1\n");
        if(level == 0)
            return t;

        //printf("here2\n");
        point intersection;
        intersection.x = r->start.x + r->dir.x * t;
        intersection.y = r->start.y + r->dir.y * t;
        intersection.z = r->start.z + r->dir.z * t;

        current_color->x = color[0] * co_efficients[AMBIENT];
        current_color->y = color[1] * co_efficients[AMBIENT];
        current_color->z = color[2] * co_efficients[AMBIENT];

        point normal = getNormal(intersection);
        point reflection = getReflection(r, normal);

        for(int i = 0; i < lights.size(); i++)
        {
            point ldir;
            ldir.x = lights[i].x - intersection.x;
            ldir.y = lights[i].y - intersection.y;
            ldir.z = lights[i].z - intersection.z;

            double len = sqrt(pow(ldir.x,2) + pow(ldir.y,2) + pow(ldir.z,2));
            ldir = Normalized(ldir);

            point lstart;
            lstart.x = intersection.x + ldir.x * 1.0;
            lstart.y = intersection.y + ldir.y * 1.0;
            lstart.z = intersection.z + ldir.z * 1.0;


            Ray L(lstart, ldir);

            bool obscured = false;

            for(int j = 0; j < objects.size(); j++)
            {
                double possibleobscure = objects[j]->getT(&L);
                //printf(",%f %d, ", possibleobscure, j);
                if(possibleobscure > 0 && abs(possibleobscure) < len)
                {
                    obscured = true;
                    break;
                }
            }
            //printf("\n");

            if(!obscured)
            {
                //printf("direct");
                //L.dir.x = -L.dir.x;
                //L.dir.y = -L.dir.y;
                //L.dir.z = -L.dir.z;
                double lambert = dotProduct(L.dir, normal);
                double phong = dotProduct(reflection, r->dir)/2;

                lambert = lambert > 0?lambert:0;
                phong = phong > 0?phong:0;

                current_color->x += source_factor * color[0] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
                current_color->y += source_factor * color[1] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
                current_color->z += source_factor * color[2] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
            }

            if(level < recursion_level)
            {
                point rs;
                rs.x = intersection.x + reflection.x * 1.0;
                rs.y = intersection.y + reflection.y * 1.0;
                rs.z = intersection.z + reflection.z * 1.0;

                //reflection = Normalized(reflection);

                Ray reflectionRay(rs, reflection);
                int nearest = -1;

                point reflectColor;
                double t_min = 9999;
                for(int k = 0; k < objects.size(); k++)
                {
                    double t = objects[k]->getT(&reflectionRay);

                    if(t <= 0)
                        continue;
                    else if(t < t_min)
                    {
                        t_min = t;
                        nearest = k;
                    }
                }

                if(nearest != -1)
                {
                    double t = objects[nearest]->intersect(&reflectionRay, &reflectColor, level + 1);
                    if(t != -1)
                    {
                        current_color->x += reflectColor.x*co_efficients[REFLECTION];
                        current_color->y += reflectColor.y*co_efficients[REFLECTION];
                        current_color->z += reflectColor.z*co_efficients[REFLECTION];
                    }

                }
            }

            //Check whether all current_color pixel value is within 1 or 0 if not set it
            if(current_color->x > 1)
                current_color->x = 1;
            if(current_color->x < 0)
                current_color->x = 0;
            if(current_color->y > 1)
                current_color->y = 1;
            if(current_color->y < 0)
                current_color->y = 0;
            if(current_color->z > 1)
                current_color->z = 1;
            if(current_color->z < 0)
                current_color->z = 0;
        }

        return t;
    }
};

class Floor: public Object
{
public:

    int isSquare=0;
    Floor(point p,double base)
    {
        reference_point=p;
        length=base;
        isSquare=1;

    }

    Floor(double floor_width, double tile_width)
    {
        reference_point.x = -floor_width/2.0;
        reference_point.y = -floor_width/2.0;
        reference_point.z = 0;
        length = tile_width;
        height = 0;
        isSquare=0;

    }


    point getNormal(point pointed)
    {
        point normal;
        normal.x = 0;
        normal.y = 0;
        normal.z = 1;
        return normal;
    }

    double getT(Ray *r)
    {
        point normal = getNormal(reference_point);
        return ((-1.0) * dotProduct(normal, r->start) / dotProduct(normal, r->dir));
    }

    double intersect(Ray *r, point* current_color, int level)
    {
        double t = getT(r);

        point intersection;
        intersection.x = r->start.x + r->dir.x * t;
        intersection.y = r->start.y + r->dir.y * t;
        intersection.z = r->start.z + r->dir.z * t;

        if(intersection.x < reference_point.x || intersection.x > -reference_point.x || intersection.y < reference_point.y || intersection.y > -reference_point.y)
        {
            return -1;
        }
        if(level == 0)
            return t;

        int x = (intersection.x - reference_point.x) / length;
        int y = (intersection.y - reference_point.y) / length;
        if(isSquare==0)
        {
            if((x + y) % 2 == 0)
            {
                color[0] = color[1] = color[2] = 0;
            }
            else
            {
                color[0] = color[1] = color[2] = 1;
            }
        }


        current_color->x = color[0] * co_efficients[AMBIENT] ;
        current_color->y = color[1] * co_efficients[AMBIENT] ;
        current_color->z = color[2] * co_efficients[AMBIENT] ;

        point normal = getNormal(intersection);
        point reflection = getReflection(r, normal);

        for(int i = 0; i < lights.size(); i++)
        {
            point ldir;
            ldir.x = lights[i].x - intersection.x;
            ldir.y = lights[i].y - intersection.y;
            ldir.z = lights[i].z - intersection.z;

            double len = sqrt(pow(ldir.x,2) + pow(ldir.y,2) + pow(ldir.z,2));

            ldir = Normalized(ldir);

            point lstart;
            lstart.x = intersection.x + ldir.x * 1.0;
            lstart.y = intersection.y + ldir.y * 1.0;
            lstart.z = intersection.z + ldir.z * 1.0;

            Ray L(lstart, ldir);

            bool obscured = false;

            for(int j = 0; j < objects.size(); j++)
            {
                double possibleobscure = objects[j]->getT(&L);
                //printf("%f ", possibleobscure);
                if(possibleobscure > 0 && abs(possibleobscure) < len)
                {
                    obscured = true;
                    break;
                }
            }
            //printf("\n");

            if(!obscured)
            {
                //printf("direct");
                //L.dir.x = -L.dir.x;
                //L.dir.y = -L.dir.y;
                //L.dir.z = -L.dir.z;
                double lambert = dotProduct(L.dir, normal);
                //double phong = dotProduct(reflection, r->dir);
                point oppo;
                oppo.x = -r->dir.x;
                oppo.y = -r->dir.y;
                oppo.z = -r->dir.z;
                double phong = dotProduct(getReflection(&L, normal), oppo);
                //if(lambert > 0 || phong > 0) printf("emon to hobar kotha na\n");
                lambert = lambert > 0?lambert:0;
                phong = phong > 0?phong:0;

                current_color->x += source_factor * color[0] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
                current_color->y += source_factor * color[1] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
                current_color->z += source_factor * color[2] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
            }

            if(level < recursion_level)
            {
                point rs;
                rs.x = intersection.x + reflection.x * 1.0;
                rs.y = intersection.y + reflection.y * 1.0;
                rs.z = intersection.z + reflection.z * 1.0;

                //reflection = Normalized(reflection);

                Ray reflectionRay(rs, reflection);
                int nearest = -1;

                point reflectColor;
                double t_min = 9999;
                for(int k = 0; k < objects.size(); k++)
                {
                    double t = objects[k]->getT(&reflectionRay);

                    if(t <= 0)
                        continue;
                    else if(t < t_min)
                    {
                        t_min = t;
                        nearest = k;
                    }
                }

                if(nearest != -1)
                {
                    double t = objects[nearest]->intersect(&reflectionRay, &reflectColor, level + 1);
                    if(t!=-1)
                    {
                        current_color->x += reflectColor.x*co_efficients[REFLECTION];
                        current_color->y += reflectColor.y*co_efficients[REFLECTION];
                        current_color->z += reflectColor.z*co_efficients[REFLECTION];
                    }
                }
            }

            //Check whether all current_color pixel value is within 1 or 0 if not set it
            if(current_color->x > 1)
                current_color->x = 1;
            if(current_color->x < 0)
                current_color->x = 0;
            if(current_color->y > 1)
                current_color->y = 1;
            if(current_color->y < 0)
                current_color->y = 0;
            if(current_color->z > 1)
                current_color->z = 1;
            if(current_color->z < 0)
                current_color->z = 0;
        }

        return t;
    }

    void draw1()
    {

        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_QUADS);
        {
            glVertex3f( reference_point.x, reference_point.y,reference_point.z);
            glVertex3f( reference_point.x+length, reference_point.y,reference_point.z);
            glVertex3f( reference_point.x+length, reference_point.y+length,reference_point.z);
            glVertex3f( reference_point.x, reference_point.y+length,reference_point.z);
        }
        glEnd();

    }
    void draw()
    {
        point temp = reference_point;

        int black = 1;
        while(temp.y < -reference_point.y)
        {
            while(temp.x < -reference_point.x)
            {
                if(black == 1)
                {
                    glColor3f(0, 0, 0);
                }
                else
                {
                    glColor3f(1, 1, 1);
                }

                black = 1 - black;
                glBegin(GL_QUADS);
                glVertex3f(temp.x, temp.y, temp.z);
                glVertex3f(temp.x, temp.y + length, temp.z);
                glVertex3f(temp.x + length, temp.y + length, temp.z);
                glVertex3f(temp.x + length, temp.y, temp.z);
                glEnd();

                temp.x += length;
            }
            temp.x = reference_point.x;
            temp.y += length;
            black = 1 - black;
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
        point u, v;
        u.x = b.x - a.x;
        u.y = b.y - a.y;
        u.z = b.z - a.z;

        v.x = c.x - a.x;
        v.y = c.y - a.y;
        v.z = c.z - a.z;
        point normal = crossProduct(u, v);
        normal = Normalized(normal);

        if(dotProduct(pointed, normal) > 0)
        {
            normal.x = -normal.x;
            normal.y = -normal.y;
            normal.z = -normal.z;
        }

        return normal;
    }

    double getT(Ray *r)
    {
        point edge1, edge2;
        edge1.x = b.x - a.x;
        edge1.y = b.y - a.y;
        edge1.z = b.z - a.z;

        edge2.x = c.x - a.x;
        edge2.y = c.y - a.y;
        edge2.z = c.z - a.z;

        point tempostart = r->start;
        point tempodir = r->dir;

        point h = crossProduct(tempodir, edge2);
        double aa = dotProduct(edge1, h);
        if(aa > -EPSILON && aa < EPSILON)
        {
            return -1;
        }
        double f = 1/aa;

        point s;
        s.x = tempostart.x - a.x;
        s.y = tempostart.y - a.y;
        s.z = tempostart.z - a.z;

        double u = f * dotProduct(s, h);
        if(u < 0.0 || u > 1.0)
            return -1;

        point q = crossProduct(s, edge1);
        double v = f * dotProduct(tempodir, q);
        if(v < 0.0 || u + v > 1.0)
            return -1;

        double t = f * dotProduct(edge2, q);
        if(t > EPSILON)
            return t;

        return -1;

    }

    double intersect(Ray *r, point* current_color, int level)
    {
        double t = getT(r);
        if(t<=0)
        {
            return -1;
        }
        if(level == 0)
        {
            return t;
        }

        point intersection;
        intersection.x = r->start.x + r->dir.x * t;
        intersection.y = r->start.y + r->dir.y * t;
        intersection.z = r->start.z + r->dir.z * t;

        current_color->x = color[0] * co_efficients[AMBIENT];
        current_color->y = color[1] * co_efficients[AMBIENT];
        current_color->z = color[2] * co_efficients[AMBIENT];


        point normal = getNormal(r->dir);
        point reflection = getReflection(r, normal);

        for(int i = 0; i < lights.size(); i++)
        {
            point ldir;
            ldir.x = lights[i].x - intersection.x;
            ldir.y = lights[i].y - intersection.y;
            ldir.z = lights[i].z - intersection.z;

            double len = sqrt(pow(ldir.x,2) + pow(ldir.y,2) + pow(ldir.z,2));
            ldir = Normalized(ldir);

            point lstart;
            lstart.x = intersection.x + ldir.x * 1.0;
            lstart.y = intersection.y + ldir.y * 1.0;
            lstart.z = intersection.z + ldir.z * 1.0;


            Ray L(lstart, ldir);

            bool obscured = false;

            for(int j = 0; j < objects.size(); j++)
            {
                double possibleobscure = objects[j]->getT(&L);
                //printf(",%f %d, ", possibleobscure, j);
                if(possibleobscure > 0 && abs(possibleobscure) <= len)
                {
                    obscured = true;
                    break;
                }
            }
            //printf("\n");

            if(!obscured)
            {
                //printf("direct");

                double lambert = dotProduct(L.dir, normal);
                double phong = dotProduct(reflection, r->dir);

                //if(lambert > 0 || phong > 0) printf("emon to hobar kotha na\n");
                lambert = lambert > 0?lambert:0;
                phong = phong > 0?phong:0;

                current_color->x += source_factor * color[0] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
                current_color->y += source_factor * color[1] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
                current_color->z += source_factor * color[2] * (lambert * co_efficients[DIFFUSE] + pow(phong, Shine) * co_efficients[SPECULAR]);
            }

            if(level < recursion_level)
            {
                point rs;
                rs.x = intersection.x + reflection.x * 1.0;
                rs.y = intersection.y + reflection.y * 1.0;
                rs.z = intersection.z + reflection.z * 1.0;

                //reflection = Normalized(reflection);

                Ray reflectionRay(rs, reflection);
                int nearest = -1;

                point reflectColor;
                double t_min = 9999;
                for(int k = 0; k < objects.size(); k++)
                {
                    double t = objects[k]->getT(&reflectionRay);

                    if(t <= 0)
                        continue;
                    else if(t < t_min)
                    {
                        t_min = t;
                        nearest = k;
                    }
                }

                if(nearest != -1)
                {
                    double t = objects[nearest]->intersect(&reflectionRay, &reflectColor, level + 1);
                    if(t != -1)
                    {
                        current_color->x += reflectColor.x*co_efficients[REFLECTION];
                        current_color->y += reflectColor.y*co_efficients[REFLECTION];
                        current_color->z += reflectColor.z*co_efficients[REFLECTION];
                    }


                }
            }

            //Check whether all current_color pixel value is within 1 or 0 if not set it
            if(current_color->x > 1)
                current_color->x = 1;
            if(current_color->x < 0)
                current_color->x = 0;
            if(current_color->y > 1)
                current_color->y = 1;
            if(current_color->y < 0)
                current_color->y = 0;
            if(current_color->z > 1)
                current_color->z = 1;
            if(current_color->z < 0)
                current_color->z = 0;
        }

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
    vector<Triangle*>triangle;
    Floor *square;

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

        p5.x=p1.x+base/2;
        p5.y=p1.y+base/2;
        p5.z=p1.z+height;

        Triangle *t ;
        t =new Triangle(p1,p5,p2);
        triangle.push_back(t);
        t =new Triangle(p2,p5,p3);
        triangle.push_back(t);
        t =new Triangle(p3,p5,p4);
        triangle.push_back(t);

        t =new Triangle(p4,p5,p1);
        triangle.push_back(t);

        square=new Floor(p,base);





    }
    double getT(Ray* r)
    {
        for(int i=0; i<triangle.size(); i++)
        {
            triangle[i]->setColor(this->color[0], this->color[1], this->color[2]);
            triangle[i]->setCoEfficients(this->co_efficients[0],this->co_efficients[1],this->co_efficients[2],this->co_efficients[3]);
            triangle[i]->setShine(this->Shine);
        }
        double t_min = 9999;
        for(int i=0; i<triangle.size(); i++)
        {
            double t=triangle[i]->getT(r);
            /// cout<<t<<endl;
            if(t <= 0)
                continue;
            if(t<t_min)
            {
                t_min=t;
            }
        }
        double t=square->getT(r);
        /// cout<<t<<endl;
        if(t <= 0)
            return t_min;
        if(t<t_min)
        {
          //  t_min=t;
        }

        return t_min;

    }

    void draw()
    {
        for(int i=0; i<triangle.size(); i++)
        {
            triangle[i]->setColor(this->color[0], this->color[1], this->color[2]);
            triangle[i]->setCoEfficients(this->co_efficients[0],this->co_efficients[1],this->co_efficients[2],this->co_efficients[3]);
            triangle[i]->setShine(this->Shine);
        }
        square->setColor(this->color[0], this->color[1], this->color[2]);
        square->setCoEfficients(this->co_efficients[0],this->co_efficients[1],this->co_efficients[2],this->co_efficients[3]);
        square->setShine(this->Shine);

        square->draw1();
        for(int i=0; i<triangle.size(); i++)
        {
            triangle[i]->draw();
        }

    }
    double intersect(Ray *r, point* current_color, int level)
    {
        for(int i=0; i<triangle.size(); i++)
        {
            triangle[i]->setColor(this->color[0], this->color[1], this->color[2]);
            triangle[i]->setCoEfficients(this->co_efficients[0],this->co_efficients[1],this->co_efficients[2],this->co_efficients[3]);
            triangle[i]->setShine(this->Shine);
        }

        square->setColor(this->color[0], this->color[1], this->color[2]);
        square->setCoEfficients(this->co_efficients[0],this->co_efficients[1],this->co_efficients[2],this->co_efficients[3]);
        square->setShine(this->Shine);
        double t_min = 9999;
        for(int i=0; i<triangle.size(); i++)
        {
            double t=triangle[i]->intersect(r,current_color,level);
            if(t <= 0)
                continue;
            if(t<t_min)
            {
                t_min=t;
            }
        }

        double t=square->intersect(r,current_color,level);
        if(t <= 0)
            return t_min;
        if(t<t_min)
        {
           // t_min=t;
        }
        return t_min;

    }

};
