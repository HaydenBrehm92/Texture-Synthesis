#ifndef _Image_h
#define _Image_h

#include <vector>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <boost/filesystem.hpp>


class Image
{
public:
	Image();
	~Image();

	std::vector<boost::filesystem::path> GetFiles();
	std::vector<cv::Mat> GetImages();
	
	void ReadImages();
	void WriteImages();
	void Synthesis();
private:
	cv::Mat GrowImage(cv::Mat & imageSample,cv::Mat & image, cv::Mat windowSize);
	void SetPaths();
	void CreateDirectory();

	std::vector<boost::filesystem::path> files;
	std::vector<cv::Mat> images;

	const std::string root = "Images\\";
	const std::string subFolder = "Synth\\";
};

#endif