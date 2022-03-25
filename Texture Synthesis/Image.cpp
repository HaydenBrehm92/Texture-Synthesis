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

/*	imageSample: image we will sample from 
	image: will be the the empty image we want to fill. For constrained synthesis it will contain all known pixels from imageSample (random 3x3 seed if by scratch / include all if constrained)
	windowSize: user defined neighborhood window
*/
cv::Mat Image::GrowImage(const cv::Mat& imageSample, cv::Mat& image, int windowSize)
{
	cv::Mat pixelList, gray(image.size(), CV_8U), thresh;
	double minVal, maxVal;
	bool emptySpace = true;
	bool progress = false;
	float maxThresh = 0.3, val;
	int x = 0;
	int count = 0;
	std::vector<std::tuple<float, cv::Point>> bestMatches;
	while (emptySpace)
	{
		pixelList = GetUnfilledNeighbors(image);
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
		cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY);
		int nonZeros = cv::countNonZero(thresh);
		std::cout << "There are " << nonZeros << " out of r,c: " << image.size().height << " x " << image.size().width << " = " << image.size().height * image.size().width << std::endl;
		/*LEFT OFF HERE*/
		if (cv::countNonZero(thresh) + (windowSize * windowSize) >= image.size().width * image.size().height)	//windowsize added to nonzero count due to an issue with what i think is a rounding issue with windowsize. 
		{
			//std::cout << "there are " << cv::countNonZero << std::endl;
			emptySpace = false;
		}
		progress = false;

		for (int i = 0; i < pixelList.total(); i++)
		{
			//roi using user defined window size
			cv::Point pixel = pixelList.at<cv::Point>(i);
			std::cout << "x: " << pixel.x << "\ny: " << pixel.y << std::endl;
			cv::Mat templateWindow(image, GetNeighborhoodWindow(image, windowSize, pixel));	//Mat around unfilled pixel (synthed image) using user defined window
			int r_pixel, c_pixel;
			r_pixel = pixel.y;
			c_pixel = pixel.x;

			bestMatches = FindMatches(templateWindow, imageSample, windowSize);

			if (!bestMatches.empty())
			{
				std::random_device random;
				std::mt19937 gen(random());
				std::uniform_int_distribution<> distr(0, bestMatches.size() - 1);
				x = distr(gen);

				auto currData = bestMatches.at(x);
				auto currPos = std::get<cv::Point>(currData);
				
				if (std::get<float>(currData) < maxThresh)
				{
					progress = true;
					std::cout << "insert into: (" << r_pixel << ", " << c_pixel << ")\n";

					//GET BGR VALUES HERE
					image.at<cv::Vec3b>(r_pixel,c_pixel) = getColor(std::get<cv::Point>(currData), imageSample);	//changed to image
					count++;
					std::cout << "count #" << count << std::endl;
				}
			}
		}
		if (progress == false)
			maxThresh *= 1.1;
	}
	return image;
}

cv::Mat Image::GetUnfilledNeighbors(cv::Mat& image)
{
	/*	
		1)	Convert original image to grayscale.	
		2)	Use threshold to create a binary image.
		3)	Dilate binary image.
		4)	Subtract binary image from dilated image.
		5)	Return non-zeros as a list of neighbors.
	*/
	cv::Mat grayscaleOrig(image.size(), CV_8U);
	cv::Mat binOrig(image.size(), CV_8U);
	cv::cvtColor(image, grayscaleOrig, cv::COLOR_BGR2GRAY);
	cv::threshold(grayscaleOrig, binOrig, 0, 255, cv::THRESH_BINARY);

	cv::Mat dilated(image.size(), CV_8U);
	cv::dilate(binOrig, dilated, cv::Mat());

	cv::Mat unfilled(image.size(), CV_8U);
	cv::subtract(dilated, binOrig, unfilled);

	cv::Mat neighbors;
	cv::findNonZero(unfilled, neighbors);	//list of non-zero pixels

	return neighbors;
}

cv::Rect Image::GetNeighborhoodWindow(const cv::Mat image, const int windowSize, const cv::Point pixel)
{
	auto offset = getXandYoffsets(image, windowSize, pixel);
	cv::Rect roiWindow(pixel.x - std::get<0>(offset), pixel.y - std::get<1>(offset), windowSize, windowSize);
	return roiWindow;
}

std::vector<std::tuple<float,cv::Point>> Image::FindMatches(const cv::Mat templateWindow, const cv::Mat& imageSample, const int windowSize)
{
	double sigma = windowSize / 6.4;
	std::vector<std::tuple<float, cv::Point>> pixelList;
	cv::Mat gaussKernel, matchResult;
	cv::Mat gaussianImageSample(imageSample.size(), CV_8UC3);
	
	/* Binary Mask */
	cv::Mat validMask(templateWindow.size(), CV_8U);
	cv::Mat grayscaleTemplate(templateWindow.size(), CV_8U);
	cv::cvtColor(templateWindow, grayscaleTemplate, cv::COLOR_BGR2GRAY);
	cv::threshold(grayscaleTemplate, validMask, 0, 255, cv::THRESH_BINARY);

	/* Apply Seperable (1D) gaussian kernel to imagesample (stored in gaussianImageSample) */
	gaussKernel = cv::getGaussianKernel(windowSize, sigma);
	cv::sepFilter2D(imageSample, gaussianImageSample, CV_8U, gaussKernel, gaussKernel);

	cv::matchTemplate(gaussianImageSample, templateWindow, matchResult, cv::TM_SQDIFF_NORMED, validMask);
	double minVal, maxVal, errThreshold = 0.05;
	float pt;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(matchResult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

	// MAKE WINDOW AROUND MINLOC
	cv::Mat resultRoi(matchResult, GetNeighborhoodWindow(matchResult, windowSize, minLoc));
	auto offset = getXandYoffsets(matchResult, windowSize, minLoc);
	std::cout << "MinLoc: " << minLoc << " || x_offset: " << std::get<0>(offset) << "|| y_offset: " << std::get<1>(offset) << " || New Coordinates: (" << minLoc.x + std::get<0>(offset)
		<< ", " << minLoc.y + std::get<1>(offset) << ")\n";

	/* TEST PURPOSES */
	//cv::namedWindow("resultroi", cv::WINDOW_AUTOSIZE);
	//cv::imshow("resultroi", resultRoi);
	//cv::namedWindow("matchResult", cv::WINDOW_AUTOSIZE);
	//cv::imshow("matchResult", matchResult);
	//cv::waitKey(0);

	cv::Point loc;
	for (int i = 0; i < resultRoi.size().height; i++)
	{
		for (int j = 0; j < resultRoi.size().width; j++)
		{
			if (resultRoi.at<float>(i, j) <= (minVal * (1 + errThreshold)))
			{ 
				pt = resultRoi.at<float>(i, j);
				int sample_x_col = j + (minLoc.x - std::get<0>(offset));
				int sample_y_row = i + (minLoc.y - std::get<1>(offset));
				//std::cout << "pt: " << pt << std::endl;
				loc.x = (sample_x_col + (templateWindow.size().width - 1) / 2);
				loc.y = (sample_y_row + (templateWindow.size().height - 1) / 2);
				//loc.x = (j + (templateWindow.size().width - 1) / 2);	//matchResult (W x H) and template (w x h) in template match is (W - w + 1) X (H - h + 1)
				//loc.y = (i + (templateWindow.size().height - 1) / 2);	//Point stores in column major (he
				//std::cout << "loc.x: " << loc.x << " / loc.y: " << loc.y << std::endl;
				auto data = std::make_tuple(pt, loc);
				pixelList.emplace_back(data);
			}
		}	
	}
	return pixelList;
}

cv::Vec3b Image::getColor(const cv::Point location, const cv::Mat& image)
{
	int r, c;
	r = location.y;
	c = location.x;
	cv::Vec3b result;
	result = image.at<cv::Vec3b>(r,c);
	return result;
}

std::tuple<int, int> Image::getXandYoffsets(const cv::Mat image, const int windowSize, const cv::Point pixel)
{
	int offsets = std::floor(windowSize / 2),
		offset_x = offsets,
		offset_y = offsets;
	bool offsetIsValid = false;

	/* Checking for out of bounds and offsetting to stay inside */
	while (!offsetIsValid)
	{

		if (pixel.x - offset_x < 0)
			offset_x -= 1;
		if (pixel.y - offset_y < 0)
			offset_y -= 1;
		if ((pixel.x - offset_x) + windowSize > image.size().width)	//have to account for other half
			offset_x += 1;
		if ((pixel.y - offset_y) + windowSize > image.size().height)
			offset_y += 1;

		offsetIsValid = ((pixel.x - offset_x >= 0 && pixel.y - offset_y >= 0) && ((pixel.x - offset_x) + windowSize <= image.size().width && (pixel.y - offset_y) + windowSize <= image.size().height)) ? true : false;
	}

	return std::make_tuple(offset_x,offset_y);
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

void Image::WriteImages(cv::Mat & image, int currPos, int windowSize)
{
	CreateDirectory();
	auto it = files.begin();	
	f::path temp;
	std::ostringstream filename;
	temp = *it;
	for (int i = currPos; i >= 0; --i)
	{
		temp = *it;
		it = (it + 1 != files.end()) ? it + 1 : it;
	}

	filename << root << subFolder << "WS_"<< windowSize << "x" << windowSize << "_new_" << temp.string();
	cv::imwrite(filename.str(), image);
	std::cout << filename.str() << std::endl;
	
	
}

//need to synth each image with user defined settings for each
//need to put a random 3x3 seed of sample image into image and put the entire pixel collection for constrained
void Image::Synthesis()
{
	ReadImages();
	int windowSize;
	bool unsanitized = true;
	uchar val;
	cv::Mat newImage, synthImage; 

	int x, y;

	do {
		std::cout << "Enter a reasonable non-negative odd numbered window size (e.g. 5, 11, 15, 23)...\n";
		std::cin >> windowSize;
		unsanitized = (windowSize != 1) && (windowSize % 2 > 0) ? false : true;
	} while (unsanitized);

	for (int i = 0; i < images.size(); i++)
	{
		newImage = cv::Mat(cv::Size(images.at(i).size().width * 1.1, images.at(i).size().height * 1.1), CV_8UC3, CV_RGB(0, 0, 0));
		x = (newImage.size().width - images.at(i).size().width) / 2;	//x offset
		y = (newImage.size().height - images.at(i).size().height) / 2;	//y offset

		/*
			https://stackoverflow.com/questions/10481411/copy-an-cvmat-inside-a-roi-of-another-one
			How to use a bounding box (roi) to copy a smaller image to a larger image size
		*/
		images.at(i).copyTo(newImage(cv::Rect(x, y, images.at(i).size().width, images.at(i).size().height)));

		

		synthImage = GrowImage(images.at(i), newImage, windowSize);
		WriteImages(synthImage, i, windowSize);
	}
	
	////for (int i = 0; i < images.size(); i++)
	////{
	////	do {
	////		std::cout << "Press 1 for constrained synthesis...\nPress 2 for synthesis by scratch...\n";
	////		std::cin >> val;
	////		std::cout << "\n";
	////		unsanitized = (val == '1' || val == '2') ? false : true;
	////	} while (unsanitized);

	////	if (val == '1')
	////	{
	////		int x, y;
	////		newImage = cv::Mat(cv::Size(images.at(i).size().width * 1.1, images.at(i).size().height * 1.1), CV_8UC3, CV_RGB(0, 0, 0));
	////		x = (newImage.size().width - images.at(i).size().width) / 2;	//x offset
	////		y = (newImage.size().height - images.at(i).size().height) / 2;	//y offset

	////		/*
	////			https://stackoverflow.com/questions/10481411/copy-an-cvmat-inside-a-roi-of-another-one
	////			How to use a bounding box (roi) to copy a smaller image to a larger image size
	////		*/
	////		images.at(i).copyTo(newImage(cv::Rect(x, y, images.at(i).size().width, images.at(i).size().height)));
	////	}	
	////	else
	////	{
	////		//use a 3x3 random seed
	////		//microsoft docs for random https://docs.microsoft.com/en-us/cpp/standard-library/random?view=msvc-170
	////		std::random_device randomx;
	////		std::random_device randomy;
	////		std::mt19937 genx(randomx());
	////		std::mt19937 geny(randomy());
	////		std::uniform_int_distribution<> distrx(0, images.at(i).size().width - 3);
	////		std::uniform_int_distribution<> distry(0, images.at(i).size().height - 3);
	////		int x = distrx(genx);
	////		int y = distry(geny);
	////		
	////		newImage = cv::Mat(cv::Size(64, 64), CV_8UC3, CV_RGB(0,0,0));

	////		cv::Mat roi(images.at(i), cv::Rect(distrx(genx), distry(geny), 3, 3));
	////		roi.copyTo(newImage(cv::Rect(61, 61, 3, 3)));		
	////	}
			

		//unsanitized = true;
		/*do {
			std::cout << "Enter a reasonable non-negative odd numbered window size (e.g. 5, 11, 15, 23)...\n";
			std::cin >> windowSize;
			unsanitized = (windowSize != 1) && (windowSize % 2 > 0) ? false : true;
		} while (unsanitized);

		synthImage = GrowImage(images.at(i), newImage, windowSize);
		WriteImages(synthImage, i);	*/
}
