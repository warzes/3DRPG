#pragma once

void Print(const std::string& msg);
void Warning(const std::string& msg);
void Error(const std::string& msg);
void Fatal(const std::string& msg);

namespace app
{
	extern bool        isExit;
	extern GLFWwindow* window;

	void Exit();
}