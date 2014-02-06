/* Paul Gentemann
 * Bucky Frost
 * CS 381 Fall 2013
 * File Name : keyboard.h
 * Last Modified : 2013-11-07
<<<<<<< Updated upstream
 * Description : Defines a cpp class for the gathering
 *      and storing state information about keyboard usage.
 *      Such as what buttons are currently depressed.
 *  This is a change to spline fiddling.
=======
 * Description : Defines a cpp class for the gathering and storing state 
 * information about keyboard usage. Such as what buttons are currently 
 * depressed.
>>>>>>> Stashed changes
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

unsigned short int KEYS = 256;

class Keyboard
{
    public:
        // Only default constructor needed.
        Keyboard(){ for(int i = 0; i < KEYS; ++i) keyState[i] = false; }

        bool getKey(int key) { return keyState[key]; }
        // Read from the current key states.
        const bool* getKeys() { return keyState; }

        void setKey(int key) { keyState[key] = true; }
        void unsetKey(int key) { keyState[key] = false; }

        /*Currently using the following compiler generated funcs:
            copy
            copy assign
            destruct*/

    protected:
    private:
        bool keyState[KEYS];
};

#endif //KEYBOARD_H
