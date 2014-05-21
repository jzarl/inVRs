
/* 
 * system includes
 */


#include <cstdlib>
#include <vector>
#include <iostream>

#include <GL/glut.h>
#include <GL/glu.h>

#include <ufo/UfoDB.h>
#include <ufo/Configurator.h>
#include <ufo/ConfigurationReader.h>
#include <ufo/PlainConfigurationReader.h>

#include <gmtl/Vec.h>

#include "SimpleObject.h"

using namespace std;
using namespace ufo;

/* initial window extent */
#define WIN_SIZE_X 400
#define WIN_SIZE_Y 400

/* current window extent */
static int win_width;
static int win_height;
static vector<SimpleObject *> objects;
static long timeold;


void drawSphere(GLdouble x, GLdouble y, GLdouble z) {
	GLUquadricObj *sphere;

	sphere = gluNewQuadric();
	glPushMatrix();
	glTranslated(x,y,z);
	gluSphere(sphere, 0.03, 10, 10);
	glPopMatrix();
	gluDeleteQuadric(sphere);
}

void drawLine(GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2) {
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glVertex3f(x1,y1,z1);
	glVertex3f(x2,y2,z2);
	glEnd();
	glEnable(GL_LIGHTING);
}

void drawCoords (GLfloat x, GLfloat y, GLfloat z) {

	/* draw coordinate axis */
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10.0);
	glEnd();

	glEnable(GL_LIGHTING);
}

/*
 * display
 *
 * this function is called by the glut-main loop to create
 * application specific window content.
 *
 * Keep in mind that render operations won't appear on the screen
 * until glutSwapBuffers is called. This is due to the 
 * double buffer mode in which each rendering operation 
 * applies to an invisible back buffer
 */
void display (void) {
	glClearColor (0.5f, 0.5f, 0.5f, 0.0f);  /* set background color   */
	glClear (GL_COLOR_BUFFER_BIT|           /* erase the color buffer */ 
			GL_DEPTH_BUFFER_BIT);          /* and z-buffer */

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	glFrustum(-1.5, 1.5, -1.5, 1.5, 19.0, 25.0);
	glTranslated(0.0, 0.0, -22.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
	glScalef(0.1f, 0.1f, 0.1f);
	glRotated(-45.0, 1.0, 0.0, 0.0);
	glRotated(-135.0, 0.0, 0.0, 1.0);
	drawCoords (0.0, 0.0, 0.0);
	drawSphere( 10.0,0.0,0.0);

	long timenew = glutGet(GLUT_ELAPSED_TIME);
	float dt = (timenew - timeold) / 1000.0f;
	timeold = timenew;
	//update flocks and/or pilots:
	UfoDB::the()->update(dt);
	//move and draw objects:
	for ( vector<SimpleObject *>::const_iterator it = objects.begin();
			it != objects.end(); ++it)
	{
		static long tm = 0;
		(*it)->draw();
		if ( (timeold - tm) >= 10000) // print all 10 seconds
		{
			gmtl::Vec3f pos;
			pos = (*it)->getPosition();
			tm = timeold;
			cout << "Time: " << timeold << ", dt: " << dt << " - SimpleObject at position " << pos[0] <<", " << pos[1] <<", " << pos[2] << endl;
		}
	}
	glutSwapBuffers ();
	glutPostRedisplay();
}

/*
 * reshape
 *
 * called after altering the window's position / size 
 */
void reshape (int w, int h) {
	win_width  = w;
	win_height = h;

	glViewport (0, 0, w, h);
}

/*
 * myInit
 *
 * application specific preparation/setup tasks 
 */
void myInit (int argc, char **argv) {
	GLfloat matSpecular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat matShininess[] = {50.0};
	GLfloat lightPosition[] = {1.0, -1.0, 1.0, 0.0};

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);


	/* setup some gl-flags */
	glDepthFunc (GL_LESS);     /* alternative: GL_LEQUAL */
	glEnable (GL_DEPTH_TEST);  /* use depth buffering for HSR */

	/* ufo init: */
	Configurator *cfg;
	ConfigurationReader *cfgReader;

	if ( argc == 2 )
	{
		cfgReader = new PlainConfigurationReader(argv[1]);
	} else {
		cfgReader = new PlainConfigurationReader("basicglut.cfg");
	}


	cfg = cfgReader->readConfig();

	// we don't need the cfgReader any more:
	delete cfgReader;

	if ( cfg == 0 )
	{
		printf("ERROR: readConfig() failed!\n");
		exit(1);
	}

	// we can mix plugins and hard-coded classes:
	UfoDB::the()->registerSteerableType("SimpleObject",&SimpleObjectStore::theFactory);

	if ( ! cfg->bind() )
	{
		printf("ERROR: bind() failed!\n");
		exit(1);
	}
	// Configurator is no longer needed:
	delete cfg;

	printf("INFO: configuration has %d Flocks and %d independent Pilots.\n"
			, (unsigned int)UfoDB::the()->getFlocks().size(), (unsigned int)UfoDB::the()->getPilots().size());

	objects = SimpleObjectStore::getObjects();
	printf("Got %d SimpleObjects\n",(unsigned int)objects.size());

	UfoDB::the()->print();
}

/*
 * some user interface related callback functions
 *
 * keyboard    - called on keyboard events
 */
	void keyboard (unsigned char key, int x, int y) {
		switch (key)
		{
			case 27:
			case 'q':
				exit(0);
				break;
			default:
				printf("Unknown key: %c\n", key);
		}
	}

/*
 * InitialGLUT
 *
 * creates the GL-Window and adds the callback functions defined 
 * above to the glut-kernel
 */
void InitialGLUT (int argc, char **argv) {
	char title[] = "Basic GLUT demo";

	win_width  = WIN_SIZE_X;
	win_height = WIN_SIZE_Y;

	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize (win_width, win_height);
	glutInitWindowPosition (0, 0);
	glutCreateWindow (title);

	myInit (argc,argv);
	cout << "GL init complete." << endl;

	glutDisplayFunc (display);
	glutIdleFunc (display);
	glutReshapeFunc (reshape);
	glutKeyboardFunc (keyboard);
}

/*
 * the main function
 */
int main (int argc, char **argv) {
	glutInit (&argc, argv);
	InitialGLUT (argc, argv);
	glutMainLoop ();
	return 0;
}
