#include <iostream>
#include <lodepng.h>
#include <fstream>
#include <sstream>
#include "Vector3.hpp"

// The goal for this lab is to draw a triangle mesh loaded from an OBJ file from scratch,
// building on the image drawing code from last week's lab.
// The mesh consists of a list of 3D vertices, that describe the points forming the mesh.
// It also has a list of triangle indices, that determine which vertices are used to form each triangle.
// This time, we'll also load the triangle indices, and use them to draw lines connecting the vertices.
// This will make a wireframe render of the mesh.

void setPixel(std::vector<uint8_t>& image, int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
{
	int pixelIdx = x + y * width;
	image[pixelIdx * 4 + 0] = r;
	image[pixelIdx * 4 + 1] = g;
	image[pixelIdx * 4 + 2] = b;
	image[pixelIdx * 4 + 3] = a;
}

void drawLine(std::vector<uint8_t>& image, int width, int height,
	int startX, int startY, int endX, int endY)
{
	int dx = endX - startX;
	int dy = endY - startY;

	bool steep = abs(dy) > abs(dx);

	if (steep)
	{
		std::swap(startX, startY);
		std::swap(endX, endY);
	}

	if (startX > endX)
	{
		std::swap(startX, endX);
		std::swap(startY, endY);
	}

	dx = endX - startX;
	dy = abs(endY - startY);

	int error = dx / 2;
	int ystep = (startY < endY) ? 1 : -1;
	int y = startY;

	if (steep)
	{
		for (int x = startX; x <= endX; ++x)
		{
			setPixel(image, y, x, width, height, 255, 255, 255);

			error -= dy;
			if (error < 0)
			{
				y += ystep;
				error += dx;
			}
		}
	}
	else
	{
		for (int x = startX; x <= endX; ++x)
		{
			setPixel(image, x, y, width, height, 255, 255, 255);

			error -= dy;
			if (error < 0)
			{
				y += ystep;
				error += dx;
			}
		}
	}
}

int main()
{
	std::string outputFilename = "output.png";

	const int width = 512, height = 512;
	const int nChannels = 4;

	std::vector<uint8_t> imageBuffer(height * width * nChannels);

	memset(&imageBuffer[0], 0, width * height * nChannels * sizeof(uint8_t));

	std::string bunnyFilename = "../models/stanford_bunny_simplified.obj";

	std::ifstream bunnyFile(bunnyFilename);

	std::vector<Vector3> vertices;
	std::vector<std::vector<unsigned int>> faces;

	std::string line;

	while (!bunnyFile.eof())
	{
		std::getline(bunnyFile, line);
		std::stringstream lineSS(line.c_str());

		char lineStart;
		lineSS >> lineStart;

		char ignoreChar;

		if (lineStart == 'v')
		{
			Vector3 v;
			for (int i = 0; i < 3; ++i)
				lineSS >> v[i];

			vertices.push_back(v);
		}

		if (lineStart == 'f')
		{
			std::vector<unsigned int> face;

			for (int i = 0; i < 3; i++)
			{
				unsigned int vertexIndex;
				lineSS >> vertexIndex;

				face.push_back(vertexIndex - 1);
			}

			faces.push_back(face);
		}
	}

	for (int f = 0; f < faces.size(); ++f)
	{
		Vector3 v0 = vertices[faces[f][0]];
		Vector3 v1 = vertices[faces[f][1]];
		Vector3 v2 = vertices[faces[f][2]];

		int x0 = (v0[0] + 1) * width / 2;
		int y0 = (v0[1] + 1) * height / 2;

		int x1 = (v1[0] + 1) * width / 2;
		int y1 = (v1[1] + 1) * height / 2;

		int x2 = (v2[0] + 1) * width / 2;
		int y2 = (v2[1] + 1) * height / 2;

		drawLine(imageBuffer, width, height, x0, y0, x1, y1);
		drawLine(imageBuffer, width, height, x1, y1, x2, y2);
		drawLine(imageBuffer, width, height, x2, y2, x0, y0);
	}

	int errorCode;
	errorCode = lodepng::encode(outputFilename, imageBuffer, width, height);

	if (errorCode)
	{
		std::cout << "lodepng error encoding image: " << lodepng_error_text(errorCode) << std::endl;
		return errorCode;
	}

	return 0;
}