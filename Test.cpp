#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow)
{
	Window window(L"Test Window", 400, 300, hInstance, 200, 150);

	auto textBox = window.addTextBox(L"text", 10, 11, 100, 18, ANCHOR_NONE);
	auto button = window.addButton(L"Click", 115, 10, 70, 20, ANCHOR_NONE, [&textBox] { MessageBox(0, textBox->getText().c_str(), L"TextBox Text", MB_OK); }, true);

	if(!window.show())
		MessageBox(0, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);

	return 0;
}