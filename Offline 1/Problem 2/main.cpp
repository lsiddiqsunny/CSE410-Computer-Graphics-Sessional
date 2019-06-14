#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double wangle;
double radius=15;
struct point
{
    double x,y,z;
    void print(){printf("%.4f %.4f %.4f\n",x,y,z);}
};

point rotating;
point pos;
point l;
point r;
point u;
void drawAxes()
{
    if(drawaxes==1)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f( 100,0,0);
            glVertex3f(-100,0,0);

            glVertex3f(0,-100,0);
            glVertex3f(0, 100,0);

            glVertex3f(0,0, 100);
            glVertex3f(0,0,-100);
        }
        glEnd();
    }
}


void drawGrid()
{
    int i;
    if(drawgrid==1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for(i=-8; i<=8; i++)
            {

                if(i==0)
                {
                    /// continue;	//SKIP the MAIN axes
                }

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,2);
        glVertex3f( a,-a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(-a, a,2);
    }
    glEnd();
}


void drawCircle()
{
    int segments=100;
    int i;
    struct point points[segments+5];
    glColor3f(1,0,0);
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=pos.x;
        points[i].y=pos.y+radius*cos(((double)i/(double)segments)*2*pi);
        points[i].z=pos.z+radius*sin(((double)i/(double)segments)*2*pi);


    }
    //printf("%.2f %.2f %.2f\n",points[].x,points[25].y,points[25].z);

    rotating=points[75];
    rotating.x+=4.0;
    /// rotating.y+=4;
    /// rotating.z+=4;

    for(i=0; i<segments; i++)
    {
        glPushMatrix();
        glBegin(GL_QUADS);
        {
            glVertex3f(points[i].x,points[i].y,points[i].z);
            glVertex3f(points[i].x+8,points[i].y,points[i].z);
            glVertex3f(points[i].x+8,points[i+1].y,points[i+1].z);
            glVertex3f(points[i].x,points[i+1].y,points[i+1].z);
        }
        glEnd();
        glPopMatrix();
    }

    glPushMatrix();
    glColor3f(0,1,0);


    glBegin(GL_QUADS);
    {
        glVertex3f(points[i].x+2,points[0].y,points[0].z);
        glVertex3f(points[i].x+6,points[0].y,points[0].z);
        glVertex3f(points[i].x+6,points[50].y,points[50].z);
        glVertex3f(points[i].x+2,points[50].y,points[50].z);
    }
    glEnd();
    glPopMatrix();


    glPushMatrix();
    glColor3f(0,0,1);


    glBegin(GL_QUADS);
    {
        glVertex3f(points[i].x+2,points[25].y,points[25].z);
        glVertex3f(points[i].x+6,points[25].y,points[25].z);
        glVertex3f(points[i].x+6,points[75].y,points[75].z);
        glVertex3f(points[i].x+2,points[75].y,points[75].z);
    }
    glEnd();
    glPopMatrix();



}
void Rotate(point &a,point &b,double angle)
{
    a.x=a.x*cos(angle)+b.x*sin(angle);
    a.y=a.y*cos(angle)+b.y*sin(angle);
    a.z=a.z*cos(angle)+b.z*sin(angle);

}
void Multiplication(point &a,point &b,point &c)
{

    a.x=(b.y*c.z-b.z*c.y);
    a.y=(b.z*c.x-b.x*c.z);
    a.z=(b.x*c.y-b.y*c.x);

}

void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {

    case '1':
        drawgrid=1-drawgrid;
        break;
    case 'a':
        angle-=2.0;


        Rotate(r,l,-pi/90.0);
        Multiplication(l,u,r);


        break;
    case 'd':
        angle+=2.0;

        Rotate(r,l,pi/90.0);
        Multiplication(l,u,r);
        break;
    case 's':
        pos.x+=2.0*l.x;
        pos.y+=2.0*l.y;
        wangle+=(360.0/(pi*radius));


        break;
    case 'w':
        pos.x-=2.0*l.x;
        pos.y-=2.0*l.y;
        wangle-=(360.0/(pi*radius));


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
        cameraHeight -= 3.0;
        break;
    case GLUT_KEY_UP:		// up arrow key
        cameraHeight += 3.0;
        break;

    case GLUT_KEY_RIGHT:
        cameraAngle += 0.03;
        break;
    case GLUT_KEY_LEFT:
        cameraAngle -= 0.03;
        break;

    case GLUT_KEY_PAGE_UP:
        break;
    case GLUT_KEY_PAGE_DOWN:
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
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes=1-drawaxes;
        }
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
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();


    gluLookAt(120*cos(cameraAngle), 120*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);


    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/

    drawGrid();
    glPushMatrix();

   /// printf("%.2f %.2f %.2f %.2f\n",pos.x,pos.y,sqrt(pos.x*pos.x+pos.y*pos.y),angle);
    glTranslatef(pos.x,pos.y,radius+pos.z);
    glRotatef(-wangle,r.x,r.y,r.z);
    /// glTranslatef(pos.x,-(pos.y),-(pos.z+radius));

    glRotatef(angle,u.x,u.y,u.z);

    glTranslatef(-rotating.x,-rotating.y,radius+rotating.z);
    drawCircle();
    glPopMatrix();




    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate()
{
    //angle-=0.05;
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //codes for initialization
    drawgrid=1;
    drawaxes=0;
    cameraHeight=100.0;
    cameraAngle=pi/4;

    angle=0;

    pos.x=0;
    pos.y=0;
    pos.z=0;

    r.x=1;
    r.y=0;
    r.z=0;

    l.x=0;
    l.y=1;
    l.z=0;

    u.x=0;
    u.y=0;
    u.z=1;



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
    gluPerspective(80,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

int main(int argc, char **argv)
{
    glutInit(&argc,argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

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
