#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))


int drawaxes;
double Length ;

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

point pos(100,100,0),u(0,0,1),r(-1.0/sqrt(2.0),1.0/sqrt(2.0),0),l(-1.0/sqrt(2),-1.0/sqrt(2),0);

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

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,0);
        glVertex3f( a,-a,0);
        glVertex3f(-a,-a,0);
        glVertex3f(-a, a,0);
    }
    glEnd();
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
        Length += 1.0;
        if(Length > 40)
            Length = 40;
        break;
    case GLUT_KEY_END:
        Length -= 1.0;
        if(Length < 0)
            Length = 0;
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


void drawQuarterSphere(double radius)
{
    int slices = 100;
    int stacks = 100;
    struct point points[stacks+5][slices+5];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*0.5*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*0.5*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    /// glColor3f(1, 0, 0);
    for(i=0; i<stacks; i++)
    {
        for(j=0; j<slices; j++)
        {
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
            }
            glEnd();
        }
    }
}


void drawCylinderPart()
{
    int i,j;
    int segments = 100;
    int stacks = 100;
    double radius = 40-Length;
    double length = Length;
    struct point points[stacks+5][segments+5];
    ///glColor3f(0,1,0);
    //generate points
    for(i=0; i<=stacks; i++)
    {
        for(j=0; j<=segments; j++)
        {
            points[i][j].x=radius*cos(((double)j/(double)segments)*0.5*pi);
            points[i][j].z=radius*sin(((double)j/(double)segments)*0.5*pi);
            points[i][j].y = length - 2*length * i / stacks;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        for(j=0; j<segments; j++)
        {
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
        }
            glEnd();
        }
    }
}


void drawingCube()
{
    glColor3f(1,1,1);
    {
        /// draw two sides parallel to xz plane

        glPushMatrix();
        glTranslated(-40,0,0);
        glRotated(90,0,1,0);
        drawSquare(Length);
        glPopMatrix();

        glPushMatrix();
        glTranslated(40,0,0);
        glRotated(90,0,1,0);
        drawSquare(Length);
        glPopMatrix();

    }
    {
        /// draw two sides parallel to yz plane
        ///  glColor3f(0,1,0);
        glPushMatrix();
        glTranslated(0,-40,0);
        glRotated(90,1,0,0);
        drawSquare(Length);
        glPopMatrix();


        glPushMatrix();
        glTranslated(0,40,0);
        glRotated(90,1,0,0);
        drawSquare(Length);
        glPopMatrix();


    }


    {
        /// draw two sides parallel to xy plane
        /// glColor3f(0,0,1);
        glPushMatrix();
        glTranslated(0,0,-40);
        drawSquare(Length);
        glPopMatrix();

        glPushMatrix();
        glTranslated(0,0,40);
        drawSquare(Length);
        glPopMatrix();
    }



}

void drawingSphere()
{

    glColor3f(1,0,0);

    {
        glPushMatrix();
        glTranslatef(Length,Length,Length);
        drawQuarterSphere(40-Length);
        glPopMatrix();

    }
    {
        glPushMatrix();
        glTranslatef(-Length,Length,Length);
        glRotatef(90, 0, 0, 1);
        drawQuarterSphere(40-Length);
        glPopMatrix();

    }


    {
        glPushMatrix();
        glTranslatef(-Length,-Length,Length);
        glRotatef(180, 0, 0, 1);
        drawQuarterSphere(40-Length);
        glPopMatrix();
    }


    {
        glPushMatrix();
        glTranslatef(Length,-Length,Length);
        glRotatef(270, 0, 0, 1);
        drawQuarterSphere(40-Length);
        glPopMatrix();

    }
    {
        glPushMatrix();
        glTranslatef(Length,Length,-Length);
        glRotatef(90, 0, 1, 0);
        drawQuarterSphere(40-Length);
        glPopMatrix();
    }




    {
        glPushMatrix();
        glTranslatef(-Length,Length,-Length);
        glRotatef(90, 0, 0, 1);
        glRotatef(90, 0, 1, 0);
        drawQuarterSphere(40-Length);
        glPopMatrix();

    }



    {
        glPushMatrix();
        glTranslatef(-Length,-Length,-Length);
        glRotatef(180, 0, 0, 1);
        glRotatef(90, 0, 1, 0);
        drawQuarterSphere(40-Length);
        glPopMatrix();
    }


    {
        glPushMatrix();
        glTranslatef(Length,-Length,-Length);
        glRotatef(270, 0, 0, 1);
        glRotatef(90, 0, 1, 0);
        drawQuarterSphere(40-Length);
        glPopMatrix();

    }


}

void drawingCylinder()
{
    glColor3f(0,1,0);
    {
        glPushMatrix();
        glTranslatef(Length,0,Length);
        drawCylinderPart();
        glPopMatrix();
    }

    {
        glPushMatrix();

        glTranslatef(Length,0,-Length);
        glRotated(90,0,1,0);
        drawCylinderPart();

        glPopMatrix();

    }

    {
        glPushMatrix();

        glTranslatef(0,Length,Length);
        glRotatef(90, 0,0,1);
        drawCylinderPart();

        glPopMatrix();

    }

    {
        glPushMatrix();
        glTranslatef(0,Length,-Length);
        glRotatef(90, 0,0,1);
        glRotated(90,0,1,0);
        drawCylinderPart();

        glPopMatrix();
    }
    {
        glPushMatrix();
        glTranslatef(Length,Length,0);
        glRotatef(90, 1,0,0);
        glRotated(90,0,1,0);
        drawCylinderPart();


        glPopMatrix();
    }
    {
        glPushMatrix();

        glTranslatef(-Length,0,Length);
        glRotatef(180, 0,0,1);
        drawCylinderPart();

        glPopMatrix();

    }
    {
        glPushMatrix();

        glTranslatef(-Length,0,-Length);
        glRotatef(180, 0,0,1);
        glRotated(90,0,1,0);
        drawCylinderPart();

        glPopMatrix();

    }
    {
        glPushMatrix();

        glTranslatef(0,-Length,Length);
        glRotatef(270, 0,0,1);
        drawCylinderPart();

        glPopMatrix();
    }
    {
        glPushMatrix();
        glTranslatef(0,-Length,-Length);
        glRotatef(270, 0,0,1);
        glRotated(90,0,1,0);
        drawCylinderPart();

        glPopMatrix();

    }
    {
        glPushMatrix();
        glTranslatef(-Length,Length,0);
        glRotatef(180, 0,0,1);
        glRotatef(90, 1,0,0);
        drawCylinderPart();

        glPopMatrix();
    }
    {
        glPushMatrix();
        glTranslatef(-Length,-Length,0);
        glRotatef(270, 0,0,1);
        glRotatef(90, 1,0,0);
        drawCylinderPart();

        glPopMatrix();

    }
    {
        glTranslatef(Length,-Length,0);
        glRotatef(-90, 0,0,1);
        glRotatef(90, 1,0,0);
        glRotated(90,0,1,0);
        drawCylinderPart();
        glPopMatrix();

    }




}
void drawProblem()
{
    drawingCube();
    drawingSphere();
    drawingCylinder();

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
    drawAxes();
    drawProblem();
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
    drawaxes=1;
    Length=20;


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
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("Offline 1");

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
