#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Image.h"


/*
	All credit and thanks to Alexei A. Efros and Thomas K. Leung for their algorithm "Texture Synthesis by Non-parametric Sampling."
	More info here: http://people.eecs.berkeley.edu/~efros/research/EfrosLeung.html
	I have implemented the algorithm in C++ as a challenge to myself. 
	A python implementation of this algorithm would be more appropriate.
*/

using namespace cv;

int main()
{
	Image I;
	I.Synthesis();
	return 0;
}