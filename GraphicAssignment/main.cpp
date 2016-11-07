#include "MfileParser.h"
#include <Gl\glut.h>//should be put at the last
#include "RenderMode.h"

#define TRANSFORM_NONE    0 
#define TRANSFORM_ROTATE  1
#define TRANSFORM_SCALE 2 
#define RADPERDEG 0.10

MFileParser *parser;
int mainWin;
GLUI_Rotation *glRotateBall;
double sphi = 90.0, stheta = 45.0, sdepth = 10;	// for simple trackball
double xpan = 0.0, ypan = 0.0;				// for simple trackball
double zNear = 1.0, zFar = 100.0;
double g_fov = 45.0;
Vector3Point g_center;

static int press_x, press_y;
static float x_angle = 0.0;
static float y_angle = 0.0;
static float scale_size = 1;


static int xform_mode = 0;

void drawGround();
void drawAxis();
void draw_z_axis();
void draw_y_axis();
void draw_x_axis();
void mymotion(int x, int y);
void drawAxes(GLdouble r);
void mymouse(int button, int state, int x, int y);

void myDisplay(void)
{
	glEnable(GL_DEPTH_TEST);
	// Just clean the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// setup the perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, .1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 10, g_center.x,g_center.y,g_center.z, 0, 1, 0);
	//gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
	glTranslatef(xpan, ypan, -sdepth);
	glRotatef(-stheta, 1.0, 0.0, 0.0);
	//glRotatef(sphi, 0.0, 1.0, 0.0);
    glTranslatef(-g_center.x, -g_center.y, -g_center.z);
	//gluLookAt(0, 0, 10, 0, 0, 0, 0, 0, 0);
	//gluLookAt(0,0,5,0,0,0,0,1,0); 
	//drawAxes(10.0);
	glEnable(GL_CULL_FACE);
	glScalef(scale_size, scale_size, scale_size);
	glRotatef(x_angle, 0, 1, 0);
	glRotatef(y_angle, 1, 0, 0);
	parser->display();
	parser->setBoundingBox();
	g_center = (parser->maxCoord + parser->minCoord) * 0.5;
	drawGround();
	//drawAxis();
	//drawAxes(10);
	//glTranslatef(-g_center.x, -g_center.y, -g_center.z);
	
	glFlush();
	glutSwapBuffers();
	//glutPostRedisplay();


}

void initGL(void)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glPolygonOffset(1.0, 1.0);
	//glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	GLfloat light0Position[] = { 0, 1, 0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glEnable(GL_LIGHT0);

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	gluOrtho2D(-9.2,9.2, -9.2, 9.2);
	parser = new MFileParser();
	parser->parserFile("meshes/cap.m");
	parser->setmode(FLAT_SHADING);
	

}

void menuCallBack(int key)
{

}

void initMenu()
{
	glutInitWindowSize(500, 500);
	mainWin = glutCreateWindow("Huang Zhe 's Graphic Assignment");
	GLUI *glui = GLUI_Master.create_glui_subwindow(mainWin, GLUI_SUBWINDOW_RIGHT);
	GLUI_Panel *panel = glui->add_panel("file");
	GLUI_Button *openFileButton = glui->add_button_to_panel(panel, "Open File");
	glRotateBall = glui->add_rotation("Rotate\nOr use mouse left button", 0, -1, menuCallBack);
		//("Open File", -1, menuCallBack);
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	initMenu();
	initGL();
	glutDisplayFunc(&myDisplay);
	glutMotionFunc(mymotion);
	glutMouseFunc(mymouse);
	glutMainLoop();
	return 0;
}

void mymouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		press_x = x; press_y = y;
		if (button == GLUT_LEFT_BUTTON)
			xform_mode = TRANSFORM_ROTATE;
		else if (button == GLUT_MIDDLE_BUTTON)
			xform_mode = TRANSFORM_SCALE;
	}
	else if (state == GLUT_UP)
	{
		xform_mode = TRANSFORM_NONE;
	}
}

void mymotion(int x, int y)
{
	if (xform_mode == TRANSFORM_ROTATE)
	{
		x_angle += (x - press_x) / 5.0;

		if (x_angle > 180)
			x_angle -= 360;
		else if (x_angle <-180)
			x_angle += 360;

		press_x = x;

		y_angle += (y - press_y) / 5.0;

		if (y_angle > 180)
			y_angle -= 360;
		else if (y_angle <-180)
			y_angle += 360;

		press_y = y;
	}
	else if (xform_mode == TRANSFORM_SCALE)
	{
		float old_size = scale_size;

		scale_size *= (1 + (y - press_y) / 60.0);

		if (scale_size <0)
			scale_size = old_size;
		press_y = y;
	}

	// force the redraw function
	glutPostRedisplay();
}

void MyDrawLineFunc(int a, int b, int c, int d)
{
	glBegin(GL_LINES);
	glColor3f(0.9, 0.9, 0.9);
	glVertex2i(a, b);
	glVertex2i(c, d);
	glEnd();
}

void drawGround()
{
	for (int i = -100; i <= 100; ++i) 
	{
		MyDrawLineFunc(i, -100, i, 100);
		MyDrawLineFunc(-100, i, 100, i);
	}

}


void draw_z_axis()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
		glTranslated(0, 0, 10);
		glutSolidCone(1, 10, 30, 30);
		glColor3f(0, 0, 1);
	
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);

	gluCylinder(qobj,1,1,10,30,30);
	gluDeleteQuadric(qobj);
	glPopMatrix();
}

void draw_y_axis()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
		glRotated(-90, 1, 0, 0);
		draw_z_axis();
	glPopMatrix();
}

void draw_x_axis()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
		glRotated(90, 0, 1, 0);
		draw_z_axis();
	glPopMatrix();
}

void drawAxis()
{
	glColor3f(0, 0, 1);
	draw_z_axis();
	glColor3f(0, 1, 0);
	draw_y_axis();
	glColor3f(1, 0, 0);
	draw_x_axis();
}


/////////


void Arrow(GLdouble x1,GLdouble y1,GLdouble z1,GLdouble x2,GLdouble y2,GLdouble z2,GLdouble D)
{
  double x=x2-x1;
  double y=y2-y1;
  double z=z2-z1;
  double L=sqrt(x*x+y*y+z*z);

    GLUquadricObj *quadObj;

    glPushMatrix ();

      glTranslated(x1,y1,z1);

      if((x!=0.)||(y!=0.)) {
        glRotated(atan2(y,x)/RADPERDEG,0.,0.,1.);
        glRotated(atan2(sqrt(x*x+y*y),z)/RADPERDEG,0.,1.,0.);
      } else if (z<0){
        glRotated(180,1.,0.,0.);
      }

      glTranslatef(0,0,L-4*D);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluCylinder(quadObj, 2*D, 0.0, 4*D, 32, 1);
      gluDeleteQuadric(quadObj);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluDisk(quadObj, 0.0, 2*D, 32, 1);
      gluDeleteQuadric(quadObj);

      glTranslatef(0,0,-L+4*D);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluCylinder(quadObj, D, D, L-4*D, 32, 1);
      gluDeleteQuadric(quadObj);

      quadObj = gluNewQuadric ();
      gluQuadricDrawStyle (quadObj, GLU_FILL);
      gluQuadricNormals (quadObj, GLU_SMOOTH);
      gluDisk(quadObj, 0.0, D, 32, 1);
      gluDeleteQuadric(quadObj);

    glPopMatrix ();

}
void drawAxes(GLdouble length)
{
    glPushMatrix();
    glTranslatef(-length,0,0);
    Arrow(0,0,0, 2*length,0,0, 0.2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,-length,0);
    Arrow(0,0,0, 0,2*length,0, 0.2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,-length);
    Arrow(0,0,0, 0,0,2*length, 0.2);
    glPopMatrix();
}