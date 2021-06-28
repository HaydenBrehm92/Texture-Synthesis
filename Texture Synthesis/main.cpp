#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Image.h"
#include "Synth.h"

using namespace cv;

int main()
{
	/*std::string image_path = "brick_sample.jpg";
	Mat img = imread(image_path, IMREAD_COLOR);

	if (img.empty()) 
	{
		std::cerr << "Could not read image: " << image_path << std::endl;
		return 1;
	}*/

	//imshow("Display Window", img);
	//int k = waitKey(0); //wait for keystroke

	//if (k == 's')
	//{
	//	imwrite("new_brick_sample.jpg", img);
	//}

	Image I;
	I.ReadImages();
	I.WriteImages();

	return 0;
}