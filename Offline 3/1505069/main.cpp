
#include "header.h"

#define FOV 90
#define window_height 500
#define window_width 500


int drawaxes;
int drawgrid;

int numObject;
int numLight;
vector<Object*> objects;
vector<point> lights;
int recursion_level;

int image_width;
int image_height;

point pos(0,-200,50),u(0,0,1),r(1,0,0),l(0,1,0);
void drawAxes()
{
    if(drawaxes==1)
    {
        glBegin(GL_LINES);
        {
            glColor3f(1, 0, 0);
            glVertex3f( 500,0,0);
            glVertex3f(-500,0,0);

            glColor3f(0, 1, 0);
            glVertex3f(0,-500,0);
            glVertex3f(0, 500,0);

            glColor3f(0, 0, 1);
            glVertex3f(0,0, 500);
            glVertex3f(0,0,-500);
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
                    continue;	//SKIP the MAIN axes

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
void capture()
{
    bitmap_image bitmap_image(image_width, image_height);
    double plane_distance = (window_height/2.0)/tan((FOV/2.0) * (pi / 180.0));

    point topleft;
    topleft.x = pos.x + l.x * plane_distance - r.x * (window_width/2.0) + u.x * (window_height/2.0);
    topleft.y = pos.y + l.y * plane_distance - r.y * (window_width/2.0) + u.y * (window_height/2.0);
    topleft.z = pos.z + l.z * plane_distance - r.z * (window_width/2.0) + u.z * (window_height/2.0);

    //printf("topleft is %f, %f, %f\n", topleft.x, topleft.y, topleft.z);

    double du = (1.0*window_width)/image_width;
    double dv = (1.0*window_height)/image_height;

    //cout<<du<<" "<<dv<<endl;

    //ofstream fout("corners.txt");
    for(int i = 0; i < image_height; i++)
    {
        for(int j = 0; j < image_width; j++)
        {
            //printf("%d ", i*j);
            struct point corner;
            corner.x = topleft.x + r.x * j * du - u.x * i * dv;
            corner.y = topleft.y + r.y * j * du - u.y * i * dv;
            corner.z = topleft.z + r.z * j * du - u.z * i * dv;

            //fout<<corner.x<<", "<<corner.y<<", "<<corner.z<<endl;
            struct point diff;
            diff.x = corner.x - pos.x;
            diff.y = corner.y - pos.y;
            diff.z = corner.z - pos.z;

            diff = Normalized(diff);
            Ray newRay(pos, diff);

            //printf("%f %f %f\n", diff2.x, diff2.y, diff2.z);
            int nearest = -1;

            struct point dummyColor;
            double t_min = 9999;
            for(int k = 0; k < objects.size(); k++)
            {
                double t = objects[k]->intersect(&newRay, &dummyColor, 0);

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
                //cout<<"not inside yet"<<endl;
                double t = objects[nearest]->intersect(&newRay, &dummyColor, 1);

                //if(dummyColor.x == 1) printf("                          yes");
                bitmap_image.set_pixel(j, i, dummyColor.x * 255, dummyColor.y * 255, dummyColor.z * 255);
            }
        }
    }

    bitmap_image.save_image("out.bmp");
    cout<<"Image captured"<<endl;
    // exit(0);
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
    drawAxes();
    for(int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }

    for(int i = 0; i < lights.size(); i++)
    {
        glBegin(GL_POINTS);
        glColor3f(1, 1, 1);
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
    drawaxes=0;
    drawgrid=0;


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

void loadActualData()
{
    ifstream fin("description.txt");
    fin>>recursion_level;
    //cout<<recursion_level<<endl;

    fin>>image_width;
    image_height = image_width;
    //cout<<image_width <<" "<<image_height<<endl;

    fin>>numObject;
    //cout<<numObject<<endl;

    Object *temp;
    for(int i = 0; i < numObject; i++)
    {
        string str;
        fin>>str;
        //cout<<str<<endl;
        if(str.compare("sphere") == 0 || str.compare("Sphere") == 0)
        {
            struct point center;
            double radius;
            fin>>center.x>>center.y>>center.z;
            //cout<<center.x<<" "<<center.y<<" "<<center.z<<endl;
            fin>>radius;
            //cout<<radius<<endl;
            temp = new Sphere(center, radius);
            double r, g, b, a, d, s, re;
            fin>>r>>g>>b;
            //cout<<r<<" "<<g<<" "<<b<<endl;
            temp->setColor(r, g, b);
            fin>>a>>d>>s>>re;
            //cout<<a<<" "<<d<<" "<<s<<" "<<re<<endl;
            temp->setCoEfficients(a, d, s, re);
            int shine;
            fin>>shine;
            //cout<<shine<<endl;
            temp->setShine(shine);
            objects.push_back(temp);
        }
        else if(str.compare("pyramid") == 0 || str.compare("pyramid") == 0)
        {
            point p;

            fin>>p.x>>p.y>>p.z;
            ///  cout<<p.x<<" "<<p.y<<" "<<p.z<<endl;
            double base,height;
            fin>>base>>height;
            ///  cout<<base<<" "<<height<<endl;
            temp = new Pyramid(p,  base,height);
            double r, g, b, a, d, s, re;
            fin>>r>>g>>b;
            //cout<<r<<" "<<g<<" "<<b<<endl;
            temp->setColor(r, g, b);
            fin>>a>>d>>s>>re;
            //cout<<a<<" "<<d<<" "<<s<<" "<<re<<endl;
            temp->setCoEfficients(a, d, s, re);
            int shine;
            fin>>shine;
            //cout<<shine<<endl;
            temp->setShine(shine);
            objects.push_back(temp);
        }
    }
    fin>>numLight;
    //cout<<numLight<<endl;
    for(int i = 0; i < numLight; i++)
    {
        struct point light;
        fin>>light.x>>light.y>>light.z;
        //cout<<light.x<<" "<<light.y<<" "<<light.z<<endl;
        lights.push_back(light);
    }

    temp = new Floor(1000, 30);
    temp->setCoEfficients(0.4, 0.2, 0.2, 0.2);
    temp->setShine(1);
    objects.push_back(temp);
}
void loadTestData()
{
    image_width = image_height = 768;
    recursion_level = 4;

    Object *temp;
    struct point C;
    double R;
    C.x = 10;
    C.y = 50;
    C.z = 10;
    R = 10.0;
    temp = new Sphere(C, R);
    temp->setColor(1, 0, 0);
    temp->setCoEfficients(0.4, 0.2, 0.2, 0.2);
    temp->setShine(1);

    objects.push_back(temp);

    struct point light1;
    light1.x = -30;
    light1.y = 20;
    light1.z = 20;
    lights.push_back(light1);

    /*light1.x = -60;
    light1.y = -20;
    light1.z = 10;
    lights.push_back(light1);*/

    C.x = -60;
    C.y = 15;
    C.z = 25;
    R = 25.0;
    temp = new Sphere(C, R);
    temp->setColor(1, 1, 0);
    temp->setCoEfficients(0.4, 0.2, 0.2, 0.2);
    temp->setShine(1);

    objects.push_back(temp);

    struct point a, b, c;
    a.x = -40, a.y = -40, a.z = 0;
    b.x = 40, b.y = -40, b.z = 0;
    c.x = 0, c.y = -40, c.z = 40;

    temp = new Triangle(a, b, c);
    temp->setColor(0, 1, 0);
    temp->setCoEfficients(0.4, 0.2, 0.2, 0.2);
    temp->setShine(1);
    objects.push_back(temp);

    temp = new Floor(1000, 20);
    temp->setCoEfficients(0.4, 0.2, 0.2, 0.2);
    temp->setShine(1);
    objects.push_back(temp);

    struct point r;
    r.x = 0;
    r.y = 0;
    r.z = 0;

    /*temp = new generalQuadratic(1, 1, 1, 0, 0, 0, 0, 0, 0, -100, r, 0, 0, 20);
    temp->setColor(0, 1, 0);
    temp->setCoEfficients(0.4, 0.2, 0.1, 0.3);
    temp->setShine(10);
    objects.push_back(temp);*/


}
int main(int argc, char **argv)
{
     //loadTestData();
    loadActualData();
    glutInit(&argc,argv);
    glutInitWindowSize(600, 600);
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
