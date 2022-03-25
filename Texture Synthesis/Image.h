#ifndef _Image_h
#define _Image_h

#include <vector>
#include <tuple>
#include <utility>
#include <random>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <boost/filesystem.hpp>


class Image
{
public:
	Image();
	~Image();

	std::vector<boost::filesystem::path> GetFiles();
	std::vector<cv::Mat> GetImages();

	void ReadImages();
	void WriteImages(cv::Mat & image, int currPos, int windowSize);
	void Synthesis();
private:
	cv::Mat GrowImage(const cv::Mat & imageSample,cv::Mat & image, int windowSize);
	cv::Mat GetUnfilledNeighbors(cv::Mat & image);
	cv::Rect GetNeighborhoodWindow(const cv::Mat image, const int windowSize, const cv::Point pixel);
	std::vector<std::tuple<float, cv::Point>> FindMatches(const cv::Mat roi, const cv::Mat & imageSample, const int windowSize);
	cv::Vec3b getColor(const cv::Point location, const cv::Mat& imagesSample);
	std::tuple<int,int> getXandYoffsets(const cv::Mat image, const int windowSize, const cv::Point pixel);
	void SetPaths();
	void CreateDirectory();

	std::vector<boost::filesystem::path> files;
	std::vector<cv::Mat> images;

	const std::string root = "Images\\";
	const std::string subFolder = "Synth\\";
};

#endif