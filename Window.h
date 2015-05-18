/*
	Simple Win32 GUI class
	
	Copyright (c) 2015 Brett Whiteman
*/

#ifndef WINDOW_WINDOW_H
#define WINDOW_WINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <list>
#include <string>
#include <functional>
#include <memory>

#define ANCHOR_LEFT 0x1
#define ANCHOR_TOP 0x2
#define ANCHOR_RIGHT 0x4
#define ANCHOR_BOTTOM 0x8
#define ANCHOR_ALL ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_RIGHT | ANCHOR_BOTTOM
#define ANCHOR_NONE 0x0

typedef int WINDOW_CONTROL;

class Control
{
	friend class Window;
public:
	Control(int x, int y, int width, int height, int anchor, int id) :
		m_x(x), m_y(y), m_width(width), m_height(height), m_anchor(anchor), m_id(id) { }

	virtual ~Control() { }

	std::wstring getText();
	void setText(const std::wstring& text);

protected:
	HWND m_hwnd;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_anchor;
	int m_id;
};

class Button : public Control
{
	friend class Window;
public:
	Button(int x, int y, int width, int height, int anchor, int id, std::function<void()> click, std::wstring text, bool defaultButton) :
		Control(x, y, width, height, anchor, id), m_text(text), m_click(click), m_default(defaultButton) { }

private:
	std::function<void()> m_click;
	std::wstring m_text;
	bool m_default;
};

class TextBox : public Control
{
	friend class Window;
public:
	TextBox(int x, int y, int width, int height, int anchor, int id, std::wstring text) :
		Control(x, y, width, height, anchor, id), m_text(text) { }

private:
	std::wstring m_text;
};

class Label : public Control
{
	friend class Window;
public:
	Label(int x, int y, int width, int height, int anchor, int id, std::wstring text) :
		Control(x, y, width, height, anchor, id), m_text(text) { }

private:
	std::wstring m_text;
};

class Window
{
public:
	Window(std::wstring title, int width, int height, HINSTANCE hInst, int minWidth = 0, int minHeight = 0);
	~Window();

	bool show();

	std::shared_ptr<Button> addButton(std::wstring text, int x, int y, int width, int height, int anchor, std::function<void()> click, bool defaultButton = false);
	std::shared_ptr<TextBox> addTextBox(std::wstring text, int x, int y, int width, int height, int anchor);
	std::shared_ptr<Label> addLabel(std::wstring text, int x, int y, int width, int height, int anchor);

	void msgBox(std::wstring title, std::wstring message, UINT style);
	HWND getMainWindowHandle() { return m_hMainWindow; }

private:
	bool createControls();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void resizeControl(std::shared_ptr<Control> control, int oldWidth, int oldHeight, int newWidth, int newHeight);
	static int getNewId();

	std::wstring m_title;
	int m_mainWindowWidth;
	int m_mainWindowHeight;
	HINSTANCE m_hInstance;
	int m_minimumWidth;
	int m_minimumHeight;
	HWND m_hMainWindow;
	std::list<std::shared_ptr<Button>> m_buttons;
	std::list<std::shared_ptr<TextBox>> m_textBoxes;
	std::list<std::shared_ptr<Label>> m_labels;
	std::shared_ptr<Button> m_defaultButton;

	static int controlIdCounter;
	static bool wndClassRegistered;
	
};

#pragma comment(linker, "/manifestdependency:\"type='win32' "\
						"name='Microsoft.Windows.Common-Controls' "\
						"version='6.0.0.0' "\
						"processorArchitecture='x86' "\
						"publicKeyToken='6595b64144ccf1df' "\
						"language='*' "\
						"\"")
#endif
