
/************************************************************************************

File: 			FlightSim.c

Description:	A complete OpenGL program that draws a flight simulation

Author:			Surya Kashyap

*************************************************************************************/

//include library headers
#include <freeglut.h>
#include <stdio.h>
#include <string.h>

#pragma warning(disable:4996)

// window dimensions
GLint windowHeight = 600;
GLint windowWidth = 600;

// camera position
GLfloat cameraPosition[] = { 0.0, 0.0, 2.5 };

// difference added at each frame
GLfloat interpDiff = 0.0003;

// determine direction to move the camera in
GLint moveUp, moveDown, moveRight, moveLeft, moveForward, moveBackward, increaseSpeed, decreaseSpeed = 0;

// size of the grid of quads
const GLint gridSize = 100;

// check whether to toggle fullscreen
GLint goFullScreen = 0;

// toggle polygon mode
GLint showWireFrame = 0;

// plane vertices, normals and faces
GLfloat planeVertices[6763][3]; planeNormals[6763][3];
GLint planeFaces[][3];

// number of vertices, normals and faces
GLint numCVertices, numCNormals, numCFaces;

typedef struct SubObjects
{
	GLfloat vertices[3639][6];
} SubObject;

struct SubObject* subobjects[33];

// file stream to access directory 
FILE *fileStream;
char fileText[100];

int arr[3639][6];

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

	// open the file
	fileStream = fopen("cessna.txt", "r");

	GLfloat x, y, z;
	int xFace, yFace, zFace;
	int i = 0, j = 0, k = 0, l = 0;

	while (fgets(fileText, sizeof fileText, fileStream))
	{
		if (fileText[0] == 'v')
		{
			if (sscanf(fileText, "v %f %f %f", &x, &y, &z) != 3) { break; }
			planeVertices[i][0] = x;
			planeVertices[i][1] = y;
			planeVertices[i][2] = z;
			i++;
		}
		else if (fileText[0] == 'n')
		{
			if (sscanf(fileText, "n %f %f %f", &x, &y, &z) != 3) { break; }
			planeNormals[j][0] = x;
			planeNormals[j][1] = y;
			planeNormals[j][2] = z;
			j++;
		}
		else if (fileText[0] == 'g')
		{
			k++;
		}
		else if (fileText[0] == 'f')
		{
			//char delim[] = " ";
			//char *ptr = strtok(fileText, delim);
			//SubObject object;
			//int value;
			//int i = 0;
			//while (ptr != NULL)
			//{
			//	//printf("'%s'\n", ptr);
			//	if (ptr[0] != 'f')
			//	{
			//		sscanf(ptr, "%04d", &value);
			//		printf("%d %d\n", l, i);
			//		arr[l][i] = value;
			//		object.vertices[l][i] = value;
			//		i++;
			//	}
			//	ptr = strtok(NULL, delim);
			//}
		}
	}


	numCVertices = i;
	numCNormals = j;
	printf("%d", k);
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
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

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


	glPushMatrix();
	glTranslatef(-0.5,  -0.5, -1.5);
	glBegin(GL_QUADS);
	for (float i = 0; i < gridSize; i += 0.5)
	{
		float t = i;
		for (float j = 0; j < gridSize; j += 0.5)
		{
			float s = j;
			glVertex3f(t, 0,s);
			glVertex3f(t + 0.5, 0, s);
			glVertex3f(t + 0.5, 0, s + 0.5);
			glVertex3f(t,0, s + 0.5);
		}
	}
	glEnd();
	glPopMatrix();

	glTranslatef(-cameraPosition[0], -cameraPosition[1], -cameraPosition[2]);

	glutSwapBuffers();

}

/************************************************************************


Function:		specialKeys


Description:	Handles special key press functionality


*************************************************************************/
void specialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		moveRight = 1;
		break;
	case GLUT_KEY_LEFT:
		moveLeft = 1;
		break;
	case GLUT_KEY_UP:
		moveUp = 1;
		break;
	case GLUT_KEY_DOWN:
		moveDown = 1;
		break;
	case GLUT_KEY_PAGE_UP:
		increaseSpeed = 1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		decreaseSpeed = 1;
		break;
	}

}

/************************************************************************


Function:		determineMovement


Description:	 Determines movement based on key presses


*************************************************************************/
void determineMovement()
{
	cameraPosition[2] -= interpDiff;
	// map camera movement to keys
	if (moveRight)
	{
		cameraPosition[0] += interpDiff;
	}
	if (moveLeft)
	{
		cameraPosition[0] -= interpDiff;
	}
	if (moveUp)
	{
		cameraPosition[1] += interpDiff;
	}
	if (moveDown)
	{
		cameraPosition[1] -= interpDiff;
	}
	//if (moveForward)
	//{
	//	cameraPosition[2] -= interpDiff;
	//}
	//if (moveBackward)
	//{
	//	cameraPosition[2] += interpDiff;
	//}
	if (increaseSpeed)
	{
		interpDiff += 0.0002;
		increaseSpeed = 0;
	}
	if (decreaseSpeed)
	{
		interpDiff -= 0.0002;
		if (interpDiff < 0)
		{
			interpDiff = 0;
		}
	}
}

/************************************************************************


Function:		myKeyUp


Description:	Handles key release functionality


*************************************************************************/
void myKeyUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		moveRight = 0;
		break;
	case GLUT_KEY_LEFT:
		moveLeft = 0;
		break;
	case GLUT_KEY_UP:
		moveUp = 0;
		break;
	case GLUT_KEY_DOWN:
		moveDown = 0;
		break;
	case GLUT_KEY_PAGE_UP:
		increaseSpeed = 0;
		break;
	case GLUT_KEY_PAGE_DOWN:
		decreaseSpeed = 0;
		break;
	}
}

/************************************************************************

Function:		myIdle

Description:	Handles idle functions


*************************************************************************/
void myIdle()
{
	if (goFullScreen)
	{
		glutFullScreen();
	}
	else
	{
		glutLeaveFullScreen();
	}
	determineMovement();

	glutPostRedisplay();
}

/************************************************************************

Function:		reshape

Description:	Handles window reshape


*************************************************************************/
void reshape(int newWidth, int newHeight)
{
	glViewport(0.0, 0.0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)newWidth / (float)newHeight, 0.1, 20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/************************************************************************

Function:		myKey

Description:	Handles key presses


*************************************************************************/
void myKey(unsigned char key, int x, int y)
{
	switch (key)
	{
	// if 'f' is pressed
	case 'f':
		if (goFullScreen)
		{
			goFullScreen = 0;
		}
		else
		{
			goFullScreen = 1;
		}
		break;
	// exit if 'e' is pressed
	case 'q':
		glutExit();
		break;
	case 'w':
		if (showWireFrame)
		{
			showWireFrame = 0;
		}
		else
		{
			showWireFrame = 1;
		}
		break;
	}
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

	// set the idle function
	glutIdleFunc(myIdle);

	// set the reshape function
	glutReshapeFunc(reshape);

	// set the keyboard function
	glutKeyboardFunc(myKey);

	// when key has been released
	glutSpecialUpFunc(myKeyUp);

	// special keys function
	glutSpecialFunc(specialKeys);

	// initialize the rendering context
	initializeGL();

	// go into perpetual loop
	glutMainLoop();
}