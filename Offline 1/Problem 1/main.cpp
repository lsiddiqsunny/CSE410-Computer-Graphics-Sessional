#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))


int drawaxes;
double cubeLengths ;

struct point
{
    double x,y,z;
    point(double a,double b,double c)
    {
        x=a;
        y=b;
        z=c;
    }
    point(){
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
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void drawCube(double a)
{




    glBegin(GL_QUADS);
    {
        glVertex3f( a,-a, -a);
        glVertex3f( a,a,-a);
        glVertex3f(a,a,a);
        glVertex3f(a,-a, a);
    }
    glEnd();

    glBegin(GL_QUADS);
    {
        glVertex3f( -a,-a, a);
        glVertex3f( -a,a,a);
        glVertex3f(-a,a,-a);
        glVertex3f(-a,-a, -a);
    }
    glEnd();


    glBegin(GL_QUADS);
    {
        glVertex3f( a,a, a);
        glVertex3f( a,a,-a);
        glVertex3f(-a,a,-a);
        glVertex3f(-a,a, a);
    }
    glEnd();

    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a,-a, -a);
        glVertex3f( a,-a,a);
        glVertex3f(-a,-a,a);
        glVertex3f(-a,-a, -a);
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
        break;
    case GLUT_KEY_UP:		// up arrow key
        pos.x+=l.x*2;
        pos.y+=l.y*2;
        break;

    case GLUT_KEY_RIGHT:
        pos.x+=r.x*2;
        pos.y+=r.y*2;
        break;
    case GLUT_KEY_LEFT:
        pos.x-=r.x*2;
        pos.y-=r.y*2;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.z+=2;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.z-=2;
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
		    cubeLengths += 0.5;
		    if(cubeLengths > 20) cubeLengths = 20;
			break;
    case GLUT_KEY_END:
            cubeLengths -= 0.5;
		    if(cubeLengths < 0) cubeLengths = 0;
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

void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawQuarterSpere(double radius)
{
    int slices = 100;
    int stacks = 95;
    struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*0.5*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*0.5*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	glColor3f(1, 0, 0);
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawQuarterSpereDown(double radius)
{
    int slices = 100;
    int stacks = 95;
    struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*0.5*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*0.5*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
    glColor3f(1,0,0);
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawCylinderPart()
{
    int i,j;
    int segments = 50;
    int stacks = 50;
    double radius = 20-cubeLengths;
    double length = cubeLengths;
    struct point points[100][100];
    glColor3f(0,1,0);
    //generate points
    for(i=0;i<=stacks;i++)
	{
        for(j=0;j<=segments;j++)
            {
                points[i][j].x=radius*cos(((double)j/(double)segments)*0.5*pi);
                points[i][j].z=radius*sin(((double)j/(double)segments)*0.5*pi);
                points[i][j].y = length - 2*length * i / stacks;
            }
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<segments;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
//                //lower hemisphere
//                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
//				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
//				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
//				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawCylinderPartDown()
{
    int i,j;
    int segments = 50;
    int stacks = 50;
    double radius = 20-cubeLengths;
    double length = cubeLengths;
    struct point points[100][100];
    glColor3f(0,1,0);
    //generate points
    for(i=0;i<=stacks;i++)
	{
        for(j=0;j<=segments;j++)
            {
                points[i][j].x=radius*cos(((double)j/(double)segments)*0.5*pi);
                points[i][j].z=radius*sin(((double)j/(double)segments)*0.5*pi);
                points[i][j].y = length - 2*length * i / stacks;
            }
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<segments;j++)
		{
			glBegin(GL_QUADS);{
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawProblem1()
{

    //starting the cube parts
    {
    glColor3f(1,1,1);

    glPushMatrix();
    glTranslatef(0,0,20);
    drawSquare(cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glTranslatef(0,0,-20);
    drawSquare(cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glColor3f(1,1,1);
    glTranslatef(0,20,0);
    glRotatef(90, 1, 0, 0);
    drawSquare(cubeLengths);

    glPopMatrix();
    glPushMatrix();


    glTranslatef(0,-20,0);
    glRotatef(90, 1, 0, 0);
    drawSquare(cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glColor3f(1,1,1);
    glTranslatef(20,0,0);
    glRotatef(90, 0, 1, 0);
    drawSquare(cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glTranslatef(-20,0,0);
    glRotatef(90, 0, 1, 0);
    drawSquare(cubeLengths);


    }

    //Starting the sphere parts
    {
    glPopMatrix();
    glPushMatrix();

    glTranslatef(cubeLengths,cubeLengths,cubeLengths);
    drawQuarterSpere(20-cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glTranslatef(-cubeLengths,cubeLengths,cubeLengths);
    glRotatef(90, 0, 0, 1);
    drawQuarterSpere(20-cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glTranslatef(-cubeLengths,-cubeLengths,cubeLengths);
    glRotatef(180, 0, 0, 1);
    drawQuarterSpere(20-cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glTranslatef(cubeLengths,-cubeLengths,cubeLengths);
    glRotatef(-90, 0, 0, 1);
    drawQuarterSpere(20-cubeLengths);


    glPopMatrix();
    glPushMatrix();

    glTranslatef(cubeLengths,cubeLengths,-cubeLengths);
    drawQuarterSpereDown(20-cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glTranslatef(-cubeLengths,cubeLengths,-cubeLengths);
    glRotatef(90, 0, 0, 1);
    drawQuarterSpereDown(20-cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glTranslatef(-cubeLengths,-cubeLengths,-cubeLengths);
    glRotatef(180, 0, 0, 1);
    drawQuarterSpereDown(20-cubeLengths);

    glPopMatrix();
    glPushMatrix();

    glTranslatef(cubeLengths,-cubeLengths,-cubeLengths);
    glRotatef(-90, 0, 0, 1);
    drawQuarterSpereDown(20-cubeLengths);
    }


    //drawing the cylinder parts
    {
    glPopMatrix();
    glPushMatrix();

    glTranslatef(cubeLengths,0,cubeLengths);
    drawCylinderPart();

    glPopMatrix();
    glPushMatrix();

    glTranslatef(cubeLengths,0,-cubeLengths);
    drawCylinderPartDown();

    glPopMatrix();
    glPushMatrix();

    glTranslatef(0,cubeLengths,cubeLengths);
    glRotatef(90, 0,0,1);
    drawCylinderPart();

    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,cubeLengths,-cubeLengths);
    glRotatef(90, 0,0,1);
    drawCylinderPartDown();

    glPopMatrix();
    glPushMatrix();
    glTranslatef(cubeLengths,cubeLengths,0);
    glRotatef(90, 1,0,0);
    drawCylinderPartDown();


    glPopMatrix();
    glPushMatrix();

    glTranslatef(-cubeLengths,0,cubeLengths);
    glRotatef(180, 0,0,1);
    drawCylinderPart();

    glPopMatrix();
    glPushMatrix();

    glTranslatef(-cubeLengths,0,-cubeLengths);
    glRotatef(180, 0,0,1);
    drawCylinderPartDown();

    glPopMatrix();
    glPushMatrix();

    glTranslatef(0,-cubeLengths,cubeLengths);
    glRotatef(-90, 0,0,1);
    drawCylinderPart();

    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,-cubeLengths,-cubeLengths);
    glRotatef(-90, 0,0,1);
    drawCylinderPartDown();

    glPopMatrix();
    glPushMatrix();
    glTranslatef(-cubeLengths,cubeLengths,0);
    glRotatef(180, 0,0,1);
    glRotatef(90, 1,0,0);
    drawCylinderPart();

    glPopMatrix();
    glPushMatrix();
    glTranslatef(-cubeLengths,-cubeLengths,0);
    glRotatef(-90, 0,0,1);
    glRotatef(90, 1,0,0);
    drawCylinderPart();

    glPopMatrix();
    glTranslatef(cubeLengths,-cubeLengths,0);
    glRotatef(-90, 0,0,1);
    glRotatef(90, 1,0,0);
    drawCylinderPartDown();
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
    //add objects

    drawAxes();

    //glColor3f(1,0,0);

    drawProblem1();





    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
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
    cubeLengths=10;


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