
/************************************************************************************

File: 			FlightSim.c

Description:	A complete OpenGL program that draws a flight simulation

Author:			Surya Kashyap

*************************************************************************************/

//include library headers
#include <freeglut.h>

// window dimensions
GLint windowHeight = 600;
GLint windowWidth = 600;

// camera position
GLfloat cameraPosition[] = { 0.0,0.0, 2.5 };

/************************************************************************


Function:		initializeGL


	Description : Initializes the OpenGL rendering context for display.


*************************************************************************/
void initializeGL()
{
	// set the background color
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// set default drawing color
	glColor3f(1.0, 1.0, 1.0);

	// set to projection mode 
	glMatrixMode(GL_PROJECTION);

	// clear any previous transformations
	glLoadIdentity();

	// set the perspective 
	gluPerspective(45, (float)windowWidth / (float)windowHeight, 0.1, 20);

	// set the shade model
	glShadeModel(GL_SMOOTH);

	// enable smooth line drawing
	glEnable(GL_LINE_SMOOTH);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);
}

/************************************************************************


Function:		myDisplay


Description:	The display function


*************************************************************************/
void myDisplay()
{
	// clear the screen 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// set to projection mode 
	//glMatrixMode(GL_MODELVIEW);

	//glLoadIdentity();

	// set the camera position
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
		cameraPosition[0], cameraPosition[1], cameraPosition[2] - 100,
		0, 1, 0);

	// initialize quad
	GLUquadric *quad;
	quad = gluNewQuadric();
	
	// set the line width
	glLineWidth(5.0);
	glBegin(GL_LINES);
	// X
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.3, 0.0, 0.0);
	// Y
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.3, 0.0);
	// Z
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(-0.2, 0.0, 1.0);
	glEnd();

	// draw the circle at the center
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.0, 0.0, 0.0);
	gluSphere(quad, 0.02, 25, 100);

	glTranslatef(-cameraPosition[0], -cameraPosition[1], -cameraPosition[2]);

	glutSwapBuffers();

}

/************************************************************************

Function:		main

Description:	Sets up the openGL rendering context and the windowing

system, then begins the display loop.


*************************************************************************/
void main(int argc, char** argv)
{
	//initialize toolkit
	glutInit(&argc, argv);

	// set display mode
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// set window size
	glutInitWindowSize(windowWidth, windowHeight);

	// open screen window
	glutCreateWindow("Flight Sim");

	// set the display function
	glutDisplayFunc(myDisplay);

	// initialize the rendering context
	initializeGL();

	// go into perpetual loop
	glutMainLoop();
}