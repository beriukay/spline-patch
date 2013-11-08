/* File: spline_f.glsl
 * Authors: Paul Gentemann
 *          Bucky Frost
 * Last Modified : Fri 08 Nov 2013
 * Based off the code by Dr. Glenn G. Chappell
 *
 *
 * For CS 381 Fall 2013
 * Assignment 5
 *
 * Description:
 * GLSL Fragment Shader for use with splinepatch.cpp
 * Blinn-Phong illumination model, per-fragment lighting
 */

uniform float myf1;        // Application float (in [0.,1.]; start: 1.)
uniform bool myb1;         // Application bool (start: true)

varying vec3 surfpt;       // Point on surface (camera coords)
varying vec3 surfnorm_un;  // Surface normal (camera coords)
                           // Normalize before & after sending


// bpLight
// Blinn-Phong illumination model
// Given light-source color & position/direction, surface paint color,
// position, & normal vec (normalized). Return apparent surface color.
vec4 bpLight(
    vec4 lightcolor,
    vec4 lightpos4,  // Homogeneous form
    vec4 paintcolor,
    vec3 surfpt,
    vec3 surfnorm)   // Normalized
{
    // Compute normalized direction of light source from object
    vec3 lightdir = normalize(lightpos4.xyz - surfpt*lightpos4.w);
        // Odd computation above is correct for both positional and
        //  directional lights.

    // Scalar lighting parameters
    float ambientfraction = 0.2;
        // Ambient light color, as fraction of light color
    float shininess = 5. + myf1 * 70.;

    // Ambient
    vec4 ambientcolor = ambientfraction * lightcolor * paintcolor;

    // Diffuse
    float lambertcos = max(0., dot(lightdir, surfnorm));
    vec4 diffusecolor = lambertcos * lightcolor * paintcolor;

    // Specular
    vec3 viewdir = normalize(-surfpt);  // Direction of cam from obj
    vec3 halfway = normalize(viewdir + lightdir);
    float specularcoeff = pow(max(0., dot(surfnorm, halfway)),
                              4.*shininess);
        // Blinn-Phong needs shininiess about 4 * Phong shininess
    vec4 specularcolor = specularcoeff * lightcolor;

    // Compute final color
    return clamp(ambientcolor + diffusecolor
                   + (myb1 ? specularcolor : vec4(0.,0.,0.,1.)),
                 0., 1.);  // Clamp to [0,1]
}


void main()
{
    // Hard-coded light-source color
    vec4 lightcolor = vec4(1., 1., 1., 1.);

    // Light-source pos (camera coordinates) from application
    vec4 lightpos4 = gl_LightSource[0].position;

    // Normalize our normal vector
    vec3 surfnorm = normalize(surfnorm_un);

    // Get surface paint color from vertex shader
    vec4 paintcolor = gl_Color;

    // Apply Blinn-Phong illumination model
    gl_FragColor = bpLight(lightcolor, lightpos4,
                           paintcolor, surfpt, surfnorm);
}

