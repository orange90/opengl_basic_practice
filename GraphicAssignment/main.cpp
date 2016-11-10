#include <windows.h>
#using <mscorlib.dll>
#using <System.dll>
#using <System.Windows.Forms.dll>
#include "MfileParser.h"
#include "RenderMode.h"
#include "HalfEdgeEncoder.h"
#include <Gl\glut.h>//should be put at the last

using namespace System::Windows::Forms;
#define TRANSFORM_NONE    0 
#define TRANSFORM_ROTATE  1
#define TRANSFORM_SCALE 2 
#define TRANSFORM_TRANSLATE 3 

#define	MODE_CHOOSE_BOX 1001
#define OPEN_FILE_BUTTON_ID 1002
#define SHOW_NORMAL_CHECK_BOX_ID 1003
#define METERIAL_TYPE_LIST_BOX 1004
#define CMD_CLOSE_ID 1005
#define CMD_RESET_CAMARA 1006
#define DIRECTION_RADIO_BOX 1007
#define PROJECTION_BOX 1008


#define MATERIAL_DIFFUSE 101
#define MATERIAL_SPECULAR 102

GLUI_Listbox *modeList;
GLUI_Rotation *glRotateBall;
GLUI_Checkbox *showVertexNormalBox;
GLUI_Listbox *materialList;
GLUI_Listbox *shadingList;
GLUI_RadioGroup *directiongroup;
GLUI_Listbox *projectionBox;

MFileParser *mParser;
HalfEdgeEncoder *HEEncoder;
int mainWin;

float defaultEyePos[3] = { 15, 15, 15 };
double sphi = 90.0, stheta = 45.0, sdepth = 10;	// for simple trackball
double xpan = 0.0, ypan = 0.0;				// for simple trackball
double zNear = 1.0, zFar = 100.0;
double g_fov = 45.0;
Vector3Point g_center;
bool drawNormal = false;

static int press_x, press_y;
static float x_angle = 0.0;
static float y_angle = 0.0;
static float scale_size = 1;
static float x_translate = 0;
static float y_translate = 0;
static float z_translate = 0;


static int xform_mode = 0;
int initRenderMode = LINE_MODE;

int cameraDirection;

void drawGround();
void drawAxis(float rate);
void draw_z_axis(float rate);
void draw_y_axis(float rate);
void draw_x_axis(float rate);
void mymotion(int x, int y);
void mymouse(int button, int state, int x, int y);
void resetCamera();
void chooseDirection();
void selectProjection();


void myDisplay(void)
{
	glEnable(GL_DEPTH_TEST);
	// Just clean the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// setup the perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60, 1, .1, 100);
	selectProjection();
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	chooseDirection();
	//gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
	//glTranslatef(xpan, ypan, -sdepth);
	//glRotatef(-stheta, 1.0, 0.0, 0.0);
	//glRotatef(sphi, 0.0, 1.0, 0.0);
    //glTranslatef(-g_center.x, -g_center.y, -g_center.z);
	//gluLookAt(0, 0, 10, 0, 0, 0, 0, 0, 0);
	//gluLookAt(0,0,5,0,0,0,0,1,0); 
	//drawAxes(10.0);
	glEnable(GL_CULL_FACE);
	glScalef(scale_size, scale_size, scale_size);
	glRotatef(x_angle, 0, 1, 0);
	glRotatef(y_angle, 1, 0, 0);
	glTranslatef(x_translate, y_translate, z_translate);
	//mParser->display();
	HEEncoder->render();
	mParser->setBoundingBox();
	glPushMatrix();
		glTranslated(mParser->minCoord.x, mParser->minCoord.y, mParser->minCoord.z);
		drawAxis(0.2);
	glPopMatrix();
	
	drawGround();
	drawAxis(1);
	
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
	
}

void resetCamera()
{
	gluLookAt(defaultEyePos[0], defaultEyePos[1], defaultEyePos[2], g_center.x, g_center.y, g_center.z, 0, 1, 0);
	x_angle = 0.0;
	y_angle = 0.0;
	x_translate = y_translate = z_translate = 0;
	scale_size = 1;
	press_x = press_y = 0;
}

void initGL(void)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonOffset(1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);

	GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0 }; // light position
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 }; // light color
	GLfloat lmodel_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	gluOrtho2D(-20,20, -20, 20);
	mParser = new MFileParser();

	mParser->parserFile("meshes/cap.m");
	HEEncoder = new HalfEdgeEncoder();
	HEEncoder->setmode(initRenderMode);
	/////////
	HEEncoder->encode(mParser->m_vertices, mParser->m_faces);
	float ratio = HEEncoder->scaleModelIntoUnitSize(mParser->minCoord, mParser->maxCoord);
	mParser->minCoord = mParser->minCoord * ratio;
	mParser->maxCoord = mParser->maxCoord * ratio;
	g_center = (mParser->maxCoord + mParser->minCoord) * 0.5;
}

void menuCallBack(int key)
{
	if (key == MODE_CHOOSE_BOX)
	{
		mParser->setmode(modeList->get_int_val());
		HEEncoder->setmode(modeList->get_int_val());
		glFlush();
		glutSwapBuffers();
		glutPostRedisplay();
	}
	if (key == OPEN_FILE_BUTTON_ID)
	{
		OpenFileDialog ^ openFileDialog1 = gcnew OpenFileDialog;
		openFileDialog1->InitialDirectory = System::Environment::CurrentDirectory;
		openFileDialog1->Filter = "model files (*.m)|*.m";
		openFileDialog1->FilterIndex = 2;
		openFileDialog1->RestoreDirectory = true;

 		if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			char* path = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(openFileDialog1->FileName);
			printf(path);
			mParser->parserFile(path);
			HEEncoder->encode(mParser->m_vertices, mParser->m_faces);
			float ratio = HEEncoder->scaleModelIntoUnitSize(mParser->minCoord, mParser->maxCoord);
			mParser->minCoord = mParser->minCoord * ratio;
			mParser->maxCoord = mParser->maxCoord * ratio;
			System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)path);
			g_center = (mParser->maxCoord + mParser->minCoord) * 0.5;
		}
	}
	if (key == SHOW_NORMAL_CHECK_BOX_ID)
	{
		if (showVertexNormalBox->get_int_val() ==0)
		{
			HEEncoder->showVertexNormal = false;
		}
		else
		{
			HEEncoder->showVertexNormal = true;
		}
	}
	if (key == METERIAL_TYPE_LIST_BOX)
	{
		if (materialList->get_int_val() == MATERIAL_DIFFUSE)
		{
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
		}
		else if (materialList->get_int_val() == MATERIAL_SPECULAR)
		{
			glColorMaterial(GL_FRONT, GL_SPECULAR);
		}
	}
	if (key == CMD_RESET_CAMARA)
	{
		resetCamera();
	}
	if (key == CMD_CLOSE_ID)
	{
		exit(0);
	}
	if (key == DIRECTION_RADIO_BOX)
	{
		cameraDirection = directiongroup->get_int_val();
		resetCamera();
	}
	if (key == PROJECTION_BOX)
	{
		
	}
}

void selectProjection()
{
	int value = projectionBox->get_int_val();
	if (value == 0)
	{
		gluPerspective(60, 1, .1, 100);
	}
	else if (value == 1)
	{
		glOrtho(-10, 10, -10, 10, .01, 300);
	}
}

void chooseDirection()
{
	switch (cameraDirection)
	{
	case 0:
		gluLookAt(defaultEyePos[0], defaultEyePos[1], defaultEyePos[2], g_center.x, g_center.y, g_center.z, 0, 1, 0);
		break;
	case 1:
		gluLookAt(g_center.x, g_center.y, defaultEyePos[2] + g_center.z, g_center.x, g_center.y, g_center.z, 0, 1, 0);
		break;
	case 2:
		gluLookAt(g_center.x, defaultEyePos[1] + g_center.y, g_center.z, g_center.x, g_center.y, g_center.z, 0, 0, 1);
		break;
	case 3:
		gluLookAt(defaultEyePos[0] + g_center.x, g_center.y, g_center.z, g_center.x, g_center.y, g_center.z, 0, 1, 0);
		break;
	default:
		break;
	}
}

void myGlutIdle(void) {
	if (glutGetWindow() != mainWin)
		glutSetWindow(mainWin);
	glutPostRedisplay();
}

void myGlutReshape(int width, int height)
{
	float xy_aspect = (float)width / (float)height;
	glViewport(0, 0, width, width);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-xy_aspect*.08, xy_aspect*.08, -.08, .08, .1, 15.0);

	glutPostRedisplay();
}

void initMenu()
{
	glutInitWindowSize(800, 800);
	mainWin = glutCreateWindow("Huang Zhe 's Graphic Assignment");
	GLUI *glui = GLUI_Master.create_glui("fff",0,800,0);// (mainWin, GLUI_SUBWINDOW_RIGHT);
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	GLUI_Master.set_glutReshapeFunc(myGlutReshape);
	GLUI_Master.auto_set_viewport();
	GLUI_Panel *panel = glui->add_panel("Basic Panel");
	GLUI_Panel *controlPanel = glui->add_panel("Control");
	
	GLUI_Panel *directionRadioPanel = glui->add_panel_to_panel(controlPanel, "Camera Direction");
	glui->add_separator_to_panel(controlPanel);
	
	//-----------------------------------------------------
	//file panel
	GLUI_Button *openFileButton = glui->add_button_to_panel(panel, "Open File", OPEN_FILE_BUTTON_ID, menuCallBack);
	new GLUI_Button(panel, "Close", CMD_CLOSE_ID, menuCallBack);

	//-----------------------------------------------------
	//control panel
	showVertexNormalBox = glui->add_checkbox_to_panel(controlPanel,"Show Vertex Normal", 0, SHOW_NORMAL_CHECK_BOX_ID, menuCallBack);
	modeList = glui->add_listbox_to_panel(controlPanel, "Render Mode:", &initRenderMode, MODE_CHOOSE_BOX, menuCallBack);
	modeList->add_item(POINT_MODE, "POINT_MODE");
	modeList->add_item(LINE_MODE,"LINE_MODE"); 
	modeList->add_item(FLAT_SHADING,"FLAT_SHADING");
	modeList->add_item(SMOOTH_SHADING, "SMOOTH_SHADING");
	modeList->set_int_val(initRenderMode);
	materialList = glui->add_listbox_to_panel(controlPanel,"Shading:", 0, METERIAL_TYPE_LIST_BOX, menuCallBack);
	materialList->add_item(MATERIAL_DIFFUSE, "MATERIAL_DIFFUSE");
	materialList->add_item(MATERIAL_SPECULAR, "MATERIAL_SPECULAR");
	
	projectionBox = glui->add_listbox_to_panel(controlPanel, "Projection:", 0, PROJECTION_BOX, menuCallBack);
	projectionBox->add_item(0, "Perspective");
	projectionBox->add_item(1, "Orthogonal");
	glui->add_separator_to_panel(controlPanel);
	glui->add_button_to_panel(controlPanel,"Reset Camera", CMD_RESET_CAMARA, menuCallBack);
	//glRotateBall = glui->add_rotation("Rotate\nOr use mouse left button", 0, -1, menuCallBack);
	//-----------------------------------------------------
	//radio panel
	directiongroup = glui->add_radiogroup_to_panel(directionRadioPanel, 0, DIRECTION_RADIO_BOX, menuCallBack);
	glui->add_radiobutton_to_group(directiongroup, "3D");
	glui->add_radiobutton_to_group(directiongroup, "XY");
	glui->add_radiobutton_to_group(directiongroup, "ZX");
	glui->add_radiobutton_to_group(directiongroup, "YZ");
}

[System::STAThreadAttribute]
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
		else if (button == GLUT_RIGHT_BUTTON)
			xform_mode = TRANSFORM_SCALE;
		else if (button == GLUT_MIDDLE_BUTTON)
			xform_mode = TRANSFORM_TRANSLATE;
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
	else if (xform_mode == TRANSFORM_TRANSLATE)
	{
		x_translate += (x - press_x)/5.0;
		y_translate += -(y - press_y)/5.0;
		press_x = x;
		press_y = y;
	}

	// force the redraw function
	glutPostRedisplay();
}

void MyDrawLineFunc(int a, int b, int c, int d)
{
	glBegin(GL_LINES);
	glVertex3i(a, 0, b);
	glVertex3i(c, 0, d);
	glEnd();
}

void drawGround()
{
	glDisable(GL_LIGHTING);
	glLineWidth(1);
	glColor3f(0.8, 0.8, 0.8);
	for (int i = -100; i <= 100; ++i) 
	{
		MyDrawLineFunc(i, -100, i, 100);
		MyDrawLineFunc(-100, i, 100, i);
	}
	glLineWidth(2);
	glColor3f(0, 0, 0);
	MyDrawLineFunc(0, -100, 0, 100);
	MyDrawLineFunc(-100, 0, 100, 0);
	glLineWidth(1);
	glEnable(GL_LIGHTING);
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
	glDisable(GL_LIGHTING);
	glColor3f(0, 0, 1);
	draw_z_axis(rate);
	glColor3f(0, 1, 0);
	draw_y_axis(rate);
	glColor3f(1, 0, 0);
	draw_x_axis(rate);
	glEnable(GL_LIGHTING);
}


