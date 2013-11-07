/* Paul Gentemann
 * Bucky Frost
 * CS 381 Fall 2013
 * File Name : mouse.h
 * Last Modified : 2013-11-06
 * Description : Defines a cpp class for the gathering
 *      and storing state information about mouse usage.
 *      Such as what buttons are currently depressed and the
 *      x & y coords of the mouse.
 */

#ifndef MOUSE_H
#define MOUSE_H

class Mouse
{
    public:
        Mouse(){ xpos = 0; ypos = 0; }
        Mouse(int x, int y){ xpos = x; ypos = y; }

        int getX(){ return xpos; }
        int getY(){return ypos; }

        void setX(int x){ xpos = x; }
        void setY(int y){ ypos = y; }
        void saveMouse(int x, int y){ xpos = x; ypos = y; }

        bool getLeft(){ return leftDown; }
        bool getMiddle(){ return middleDown; }
        bool getRight(){ return rightDown; }

        void setLeft(bool state){ leftDown = state; }
        void setMiddle(bool state){ middleDown = state; }
        void setRight(bool state){ rightDown = state; }

        /*Currently using the following compiler generated funcs:
            copy
            copy assign
            destruct*/

        //virtual ~Mouse();
    protected:
    private:
        int xpos;
        int ypos;
        bool leftDown;
        bool middleDown;
        bool rightDown;
};

#endif // MOUSE_H
