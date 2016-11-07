#include "MfileParser.h"
#include <Gl\glut.h>//should be put at the last
#include "RenderMode.h"

#define TRANSFORM_NONE    0 
#define TRANSFORM_ROTATE  1
#define TRANSFORM_SCALE 2 

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
void drawAxis(float rate);
void draw_z_axis(float rate);
void draw_y_axis(float rate);
void draw_x_axis(float rate);
void mymotion(int x, int y);
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

	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	glPushMatrix();
		glTranslated(parser->minCoord.x, parser->minCoord.y, parser->minCoord.z);
		drawAxis(0.2);
	glPopMatrix();
	g_center = (parser->maxCoord + parser->minCoord) * 0.5;
	drawGround();
	drawAxis(1);
	
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
	gluOrtho2D(-10,10, -10, 10);
	parser = new MFileParser();
	parser->parserFile("meshes/cap.m");
	parser->setmode(LINE_MODE);
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


void draw_z_axis(float rate)
{
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluCylinder(qobj, .1* rate, .1* rate, 10 * rate, 30, 30);
	gluDeleteQuadric(qobj);
	glPushMatrix();
	glTranslated(0, 0, 10 * rate);
	glutSolidCone(0.2*rate, 1 * rate, 32, 32);
	glPopMatrix();
}

void draw_y_axis(float rate)
{
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glPushMatrix();
		glRotated(-90, 1, 0, 0);
		draw_z_axis(rate);
	glPopMatrix();
}

void draw_x_axis(float rate)
{
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glPushMatrix();
		glRotated(90, 0, 1, 0);
		draw_z_axis(rate);
	glPopMatrix();
}

void drawAxis(float rate)
{
	glColor3f(0, 0, 1);
	draw_z_axis(rate);
	glColor3f(0, 1, 0);
	draw_y_axis(rate);
	glColor3f(1, 0, 0);
	draw_x_axis(rate);
}


