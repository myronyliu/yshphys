// yshphys.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Box.h"
#include "Geometry.h"

#include <glew.h>
#include "Window.h"

int main(int argc, char *args[])
{
	Window window;
	window.CreateWindow();
	SDL_Delay(8000);

	Box box1;
	Box box2;
	box1.SetDimensions(1, 1, 1);
	box2.SetDimensions(1, 1, 1);
	box1.SetPosition(dVec3(0.0, 0.0, 0.0));
	box2.SetPosition(dVec3(3.0, 0.0, 0.0));
	box1.SetRotation(dQuat(0.0, 0.0, 0.0, 1.0));
	box2.SetRotation(dQuat(0.0, 0.0, 0.0, 1.0));
	dVec3 pt1;
	dVec3 pt2;
	dVec3 pt3;
	dVec3 pt4;
	double asdf = box1.ComputeSeparation(&box2, pt1, pt2);
	double qwer = box2.ComputeSeparation(&box1, pt3, pt4);
	
	SDL_Quit();

    return 0;
}

