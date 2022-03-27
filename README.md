# Texture Synthesis

## All credit to Alexei A. Efros and Thomas K. Leung for "Texture Synthesis by Non-parametric Sampling." This is a personal project based on the algorithm described in their paper.
More information can be found here: http://people.eecs.berkeley.edu/~efros/research/EfrosLeung.html. 

I did this in C++ as a challenge for myself while being wholly unfamiliar with OpenCV.
While the project does not work exactly as it should, I find that spending any more time would not benefit me as much as something new. Not to dismiss the limited success I did find with this project, but i feel that it is time to put a pin in it and learn more about OpenCV through a different project. My goal was to learn more about OpenCV and how it works, and I feel that I have accomplished that.

## This project aims to synthesize texture in images using a sample image as a means to generate a new image of increased size who's extra pixels closely mimic how the texture behaves.

This project uses VS19 with C++, Boost 1.75.0, and OpenCV 4.5.2. You must have at least OpenCV 4.5.2 installed to run (here is a good resource: https://developpaper.com/cmake-compilation-and-installation-of-opencv-under-windows/).

### Original images used:

![Bricks](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/bricks256x256.jpg)

![Bricks cropped](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/bricks_cropped.jpg)

![Leaf](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/leaf.jpg)

![Leather](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/leather.png)

![Small Bricks](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/small_bricks.png)

![Small Bricks cropped](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/small_bricks_cropped.png)

![Wood](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/wood.png)

![Zoomed bricks](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/zoomed_bricks.png)

### Some Success Examples (Increased height and width 10%):

![New Bricks cropped](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/Synth/WS_3x3_new_bricks_cropped.jpg)

![New Leaf](https://github.com/HaydenBrehm92/Texture-Synthesis/tree/master/Texture%20Synthesis/Images/Synth/WS_3x3_new_leaf.jpg)

More examples can be found in /Images/Synth/ 

A common Issue can be found in the top and bottom of the images as well as with increased window size.


