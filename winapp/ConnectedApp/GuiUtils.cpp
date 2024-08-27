#include "../../shared/ImGuiSrc/imgui.h"
#include <iostream>

//Helper function to convert RGBA color value to ImVec4 for ImGui readability
ImVec4 RGBAtoIV4(int r, int g, int b, float a)
{
	float newr = r / 255.0f;
	float newg = g / 255.0f;
	float newb = b / 255.0f;
	float newa = a;
	
	return ImVec4(newr, newg, newb, newa);
}

//Helper function to generate random color for UI elements
ImVec4 RandomDarkColor()
{
	//Random number generator with a seed
	std::srand(static_cast<unsigned int>(std::time(0)));

	//Generates random RGB values between 0 and 100
	int r = std::rand() % 101;
	int g = std::rand() % 101;
	int b = std::rand() % 101;

	//Converts to ImVec4
	return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}