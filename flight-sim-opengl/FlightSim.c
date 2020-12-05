
/************************************************************************************

File: 			FlightSim.c

Description:	A complete OpenGL program that draws a flight simulation

Author:			Surya Kashyap

*************************************************************************************/

//include library headers
#include <freeglut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#pragma warning(disable:4996)

#define TO_RADIANS 3.14/180.0

#define SNOW_NUM 10000

// window dimensions
GLint windowHeight = 600;
GLint windowWidth = 600;

// camera position
//GLfloat cameraPosition[] = { 0.0, 0.0, 0.0};


// difference added at each frame
GLfloat interpDiff = 0.0003;

// speed at which the camera moves
GLfloat speed = 0;

// determine direction to move the camera in
GLint moveUp, moveDown, moveRight, moveLeft, moveForward, moveBackward, increaseSpeed, decreaseSpeed = 0;

// size of the grid of quads
const GLint gridSize = 100;
const GLint gridDim = 1.0;

// check whether to toggle fullscreen
GLint goFullScreen = 0;

// toggle polygon mode
GLint showWireFrame = 0;

// toggle between grid and textures
GLint showGrid, showTextures = 0;

// toggle mountains and their textures
GLint showMount, showMountTextures = 0;

// toggle fog
GLint showFog = 0;

// set fog density and color
GLfloat fogDensity = 0.05;
GLfloat fogColor[4] = { 1.0, 1.0, 1.0, 1.0 };

// plane vertices, normals and faces
GLfloat planeVertices[6763][3]; planeNormals[6763][3];

// number of vertices, normals and faces
GLint numCVertices, numCNormals, numCFaces;

// file stream to access directory 
FILE *fileStream;
char fileText[100];

// array top store values of cessna
int cessnaArray[34][500][20];

int propArray[3][500][30];

// size of each object in propeller
int propSizes[3];

// size of each object in each face of cessna
int pFaceSizes[131];

// size of each object in cessna
int cessnaSizes[34];

// size of each object in propeller
int cFaceSizes[3640];

// angle to rotate camera
float angle = 0.0;

// variable to store last x position
GLfloat lastX;

//  position the light source at the origin 
GLfloat lightPosition[] = { 1.0, 3.0, 0.0, 1.0 };

// a material that is all zeros
//GLfloat zeroMaterial[] = { 0.0, 0.0, 0.0, 1.0 };

GLfloat zeroMaterial[] = { 0.8, 0.8, 0.8, 1.0 };

// a red ambient material
GLfloat redAmbient[] = { 0.0, 0.0, 0.0, 1.0 };

// a blue diffuse material
GLfloat blueDiffuse[] = { 0.1, 0.5, 0.8, 1.0 };

// a red diffuse material
GLfloat redDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };

// a white specular material
GLfloat whiteSpecular[] = { 1.0, 1.0, 1.0, 1.0 };


// the degrees of shinnines (size of the specular highlight, bigger number means smaller highlight)
GLfloat noShininess = 0.0;
GLfloat highShininess = 100.0;

int cl = 0, pl = 0;

// image size
int skyImageWidth, skyImageHeight, landImageWidth, landImageHeight, mountImageWidth, mountImageHeight;

// the image data
GLubyte *skyImageData, *landImageData, *mountImageData;

// ids to swtich between textures
GLuint skyTextureId, landTextureId, mountTextureId;

// keep track of plane skins
GLint firstSkin, secondSkin, thirdSkin = 0;

// toggle weather
GLint sunrise,sunset, night = 0;

// handle rain and snow
GLint startRain, startSnow , startMist = 0;

// keep track of camera positions
GLfloat camX, camZ = 0.0;

// position of snow
GLfloat snowPosX[SNOW_NUM], snowPosY[SNOW_NUM], snowPosZ[SNOW_NUM];

// position of rain
GLfloat rainPosX[SNOW_NUM], rainPosY[SNOW_NUM], rainPosZ[SNOW_NUM];

// sizes of the snow particles
GLfloat snowSize[SNOW_NUM];

// density and color for mist
GLfloat mistDensity = 0.1;
GLfloat mistColor[4] = { 0.3,0.3,0.3,1.0 };

GLfloat theta = 0.0;

// island scales and colors
GLfloat i1Scales[3], i2Scales[3], i3Scales[3];
GLfloat level1Colors[3], level2Colors[3];

// set camera position
GLfloat cameraPosition[3] = { 0,0,0 };

// set camera movement speed
GLfloat cameraSpeed = 0;

// struct for storing the direction vector of the camera
struct directionVector
{
	GLfloat dirVector[3];
};

/************************************************************************


Function:		convertToVector


Description :	Takes in an angle and converts it to a vector.


*************************************************************************/
struct directionVector convertToVector(GLfloat angle)
{
	struct directionVector resVector;
	resVector.dirVector[0] = sin((angle * 3.142) / 180);
	resVector.dirVector[1] = 0;
	resVector.dirVector[2] = -cos((angle * 3.142) / 180);
	return resVector;
}


/************************************************************************


Function:		readCessnaFile


Description : Reads the Cessna.txt file.


*************************************************************************/
void readCessnaFile()
{
	// open the file
	fileStream = fopen("cessna.txt", "r");

	GLfloat x, y, z;
	int xFace, yFace, zFace;
	int i = 0, j = 0, k = 0, l = 0, h = 0;

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
			l = 0;
		}
		else if (fileText[0] == 'f')
		{
			char delim[] = " ";
			char temp[100];
			strcpy(temp, fileText);
			char *ptr = strtok(temp, delim);
			int value;
			h = 0;
			while (ptr != NULL)
			{
				if (ptr[0] != 'f')
				{
					if (sscanf(ptr, "%d", &value) != 1)
					{
						break;
					}
					//printf("%d  ", value);
					cessnaArray[k][l][h] = value;
					h++;
				}
				ptr = strtok(NULL, delim);
			}
			//printf("%d %d ",cl, h);
			cFaceSizes[cl] = h;
			cl++;
			l++;

			cessnaSizes[k] = l;
		}
	}
	numCVertices = i;
	numCNormals = j;
	numCFaces = k;
	fclose(fileStream);

}

/************************************************************************


Function:		readPropellerFile


Description : Reads the propeller.txt file.


*************************************************************************/
void readPropellerFile()
{
	// open the file
	fileStream = fopen("propeller.txt", "r");
	GLfloat x, y, z;
	int k = 0, l = 0, h = 0;

	while (fgets(fileText, sizeof fileText, fileStream))
	{
		if (fileText[0] == 'g')
		{
			k++;
			l = 0;
		}
		else if (fileText[0] == 'f')
		{
			char delim[] = " ";
			char temp[100];
			strcpy(temp, fileText);
			char *ptr = strtok(temp, delim);
			int value;
			h = 0;
			while (ptr != NULL)
			{
				if (ptr[0] != 'f')
				{
					if (sscanf(ptr, "%d", &value) != 1)
					{
						break;
					}
					//printf("%d  ", value);
					propArray[k][l][h] = value;
					h++;
				}
				ptr = strtok(NULL, delim);
			}
			//printf("%d %d ",pl, h);
			pFaceSizes[pl] = h;
			pl++;
			l++;

			propSizes[k] = l;
		}
	}
	fclose(fileStream);
}

/************************************************************************

	Function:		loadImage

	Description:	Loads in the PPM image

*************************************************************************/
void loadSkyImage()
{


	// maxValue
	int  maxValue;

	// total number of pixels in the image
	int  totalPixels;

	// temporary character
	char tempChar;

	// counter variable for the current pixel in the image
	int i;

	// array for reading in header information
	char headerLine[100];

	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	// open the image file for reading
	fileStream = fopen("sky08.ppm", "r");

	// read in the first header line
	//    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
	//    - so we are just reading everything up to the first line break
	fscanf(fileStream, "%[^\n] ", headerLine);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// read in the first character of the next line
	fscanf(fileStream, "%c", &tempChar);

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf(fileStream, "%[^\n] ", headerLine);

		// print the comment
		//printf("%s\n", headerLine);

		// read in the first character of the next line
		fscanf(fileStream, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileStream);

	// read in the image hieght, width and the maximum value
	fscanf(fileStream, "%d %d %d", &skyImageWidth, &skyImageHeight, &maxValue);

	// print out the information about the image file
	//printf("%d rows  %d columns  max value= %d\n", skyImageHeight, skyImageWidth, maxValue);

	// compute the total number of pixels in the image
	totalPixels = skyImageWidth * skyImageHeight;

	// allocate enough memory for the image  (3*) because of the RGB data
	skyImageData = malloc(3 * sizeof(GLuint) * totalPixels);


	// determine the scaling for RGB values
	RGBScaling = 255.0 / maxValue;


	// if the maxValue is 255 then we do not need to scale the 
	//    image data values to be in the range or 0 to 255
	if (maxValue == 255)
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileStream, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			skyImageData[3 * totalPixels - 3 * i - 3] = red;
			skyImageData[3 * totalPixels - 3 * i - 2] = green;
			skyImageData[3 * totalPixels - 3 * i - 1] = blue;
		}
	}
	else  // need to scale up the data values
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileStream, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			skyImageData[3 * totalPixels - 3 * i - 3] = red * RGBScaling;
			skyImageData[3 * totalPixels - 3 * i - 2] = green * RGBScaling;
			skyImageData[3 * totalPixels - 3 * i - 1] = blue * RGBScaling;
		}
	}


	// close the image file
	fclose(fileStream);
}

/************************************************************************

	Function:		loadImage

	Description:	Loads in the PPM image

*************************************************************************/
void loadILandImage()
{
	// maxValue
	int  maxValue;

	// total number of pixels in the image
	int  totalPixels;

	// temporary character
	char tempChar;

	// counter variable for the current pixel in the image
	int i;

	// array for reading in header information
	char headerLine[100];

	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	// open the image file for reading
	fileStream = fopen("sea02.ppm", "r");

	// read in the first header line
	//    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
	//    - so we are just reading everything up to the first line break
	fscanf(fileStream, "%[^\n] ", headerLine);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// read in the first character of the next line
	fscanf(fileStream, "%c", &tempChar);

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf(fileStream, "%[^\n] ", headerLine);

		// print the comment
		//printf("%s\n", headerLine);

		// read in the first character of the next line
		fscanf(fileStream, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileStream);

	// read in the image hieght, width and the maximum value
	fscanf(fileStream, "%d %d %d", &landImageWidth, &landImageHeight, &maxValue);

	// print out the information about the image file
	//printf("%d rows  %d columns  max value= %d\n", landImageHeight, landImageWidth, maxValue);

	// compute the total number of pixels in the image
	totalPixels = landImageWidth * landImageHeight;

	// allocate enough memory for the image  (3*) because of the RGB data
	landImageData = malloc(3 * sizeof(GLuint) * totalPixels);


	// determine the scaling for RGB values
	RGBScaling = 255.0 / maxValue;


	// if the maxValue is 255 then we do not need to scale the 
	//    image data values to be in the range or 0 to 255
	if (maxValue == 255)
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileStream, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			landImageData[3 * totalPixels - 3 * i - 3] = red;
			landImageData[3 * totalPixels - 3 * i - 2] = green;
			landImageData[3 * totalPixels - 3 * i - 1] = blue;
		}
	}
	else  // need to scale up the data values
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileStream, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			landImageData[3 * totalPixels - 3 * i - 3] = red * RGBScaling;
			landImageData[3 * totalPixels - 3 * i - 2] = green * RGBScaling;
			landImageData[3 * totalPixels - 3 * i - 1] = blue * RGBScaling;
		}
	}


	// close the image file
	fclose(fileStream);
}

/************************************************************************

	Function:		loadMountImage

	Description:	Loads in the PPM image

*************************************************************************/
void loadMountImage()
{
	// maxValue
	int  maxValue;

	// total number of pixels in the image
	int  totalPixels;

	// temporary character
	char tempChar;

	// counter variable for the current pixel in the image
	int i;

	// array for reading in header information
	char headerLine[100];

	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	// open the image file for reading
	fileStream = fopen("mount03.ppm", "r");

	// read in the first header line
	//    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
	//    - so we are just reading everything up to the first line break
	fscanf(fileStream, "%[^\n] ", headerLine);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// read in the first character of the next line
	fscanf(fileStream, "%c", &tempChar);

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf(fileStream, "%[^\n] ", headerLine);

		// print the comment
		//printf("%s\n", headerLine);

		// read in the first character of the next line
		fscanf(fileStream, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileStream);

	// read in the image hieght, width and the maximum value
	fscanf(fileStream, "%d %d %d", &mountImageWidth, &mountImageHeight, &maxValue);

	// print out the information about the image file
	//printf("%d rows  %d columns  max value= %d\n", landImageHeight, landImageWidth, maxValue);

	// compute the total number of pixels in the image
	totalPixels = mountImageWidth * mountImageHeight;

	// allocate enough memory for the image  (3*) because of the RGB data
	mountImageData = malloc(3 * sizeof(GLuint) * totalPixels);


	// determine the scaling for RGB values
	RGBScaling = 255.0 / maxValue;


	// if the maxValue is 255 then we do not need to scale the 
	//    image data values to be in the range or 0 to 255
	if (maxValue == 255)
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileStream, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			mountImageData[3 * totalPixels - 3 * i - 3] = red;
			mountImageData[3 * totalPixels - 3 * i - 2] = green;
			mountImageData[3 * totalPixels - 3 * i - 1] = blue;
		}
	}
	else  // need to scale up the data values
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileStream, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			mountImageData[3 * totalPixels - 3 * i - 3] = red * RGBScaling;
			mountImageData[3 * totalPixels - 3 * i - 2] = green * RGBScaling;
			mountImageData[3 * totalPixels - 3 * i - 1] = blue * RGBScaling;
		}
	}


	// close the image file
	fclose(fileStream);
}
/************************************************************************

	Function:		createLandTexture

	Description:	Creates texture for the land

*************************************************************************/
void createLandTexture()
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &landTextureId);
	glBindTexture(GL_TEXTURE_2D, landTextureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, landImageWidth, landImageHeight, GL_RGB, GL_UNSIGNED_BYTE, landImageData);
}

/************************************************************************

	Function:		createSkyTexture

	Description:	Creates texture for the sky

*************************************************************************/
void createSkyTexture()
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &skyTextureId);
	glBindTexture(GL_TEXTURE_2D, skyTextureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, skyImageWidth, skyImageHeight, GL_RGB, GL_UNSIGNED_BYTE, skyImageData);
}

/************************************************************************

	Function:		createMountTexture

	Description:	Creates texture for the islands

*************************************************************************/
void createMountTexture()
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &mountTextureId);
	glBindTexture(GL_TEXTURE_2D, mountTextureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, mountImageWidth, mountImageHeight, GL_RGB, GL_UNSIGNED_BYTE, mountImageData);
}

/************************************************************************


Function:		getRandomFloat


Description:	Generates a random float between min and max


*************************************************************************/
float getRandomFloat(float min, float max)
{
	float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
	return min + scale * (max - min);      /* [min, max] */
}

/************************************************************************


Function:		getRandomNumber


Description:	Generates a random number from min to max


*************************************************************************/
int getRandomNumber(int min, int max)
{
	int num = (rand() % (max - min + 1) + min);
	return num;
}

/************************************************************************


Function:		initializeGL


	Description : Initializes the OpenGL rendering context for display.


*************************************************************************/
void initializeGL()
{
	srand(time(0));

	// define the light color and intensity
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };  // relying on global ambient
	GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };


	//  the global ambient light level
	GLfloat globalAmbientLight[] = { 0.2, 0.2, 0.2, 1.0 };

	// set the global ambient light level
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	// define the color and intensity for light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, specularLight);
	// enable lighting 
	glEnable(GL_LIGHTING);
	// enable light 0
	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_NORMALIZE);
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

	// read texture and data files
	readCessnaFile();

	readPropellerFile();

	loadSkyImage();

	loadILandImage();

	loadMountImage();

	// initiliaze island scales and colors
	i1Scales[0] = getRandomFloat(0.9, 2.0); i1Scales[1] = getRandomFloat(0.3, 0.8); i1Scales[3] = getRandomFloat(1.0, 3.0);
	i2Scales[0] = getRandomFloat(0.9, 2.0); i2Scales[1] = getRandomFloat(0.3, 0.8); i2Scales[3] = getRandomFloat(0.6, 1.6);
	i3Scales[0] = getRandomFloat(0.9, 2.0); i3Scales[1] = getRandomFloat(0.3, 0.8); i3Scales[3] = getRandomFloat(0.6, 1.6);

	level1Colors[0] = 0; level1Colors[1] = getRandomFloat(0.7, 1.0); level1Colors[2] = 0;
	level2Colors[0] = getRandomFloat(0.7, 1.0); level2Colors[1] = getRandomFloat(0.7, 1.0); level2Colors[2] = getRandomFloat(0.7, 1.0);

	// create textures
	createLandTexture();
	createSkyTexture();
	createMountTexture();

	// set defaults
	showTextures = 1;
	firstSkin = 1;
	sunrise = 1;

	// initialize snow and rain positions
	for (int i = 0; i < SNOW_NUM; i++)
	{
		snowPosX[i] = getRandomFloat(-10.0, 10.0);
		snowPosY[i] = getRandomFloat(2.0, 20.3);
		snowPosZ[i] = getRandomFloat(7.0,-7.0);
		snowSize[i] = getRandomFloat(3.0, 5.0);
		rainPosX[i] = getRandomFloat(-10.0, 10.0);
		rainPosY[i] = getRandomFloat(2.0, 20.3);
		rainPosZ[i] = getRandomFloat(7.0, -7.0);
	}

	cameraSpeed = 0.001;

}

/************************************************************************


Function:		drawReferencePoint


Description:	Draws point of reference


*************************************************************************/
void drawReferencePoint(GLUquadric *quad)
{
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
}

/************************************************************************


Function:		drawGrid


Description:	Draws a grid of quads


*************************************************************************/
void drawGrid()
{
	glEnable(GL_LIGHTING);

	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, highShininess);
	glNormal3d(0, 1, 0);  // normal of the floor is pointing up

	glPushMatrix();
	glColor3f(0, 0, 0.3);
	glTranslatef(-4.0, -1.5, -5.5);
	glBegin(GL_QUADS);
	for (float i = 0; i < gridSize; i += gridDim)
	{
		float t = i;
		for (float j = 0; j < gridSize; j += gridDim)
		{
			float s = j;

			glVertex3f(t, 0, s);
			glVertex3f(t + gridDim, 0, s);
			glVertex3f(t + gridDim, 0, s + gridDim);
			glVertex3f(t, 0, s + gridDim);
		}
	}
	glEnd();
	glPopMatrix();
}

/************************************************************************


Function:		drawPropeller


Description : Draws the plane from the propeller.txt file


*************************************************************************/
void drawPropeller()
{
	glPushMatrix();
	int count = 0;
	for (int i = 1; i < 3; i++)
	{
		for (int j = 0; j < propSizes[i]; j++)
		{
			glBegin(GL_POLYGON);
			for (int k = 0; k < pFaceSizes[count]; k++)
			{
				if (i == 1) glColor3f(1.0, 1.0, 0.0);
				if (i == 2) glColor3f(1.0, 0.0, 0.0);
				int row = propArray[i][j][k];
				glNormal3f(planeNormals[row - 1][0], planeNormals[row - 1][1], planeNormals[row - 1][2]);
				glVertex3f(planeVertices[row - 1][0], planeVertices[row - 1][1], planeVertices[row - 1][2]);

			}
			glEnd();
			count++;
		}
	}
	glPopMatrix();
}

void drawIsland(GLUquadric* quad)
{
	glPushMatrix();
	if(showMountTextures)
	glEnable(GL_TEXTURE_2D);

	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, mountTextureId);
	// pyramid
	glBegin(GL_TRIANGLES);
	if(!showMountTextures)
	glColor3f(level2Colors[0], level2Colors[1], level2Colors[2]);
	glTexCoord2f(0, 0);
	glVertex3f(0.0, 1.0, 0.0);
	if (!showMountTextures)
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, 1.0);

	glTexCoord2f(1, 0);
	if (!showMountTextures)
	glColor3f(level2Colors[0], level2Colors[1], level2Colors[2]);
	glVertex3f(0.0, 1.0, 0.0);
	if (!showMountTextures)
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(-1.0, -1.0, 1.0);
	if (!showMountTextures)
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(0.0, -1.0, -1.0);

	glTexCoord2f(1, 1);
	if (!showMountTextures)
	glColor3f(level2Colors[0], level2Colors[1], level2Colors[2]);
	glVertex3f(0.0, 1.0, 0.0);
	if (!showMountTextures)
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(0.0, -1.0, -1.0);
	if (!showMountTextures)
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(1.0, -1.0, 1.0);

	glTexCoord2f(0, 1);
	if (!showMountTextures)
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(-1.0, -1.0, 1.0);
	if (!showMountTextures)
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(0.0, -1.0, -1.0);
	if (!showMountTextures)
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(1.0, -1.0, 1.0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(level1Colors[0], level1Colors[1], level1Colors[2]);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(0.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, 1.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

/************************************************************************


Function:		drawPlane


Description : Draws the plane from the cessna.txt file


*************************************************************************/
void drawPlane()
{
	glDisable(GL_LIGHTING);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blueDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, highShininess);
	glPushMatrix();


	glRotatef(-angle, 0, 1, 0);
	glTranslatef(cameraPosition[0], cameraPosition[1], -2.5 + cameraPosition[2]);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	int count = 0;
	for (int i = 1; i < 34; i++)
	{
		for (int j = 0; j < cessnaSizes[i]; j++)
		{
			glBegin(GL_POLYGON);
			for (int k = 0; k < cFaceSizes[count]; k++)
			{
				int row = cessnaArray[i][j][k];
				if (firstSkin)
				{
					if (i > 0 && i < 5) glColor3f(1.0, 1.0, 0.0);
					if (i > 4 && i < 7) glColor3f(0.0, 0.0, 0.0);
					if (i == 7) glColor3f(1.0, 0.0, 1.0);
					if (i == 8) glColor3f(0.0, 0.0, 1.0);
					if (i > 8 && i < 15) glColor3f(1.0, 1.0, 0.0);
					if (i > 14 && i < 27) glColor3f(0.0, 0.0, 1.0);
					if (i > 26 && i < 34) glColor3f(1.0, 1.0, 0.0);
				}
				else if (secondSkin)
				{
					if (i > 0 && i < 5) glColor3f(1.0, 0.5, 0.0);
					if (i > 4 && i < 7) glColor3f(0.0, 0.0, 0.0);
					if (i == 7) glColor3f(0.0, 0.1, 0.0);
					if (i == 8) glColor3f(0.0, 0.5, 1.0);
					if (i > 8 && i < 15) glColor3f(1.0, 0.5, 0.0);
					if (i > 14 && i < 27) glColor3f(0.0, 0.5, 1.0);
					if (i > 26 && i < 34) glColor3f(1.0, 0.5, 0.0);
				}
				else if (thirdSkin)
				{
					if (i > 0 && i < 5) glColor3f(0.5, 1.0, 1.0);
					if (i > 4 && i < 7) glColor3f(0.0, 0.0, 0.0);
					if (i == 7) glColor3f(0.5, 1.0, 1.0);
					if (i == 8) glColor3f(1.0, 0.0, 0.0);
					if (i > 8 && i < 15) glColor3f(0.5, 1.0, 1.0);
					if (i > 14 && i < 27) glColor3f(1.0, 0.0, 0.0);
					if (i > 26 && i < 34) glColor3f(0.5, 1.0, 1.0);
				}
				glNormal3f(planeNormals[row - 1][0], planeNormals[row - 1][1], planeNormals[row - 1][2]);
				glVertex3f(planeVertices[row - 1][0], planeVertices[row - 1][1], planeVertices[row - 1][2]);

			}
			glEnd();
			count++;
		}
	}
	drawPropeller();
	glTranslatef(0, 0, -0.7);
	drawPropeller();
	glPopMatrix();
}

/************************************************************************


Function:		drawSeaAndSky


Description : Draws the sea and sky with textures


*************************************************************************/
void drawSeaAndSky(GLUquadric *quad)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_EMISSION, noShininess);

	glEnable(GL_TEXTURE_2D);

	// toggle fog on or off
	if (showFog)
	{
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, fogDensity);
	}
	
	if (sunrise)
	{
		glColor3f(1.0, 1.0, 1.0);
	}
	else if (sunset)
	{
		glColor3f(1.0, 0.0, 0.5);
	}
	else if (night)
	{
		glColor3f(0.0, 0.0, 0.5);
	}

	// draw the disk
	glBindTexture(GL_TEXTURE_2D, landTextureId);
	glPushMatrix();
	glTranslatef(0.0, -1.0, 0.0);
	glRotatef(90, 1.0, 0.0, 0.0);
	gluDisk(quad, 0, 7.0, 100, 25);
	glPopMatrix();

	glDisable(GL_FOG);
	
	if (sunrise)
	{
		glColor3f(1.0, 1.0, 1.0);
	}
	else if (sunset)
	{
		glColor3f(1.0, 0.0, 1.0);
	}
	else if (night)
	{
		glColor3f(0.1, 0.5, 1.0);
	}

	if (startMist)
	{
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR, mistColor);
		glFogf(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, mistDensity);
	}

	// draw the cylinder
	glBindTexture(GL_TEXTURE_2D, skyTextureId);
	glPushMatrix();
	glTranslatef(0.0, -1.1, 0.0);
	glRotatef(-90.0, 1, 0, 0);
	gluCylinder(quad, 6.0, 5.5, 7, 100, 25);
	glPopMatrix();
	glDisable(GL_FOG);

	glDisable(GL_TEXTURE_2D);
}



/************************************************************************


Function:		startSnowing


Description:	Starts the snow


*************************************************************************/
void startSnowing()
{
	for (int i = 0; i < SNOW_NUM; i++)
	{
		glColor3f(1.0, 1.0, 1.0);
		glPointSize(snowSize[i]);
		glBegin(GL_POINTS);
		glVertex3f(snowPosX[i], snowPosY[i], snowPosZ[i]);
		glEnd();
	}
}

/************************************************************************


Function:		startRaining


Description:	Starts the rain


*************************************************************************/
void startRaining()
{
	glEnable(GL_BLEND);

	//set the blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < SNOW_NUM; i++)
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(rainPosX[i], rainPosY[i], rainPosZ[i]);
		glVertex3f(rainPosX[i], rainPosY[i]+0.1, rainPosZ[i]);
		glEnd();
	}
	glDisable(GL_BLEND);
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

	struct directionVector dirVector;
	dirVector = convertToVector(angle);
	GLfloat xLookAt = cameraPosition[0] + dirVector.dirVector[0];
	GLfloat yLookAt = cameraPosition[1] + dirVector.dirVector[1];
	GLfloat zLookAt = cameraPosition[2] + dirVector.dirVector[2];

	//printf("Position : %f %f %f \n", cameraPosition[0], cameraPosition[1], cameraPosition[2]);
	//printf("Look at : %f %f %f \n", xLookAt, yLookAt, zLookAt);
	//printf("Direction: %f %f %f \n", dirVector.dirVector[0], dirVector.dirVector[1], dirVector.dirVector[2]);
	//printf("Angle : %f \n", angle);


	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
		xLookAt , yLookAt, zLookAt,
		0, 1, 0);


	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	if (startSnow)
	{
		startSnowing();
	}

	if (startRain)
	{
		startRaining();
	}

	if (showWireFrame)
	{
		glLineWidth(1.0);
		glDisable(GL_TEXTURE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}



	// initialize quad
	GLUquadric *quad;
	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);

	if (showTextures)
	{
		// draw the sea and the sky
		drawSeaAndSky(quad);

		if (showMount)
		{
			glPushMatrix();
			glTranslatef(-0.7, -0.789, 0.0);
			glScalef(i1Scales[0], i1Scales[1], i1Scales[2]);
			drawIsland(quad);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(1.7, -0.789, -2.0);
			glScalef(i2Scales[0], i2Scales[1], i2Scales[2]);
			drawIsland(quad);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(1.7, -0.789, 2.0);
			glScalef(i3Scales[0], i3Scales[1], i3Scales[2]);
			drawIsland(quad);
			glPopMatrix();
		}
	}
	
	if (showGrid)
	{
		// draw point of reference	
		drawReferencePoint(quad);
		// draw grid
		drawGrid();
	}



	glPushMatrix();
	drawPlane();
	glPopMatrix();

	 
	glTranslatef(-cameraPosition[0], -cameraPosition[1], -cameraPosition[2]);

	glutSwapBuffers();

	cameraPosition[0] += cameraSpeed * dirVector.dirVector[0];
	cameraPosition[2] += cameraSpeed * dirVector.dirVector[2];

}

/************************************************************************


Function:		specialKeys


Description:	Handles special key press functionality


*************************************************************************/
void specialKeys(int key, int x, int y)
{
	switch (key)
	{
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
	//cameraPosition[2] -= interpDiff + speed;

	// map camera movement to keys
	if (moveUp)
	{
		cameraPosition[1] += interpDiff;
	}
	if (moveDown)
	{
		cameraPosition[1] -= interpDiff;
	}
	if (increaseSpeed)
	{
		speed += 0.0002;
		cameraSpeed += speed;
		increaseSpeed = 0;
	}
	if (decreaseSpeed)
	{
		speed -= 0.0002;
		cameraSpeed += speed;
		if (speed < 0)
		{
			speed = 0;
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

Function:		myMouse

Description:	Handles mouse movement 


*************************************************************************/
void myMouse(int x, int y)
{
	int diffx = x - lastX; //check the difference between the current x and the last x position
	lastX = x; //set lastx to the current x position
	angle += (float)diffx;
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

	// make snow and rain "fall"
	for (int i = 0; i < SNOW_NUM; i++)
	{
		if (startSnow)
		{
			snowPosY[i] -= 0.002;
		}

		if (startRain)
		{
			rainPosY[i] -= 0.002;
		}
	}
	theta += 0.02;
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
	// toggle between grid and texture when s is pressed
	case 's':
		if (showGrid)
		{
			showGrid = 0;
			showTextures = 1;
		}
		else
		{
			showTextures = 0;
			showGrid = 1;
		}
		break;
	// toggle fog on and off when b is pressed
	case 'b':
		if (showFog)
		{
			showFog = 0;
		}
		else
		{
			showFog = 1;
		}
		break;
	// toggle mountain and mountain textures
	case 'm':
		if (showMount)
		{
			showMount = 0;
		}
		else
		{
			showMount = 1;
		}
		break;
	case 't':
		if (showMountTextures)
		{
			showMountTextures = 0;
		}
		else
		{
			showMountTextures = 1;
		}
		break;
	// select plane colors
	case '1':
		if (firstSkin)
		{
			firstSkin;
		}
		else
		{
			firstSkin = 1;
			secondSkin = thirdSkin = 0;
		}
		break;
	case '2':
		if (secondSkin == 0)
		{
			secondSkin = 1;
			firstSkin = thirdSkin = 0;
		}
		break;
	case '3':
		if (thirdSkin == 0)
		{
			thirdSkin = 1;
			firstSkin = secondSkin = 0;
		}
		break;
	// select weather
	case '4':
		if (startRain == 0)
		{
			startRain = 1;
			startMist = startSnow = 0;
		}
		else
		{
			startRain = 0;
		}
		break;
	case '5':
		if (startSnow == 0)
		{
			startSnow = 1;
			startRain = startMist = 0;
		}
		else
		{
			startSnow = 0;
		}
		break;
	case '6':
		if (startMist == 0)
		{
			startMist = 1;
			startSnow = startRain = 0;
		}
		else
		{
			startMist = 0;
		}
		break;

	// select time of day
	case '7':
		if (sunrise)
		{
			sunrise;
		}
		else
		{
			sunrise = 1;
			sunset = night = 0;
		}
		break;
	case '8':
		if (sunset == 0)
		{
			sunset = 1;
			sunrise = night = 0;
		}
		break;
	case '9':
		if (night == 0)
		{
			night = 1;
			sunrise = sunset = 0;
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

	glutPassiveMotionFunc(myMouse);

	// initialize the rendering context
	initializeGL();

	// go into perpetual loop
	glutMainLoop();
}