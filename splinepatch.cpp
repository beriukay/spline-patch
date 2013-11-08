/* Bucky Frost
 * Paul Gentemann
 * CS 381
 * File Name : splinepatch.cpp
 * Last Modified : Thu 07 Nov 2013 12:02:38 PM AKST
 * Description : A pair of spline plains, patched together, that will
 * ripple upon user input.
 */

// OpenGL/GLUT includes - DO THESE FIRST
#include <cstdlib>       // Do this before GL/GLUT includes
using std::exit;
#ifndef __APPLE__
# include <GL/glew.h>
# include <GL/glut.h>    // Includes OpenGL headers as well
#else
# include <GLEW/glew.h>
# include <GLUT/glut.h>  // Apple puts glut.h in a different place
#endif
#ifdef _MSC_VER          // Tell MS-Visual Studio about GLEW lib
# pragma comment(lib, "glew32.lib")
#endif
#include "lib381/bitmapprinter.h"
#include "lib381/glslprog.h"    // For GLSL code-handling functions
#include <string>
using std::string;
#include <iostream>
using std::cout; using std::cerr; using std::endl;
#include <vector>
using std::vector;

// Global variables
const int ESCKEY = 27;          // ASCII value of Escape
const int startwinsize = 600;   // Start window width & height (pixels)
int winw = 1, winh = 1;         // Window width, height (pixels)
bool help = false;

// Shaders
string vshader1fname;          // Filename for vertex shader source
string fshader1fname;          // Filename for fragment shader source

// Camera
GLdouble viewmatrix[16],
         viewang = 5.;          // Degrees


// myDisplay
// The GLUT display function
void myDisplay()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw objects
    glEnable(GL_DEPTH_TEST);    // Set up 3D
    // Do stuff

    // Draw documentation
    glDisable(GL_DEPTH_TEST);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);  // Set up simple ortho projection
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0., double(winw), 0., double(winh));
        glColor3d(0., 0., 0.);        // Black text
        BitmapPrinter p(20., winh-20., 20.);
        if(help)
        {
            p.print("Arrows         Rotate Scene");
            p.print("+/-            Zoom in/out");
            p.print("r              Reset Camera");
        }
        else
        {
            p.print("h              help");
        }
        p.print("Esc            Quit");
    glPopMatrix();                // Restore prev projection
    glMatrixMode(GL_MODELVIEW);
    glutSwapBuffers();
}

// myIdle
// The GLUT idle function
void myIdle()
{
    glutPostRedisplay();
    // Print OpenGL errors, if there are any (for debugging)
    static int error_count = 0;
    if (GLenum err = glGetError())
    {
        ++error_count;
        cerr << "OpenGL ERROR " << error_count << ": "
             << gluErrorString(err) << endl;
    }
}

// set()
// Sets given matrix to given translation/rotation.
void set(GLdouble *arr, GLdouble tx, GLdouble ty, GLdouble tz,
         GLdouble rang=0., GLdouble rx=0., GLdouble ry=0., GLdouble rz=0.)
{
    glLoadIdentity();
    glTranslated(tx, ty, tz);
    glRotated(rang, rx, ry, rz);
    glMultMatrixd(arr);
    glGetDoublev(GL_MODELVIEW_MATRIX, arr);
}

// reset()
// Resets the given matrix to the identity
void reset(GLdouble *arr)
{
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, arr);
}


// myKeyboard
// The GLUT keyboard function
void myKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ESCKEY:  // Esc: quit
        exit(0);
        break;
    case '+':
        set(viewmatrix, 1., 0., 0.);
        break;
    case '-':
        set(viewmatrix,-1., 0., 0.);
        break;
    case 'R':
    case 'r':
        reset(viewmatrix);
        break;
    case 'h':
    case 'H':
        help = !help;
    }
    glutPostRedisplay();
}

// mySpecial
// The GLUT special function
void mySpecial(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:
            set(viewmatrix, 0.,0.,0., -viewang, 0.,1.,0.);
            break;
        case GLUT_KEY_RIGHT:
            set(viewmatrix, 0.,0.,0., viewang, 0.,1.,0.);
            break;
        case GLUT_KEY_UP:
            set(viewmatrix, 0.,0.,0., -viewang,0.,.0,1.);
            break;
        case GLUT_KEY_DOWN:
            set(viewmatrix, 0.,0.,0., viewang,0.,.0,1.);
            break;
    }
    glutPostRedisplay();
}

// myReshape
// The GLUT reshape function
void myReshape(int w, int h)
{
    // Set viewport & save window dimensions in globals
    glViewport(0, 0, w, h);
    winw = w;
    winh = h;
    // Set up projection
    // Perspective projection with near quite near & far pretty far
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60., double(w)/h, 0.01, 100.);
    glMatrixMode(GL_MODELVIEW);  // Always go back to model/view mode
}

// init
// Initialize GL states & global data
void init()
{
    reset(viewmatrix);    // Reset camera position
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char ** argv)
{
    // Initilization of OpenGL/GLUT
    glutInit(&argc, argv);
    // Set shader source filenames. Done here, as opposed to in
    getShaderFilenames(vshader1fname, fshader1fname, argc, argv);
    //  function init, so that we can use command-line arguments.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Creating the view window
    glutInitWindowSize(startwinsize, startwinsize);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("CS 381 - Shaders, Normals, Lighting, and Splines Oh My!");

    // Init GLEW & check status - exit on failure
    if (glewInit() != GLEW_OK)
    {
        cerr << "glewInit failed" << endl;
        exit(1);
    }

    init();
    glutDisplayFunc(myDisplay);
    glutIdleFunc(myIdle);
    glutReshapeFunc(myReshape);
//    glutMouseFunc(myMouse);
    glutKeyboardFunc(myKeyboard);

    // Start GLUT event handling loop
    glutMainLoop();

    return 0;
}

