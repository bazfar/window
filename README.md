# window

A very simple, minimal C++ Win32 GUI library

## Usage

Simply drop the files Window.cpp and Window.h into your project and use it like this:

```C++
// Create a new Window object with the title "Test Window", size of 400x300
// and minimum size of 200x150
Window window(L"Test Window", 400, 300, hInstance, 200, 150);

// Add a Text Box control with the text "text", position x=10 y=11, size 100x18,
// anchored to both sides of the window
auto textBox = window.addTextBox(L"text", 10, 11, 100, 18, ANCHOR_LEFT | ANCHOR_RIGHT);

// Add a Button control that displays the Text Box content in a Message Box when
// clicked. The last argument specifies that this is the default button.
auto button = window.addButton(L"Click", 115, 10, 70, 20, ANCHOR_NONE,
	[&textBox] { MessageBox(0, textBox->getText().c_str(), L"TextBox Text", MB_OK); },
	true);

// Show the Window. If the Window creation fails for any reason, display a error message
if(!window.show())
{
	MessageBox(0, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
}
```