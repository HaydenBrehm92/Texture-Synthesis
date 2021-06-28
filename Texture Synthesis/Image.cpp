#include "Image.h"
#include <sstream>

namespace f = boost::filesystem;


Image::Image()
{
}

Image::~Image()
{
}

std::vector<f::path> Image::GetFiles()
{
	return files;
}

std::vector<cv::Mat> Image::GetImages()
{
	return images;
}

void Image::CreateDirectory()
{
	if (!f::exists(root + subFolder))
	{
		f::create_directory(root + subFolder);
	}
}

cv::Mat Image::GrowImage(cv::Mat& imageSample, cv::Mat& image, cv::Mat windowSize)
{
	/*bool progress = false;
	while
	return cv::Mat();*/
}

/* Fantastic implementation of grabbing files w/ext taken from here
https://stackoverflow.com/questions/11140483/how-to-get-list-of-files-with-a-specific-extension-in-a-given-folder */
void Image::SetPaths()
{
	if (f::exists(root) && f::is_directory(root))
	{
		for (auto const& entry : f::directory_iterator(root))
		{
			if (f::is_regular_file(entry) && (entry.path().extension() == ".jpg" || entry.path().extension() == ".png"))
			{
				files.emplace_back(entry.path().filename());
				std::cout << entry.path().filename() << std::endl;
			}	
		}
	}
}

void Image::ReadImages()
{
	SetPaths();
	cv::Mat currentImg;
	for (auto const& entry : files)
	{
		currentImg = cv::imread(root + entry.string(), cv::IMREAD_COLOR);
		if (currentImg.empty())
			std::cerr << "Could not read image: " << entry.string() << std::endl;
		else
			images.push_back(currentImg);
	}
}

void Image::WriteImages()
{
	CreateDirectory();
	auto it = files.begin();	
	f::path temp;
	std::ostringstream filename;
	for (auto const& entry : images)
	{
		temp = *it;
		filename << root << subFolder << "new_" << temp.string();
		cv::imwrite(filename.str(), entry);
		std::cout << filename.str() << std::endl;
		it = (it + 1 != files.end()) ? it + 1 : it;
	}
}

void Image::Synthesis()
{

}
