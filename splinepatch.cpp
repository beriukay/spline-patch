/* Bucky Frost
 * Paul Gentemann
 * CS 381
 * File Name : splinepatch.cpp
 * Last Modified : Mon 04 Nov 2013 05:30:33 PM AKST
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
#include "lib381/bitmapprinter.h"
#include "lib381/glslprog.h"    // For GLSL code-handling functions
#include <iostream>
using std::cout; using std::cerr; using std::endl;
#include <vector>
using std::vector;
// Global variables
const int ESCKEY = 27;          // ASCII value of Escape
const int startwinsize = 600;   // Start window width & height (pixels)

int main(int argc, char ** argv)
{
    // Initilization of OpenGL/GLUT
    glutInit(&argc, argv);
    getShaderFilenames(vshader1fname, fshader1fname, argc, argv);

    // Set shader source filenames. Done here, as opposed to in
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

    // Start GLUT event handling loop
    glutMainLoop();

    return 0;
}
