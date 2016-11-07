#include "MfileParser.h"
#include <Gl\glut.h>//should be put at the last
#include "RenderMode.h"
#define RADPERDEG 0.0174533

MFileParser *parser;
int mainWin;
double sphi = 90.0, stheta = 45.0, sdepth = 10;	// for simple trackball
double xpan = 0.0, ypan = 0.0;				// for simple trackball
double zNear = 1.0, zFar = 100.0;
double g_fov = 45.0;
Vector3Point g_center;
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

	//glClear(GL_COLOR_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glTranslatef(xpan, ypan, -sdepth);
	glRotatef(-stheta, 1.0, 0.0, 0.0);
	//glRotatef(sphi, 0.0, 1.0, 0.0);
	glTranslatef(-g_center.x, -g_center.y, -g_center.z);
	//gluLookAt(0,0,5,0,0,0,0,1,0); 
	//drawAxes(10.0);
	glEnable(GL_CULL_FACE);
	parser->display();
	g_center = (parser->maxCoord + parser->minCoord) * 0.5;
	//glTranslatef(-g_center.x, -g_center.y, -g_center.z);
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
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
	parser->setmode(LINE_MODE);

}

void menuCallBack(int key)
{

}

void initMenu()
{
	mainWin = glutCreateWindow("Huang Zhe 's Graphic Assignment");
	//GLUI *glui = GLUI_Master.create_glui_subwindow(mainWin, GLUI_SUBWINDOW_RIGHT);
	//glui->add_button("Open File", -1, menuCallBack);
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	initMenu();
	initGL();
	glutDisplayFunc(&myDisplay);
	glutMainLoop();
	return 0;
}



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