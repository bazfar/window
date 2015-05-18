/*
	Simple Win32 GUI class
	
	Copyright (c) 2015 Brett Whiteman
*/

#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow)
{
	Window window(L"Test Window", 400, 300, hInstance, 200, 150);
	
	auto label = window.addLabel(L"Test:", 10, 10, 70, 20, ANCHOR_LEFT | ANCHOR_RIGHT);
	auto textBox = window.addTextBox(L"text", 10, 36, 100, 18, ANCHOR_LEFT | ANCHOR_RIGHT);
	window.addButton(L"Click", 115, 35, 70, 20, ANCHOR_RIGHT, [&textBox, &label] {
		MessageBox(0, textBox->getText().c_str(), L"TextBox Text", MB_OK);
		label->setText(textBox->getText());
	}, true);

	if(!window.show())
		MessageBox(0, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);

	return 0;
}