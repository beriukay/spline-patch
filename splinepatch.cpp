/* Bucky Frost
 * Paul Gentemann
 * CS 381
 * File Name : splinepatch.cpp
 * Last Modified : Sat 09 Nov 2013 09:26:20 PM AKST
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
#include <sstream>
using std::ostringstream;
#include <iomanip>
using std::setprecision;
using std::fixed;
#include <cmath>
using std::sin;
#include "mouse.h"
Mouse mousy = Mouse();

// Function prototypes
void documentation();
void drawBezierPatch(int subdivs, double mod);
void myDisplay();
void myIdle();
void reset(GLdouble *arr);
void myKeyboard(unsigned char key, int x, int y);
void myPassiveMotion(int x, int y);
void init();
void myReshape(int w, int h);
void set(GLdouble *arr, GLdouble tx, GLdouble ty, GLdouble tz,
         GLdouble rang, GLdouble rx, GLdouble ry, GLdouble rz);

// Global variables
const int ESCKEY = 27;         // ASCII value of Escape
const int startwinsize = 600;  // Start window width & height (pixels)
int winw = 1, winh = 1;        // Window width, height (pixels)
bool help = false;

int numsubdivs;                // Number of subdivisions for object
const int minsubdivs = 1;      //  Minumum of above
const int maxsubdivs = 50;     //  Maximum of above

bool wave;                     // Starts up the wave to propagate throught the splines

// Shaders
string vshader1fname;          // Filename for vertex shader source
string fshader1fname;          // Filename for fragment shader source
GLhandleARB prog1;             // GLSL Program Object
bool shaderbool1 = false;
GLfloat shaderfloat1 = .5;

// Camera
GLdouble viewmatrix[16],
         viewang = 5.;         // Degrees

double modd;                   // Value for used in the waves in the Bezier patches

// Bezier Patches
GLdouble b1[48] = {
    1.5,-1.5, 0.0,   1.5,-0.5,-3.0,   1.5, 0.5,-2.0,   1.5, 1.5, 1.0,
    0.5,-1.5, 0.0,   0.5,-0.5, 0.0,   0.5, 0.5, 0.0,   0.5, 1.5, 0.0,
   -0.5,-1.5, 0.0,  -0.5,-0.5, 1.0,  -0.5, 0.5,-3.0,  -0.5, 1.5, 1.0,
   -1.5,-1.5, 1.0,  -1.5,-0.5,-2.0,  -1.5, 0.5, 1.0,  -1.5, 1.5, 0.0};

GLdouble b2[48] = {
    1.5, 1.5, 1.0,   1.5, 0.5,-2.0,   2.5, 0.5,-2.0,   1.5, 1.5, 1.0,
    0.5, 1.5, 0.0,   0.5, 0.5, 0.0,   1.5, 0.5, 0.0,   0.5, 1.5, 0.0,
   -0.5, 1.5, 1.0,  -0.5, 0.5,-3.0,  -1.5, 0.5,-3.0,  -0.5, 1.5, 1.0,
   -1.5, 1.5, 0.0,  -1.5, 0.5, 1.0,  -2.5, 0.5, 1.0,  -1.5, 1.5, 0.0};



// drawBezierPatch
// Draws a number of control points for a bezier patch
// The z coordinates of all the points are translated
// by the sine of the mod parameter.
void drawBezierPatch(int subdivs, double mod)
{
    glMap2d(GL_MAP2_VERTEX_3, 0., 1., 3, 4, 0., 1., 3*4, 4, cpts);
    glEnable(GL_MAP2_VERTEX_3);

    glMapGrid2d(subdivs, 0., 1., subdivs, 0., 1.);
    glEnable(GL_AUTO_NORMAL);

    glFrontFace(GL_CW);  // Normals are evidently backwards here :-(
    glEvalMesh2(GL_FILL, 0, subdivs, 0, subdivs);
    glFrontFace(GL_CCW);
}

// myDisplay
// The GLUT display function
void myDisplay()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLhandleARB theprog;  // CURRENTLY-used program object or 0 if none

    // Activating the shaders
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    theprog = prog1;

    glEnable(GL_DEPTH_TEST);    // Set up 3D

    // Setting up the camera view
    glLoadIdentity();
    glMultMatrixd(viewmatrix);



    // Position light source 0 & draw ball there
    glPushMatrix();
        glTranslated(0.0, 0.0, 1.0);
//        glRotated(lightrotang, 1.,0.,0.);
        glTranslated(-1., 1., 1.);
        GLfloat origin4[] = { 0.f, 0.f, 0.f, 1.f };
        glLightfv(GL_LIGHT0, GL_POSITION, origin4);
        GLfloat spotdir3[] = { 1.f, -1.f, -1.f };
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotdir3);
        glUseProgramObjectARB(0);
        glColor3d(1., 1., 1.);
        glutSolidSphere(0.1, 20, 15);  // obj for light source
    glPopMatrix();


    //Send info to shader
    if (theprog)
    {
        GLint loc;  // Location for shader vars

        loc = glGetUniformLocationARB(theprog, "myb1");
        if (loc != -1)
        {
            glUniform1i(loc, shaderbool1);
        }

        loc = glGetUniformLocationARB(theprog, "myf1");
        if (loc != -1)
        {
            glUniform1f(loc, shaderfloat1);
        }
    }

    // Draw objects
    glColor3b(1, 1, 1);
    glTranslated(0,0,-4);
    if(wave)
        drawBezierPatch(numsubdivs, modd);
    else
        drawBezierPatch(numsubdivs, 0);

    documentation();
    glutSwapBuffers();
}

// myIdle
// The GLUT idle function
void myIdle()
{
    modd += 0.1;
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
        set(viewmatrix, 0., 0., 1.);
        break;
    case '-':
        set(viewmatrix, 0., 0., -1.);
        break;
    case '<':
        set(viewmatrix, 0., -1., 0., 10,1,0,0);
        break;
    case '>':
        set(viewmatrix, 0., 1., 0., -10,1,0,0);
        break;
    case ',':
        set(viewmatrix, 0., -1., 0., 10,0,1,0);
        break;
    case '.':
        set(viewmatrix, 0., 1., 0., -10,0,1,0);
        break;

    case 'R':
    case 'r':
        reset(viewmatrix);
        break;
    case ' ':
        wave = !wave;
        break;
    case '(':
        if(numsubdivs > minsubdivs)
            --numsubdivs;
        break;
    case ')':
        if(numsubdivs < maxsubdivs)
            ++numsubdivs;
        break;
    case 'h':
    case 'H':
        help = !help;
    case ' ':     // Space: toggle shader bool
        shaderbool1 = !shaderbool1;
        glutPostRedisplay();
        break;
    case '[':     // [: Decrease shader float
        shaderfloat1 -= 0.02;
        if (shaderfloat1 < 0.)
            shaderfloat1 = 0.;
        glutPostRedisplay();
        break;
    case ']':     // ]: Increase shader float
        shaderfloat1 += 0.02;
        if (shaderfloat1 > 1.)
            shaderfloat1 = 1.;
        glutPostRedisplay();
        break;
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
    modd = 0.0;
    wave = false;
    numsubdivs = 1;
}

// myPassiveMotion
// The GLUT passiveMotion function
void myPassiveMotion(int x, int y)
{
    // Save mouse position in class (haha)
    mousy.saveMouse(x, y);
}

// The main
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
    glutSpecialFunc(mySpecial);
    glutPassiveMotionFunc(myPassiveMotion);

    // Start GLUT event handling loop
    glutMainLoop();

    return 0;
}

void documentation()
{
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
            ostringstream os2;
            os2 << fixed << setprecision(2) << numsubdivs;
            p.print("Arrows         Rotate Scene");
            p.print("( )            Change Subdivisions (" + os2.str() + ")");
            p.print("+/-            Zoom in/out");
            p.print("r              Reset Camera");
            p.print(string("space          Start wave (" )+ (wave ? "true" : "false") + ")");
        }
        else
        {
            p.print("h              help");
        }
        p.print("Esc            Quit");
    glPopMatrix();                // Restore prev projection
    glMatrixMode(GL_MODELVIEW);
}
