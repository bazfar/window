#include "Window.h"

#define WINDOW_CLASS_NAME L"Window"

int Window::controlIdCounter = 0;
bool Window::wndClassRegistered = false;

std::wstring TextBox::getText()
{
	int length = GetWindowTextLength(m_hwnd);

	std::wstring text;
	text.resize(length);

	GetWindowText(m_hwnd, &text[0], length + 1);

	return text;
}

Window::Window(std::wstring title, int width, int height, HINSTANCE hInst, int minWidth, int minHeight) :
	m_mainWindowWidth(width), m_mainWindowHeight(height), m_hInstance(hInst), m_hMainWindow(0), m_minimumWidth(minWidth), m_minimumHeight(minHeight)
{
	if(title.size() > 32)
		title.erase(32);

	m_title = std::move(title);
}

Window::~Window()
{
	if(wndClassRegistered)
	{
		UnregisterClass(WINDOW_CLASS_NAME, m_hInstance);

		wndClassRegistered = false;
	}
}

bool Window::show()
{
	RECT rc;
	
	rc.top = 0;
	rc.left = 0;
	rc.right = m_mainWindowWidth;
	rc.bottom = m_mainWindowHeight;

	if(!AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE))
		return false;

	if(!wndClassRegistered)
	{
		WNDCLASS wc;

		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hInstance = m_hInstance;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = WINDOW_CLASS_NAME;
		wc.style = 0;
		wc.lpfnWndProc = (WNDPROC)WndProc;

		RegisterClass(&wc);

		wndClassRegistered = true;
	}

	m_hMainWindow = CreateWindow(WINDOW_CLASS_NAME, m_title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, m_hInstance, NULL);

	if(!m_hMainWindow)
		return false;

	SetWindowLongPtr(m_hMainWindow, GWLP_USERDATA, reinterpret_cast<LONG>(this));

	if(!createControls())
		return false;

	ShowWindow(m_hMainWindow, SW_SHOW);

	UpdateWindow(m_hMainWindow);

	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		if(msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN)
		{
			if(m_defaultButton)
				m_defaultButton->m_click();
		}
		else
		{
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return true;
}

std::shared_ptr<Button> Window::addButton(std::wstring text, int x, int y, int width, int height, int anchor, std::function<void()> click, bool defaultButton)
{
	auto control = std::make_shared<Button>(x, y, width, height, anchor, getNewId(), click, text, defaultButton);

	m_buttons.push_back(control);

	if(defaultButton)
		m_defaultButton = control;

	return control;
}

std::shared_ptr<TextBox> Window::addTextBox(std::wstring text, int x, int y, int width, int height, int anchor)
{
	auto control = std::make_shared<TextBox>(x, y, width, height, anchor, getNewId(), text);

	m_textBoxes.push_back(control);

	return control;
}

void Window::msgBox(std::wstring title, std::wstring message, UINT style)
{
	MessageBox(m_hMainWindow, message.c_str(), title.c_str(), style);
}

bool Window::createControls()
{
	for(auto it = m_buttons.begin(); it != m_buttons.end(); ++it)
	{
		auto control = *it;

		control->m_hwnd = CreateWindow(L"BUTTON", control->m_text.c_str(), WS_CHILD | WS_VISIBLE | (control->m_default ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON), control->m_x,
			control->m_y, control->m_width, control->m_height, m_hMainWindow, (HMENU)control->m_id, m_hInstance, NULL);

		if(!control->m_hwnd)
			return false;

		SendMessage(control->m_hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0));
	}

	for(auto it = m_textBoxes.begin(); it != m_textBoxes.end(); ++it)
	{
		auto control = *it;

		control->m_hwnd = CreateWindow(L"EDIT", control->m_text.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER, control->m_x,
			control->m_y, control->m_width, control->m_height, m_hMainWindow, (HMENU)control->m_id, m_hInstance, NULL);

		if(!control->m_hwnd)
			return false;

		SendMessage(control->m_hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), MAKELPARAM(TRUE, 0));
		SendMessage(control->m_hwnd, EM_SETMARGINS, static_cast<WPARAM>(EC_LEFTMARGIN | EC_RIGHTMARGIN), MAKELPARAM(2, 2));
	}

	return true;
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CLOSE:
		{
			DestroyWindow(hWnd);

			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);

			break;
		}

		case WM_COMMAND:
			if(wParam == IDCANCEL)
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			else
			{
				Window* w = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			
				for(auto it = w->m_buttons.begin(); it != w->m_buttons.end(); ++it)
				{
					auto button = *it;

					if(LOWORD(wParam) == button->m_id)
					{
						if(HIWORD(wParam) == BN_CLICKED)
							if(button->m_click)
								button->m_click();

						break;
					}
				}
			}

			break;

		case WM_SIZE:
		{
			int newWidth = LOWORD(lParam);
			int newHeight = HIWORD(lParam);

			Window* w = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

			for(auto it = w->m_buttons.begin(); it != w->m_buttons.end(); ++it)
			{
				auto button = *it;

				if(button->m_anchor & ANCHOR_RIGHT)
				{
					if(button->m_anchor & ANCHOR_LEFT)
					{
						button->m_width += newWidth - w->m_mainWindowWidth;

						SetWindowPos(button->m_hwnd, NULL, button->m_x, button->m_y, button->m_width, button->m_height, 0);
					}
					else
					{
						int distFromRight = w->m_mainWindowWidth - button->m_x;

						button->m_x = newWidth - distFromRight;

						SetWindowPos(button->m_hwnd, NULL, button->m_x, button->m_y, button->m_width, button->m_height, 0);
					}
				}

				if(button->m_anchor & ANCHOR_BOTTOM)
				{
					if(button->m_anchor & ANCHOR_TOP)
					{
						button->m_height += newHeight - w->m_mainWindowHeight;

						SetWindowPos(button->m_hwnd, NULL, button->m_x, button->m_y, button->m_width, button->m_height, 0);
					}
					else
					{
						int distFromBottom = w->m_mainWindowHeight - button->m_y;

						button->m_y = newHeight - distFromBottom;

						SetWindowPos(button->m_hwnd, NULL, button->m_x, button->m_y, button->m_width, button->m_height, 0);
					}
				}
			}

			w->m_mainWindowWidth = newWidth;
			w->m_mainWindowHeight = newHeight;

			break;
		}

		case WM_GETMINMAXINFO:
		{
			Window *pw = (Window *)GetWindowLong(hWnd, GWLP_USERDATA);

			if(pw == NULL)
				break;

			MINMAXINFO *mmi = (MINMAXINFO *)lParam;

			mmi->ptMinTrackSize.x = pw->m_minimumWidth;
			mmi->ptMinTrackSize.y = pw->m_minimumHeight;

			break;
		}

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
			break;
	}

	return 0;
}

int Window::getNewId()
{
	int id = ++controlIdCounter;

	if(id == IDCANCEL)
		id = ++controlIdCounter;

	return id;
}

/*
void Window::setButtonText(WINDOW_CONTROL buttonHandle, const char *text)
{
	for(std::list<Button>::iterator it = buttons.begin(); it != buttons.end(); ++it)
	{
		if(it->handle == buttonHandle)
		{
			SetWindowText(it->hButton, text);

			break;
		}
	}
}

void Window::setButtonEnabled(WINDOW_CONTROL buttonHandle, bool enabled)
{
	for(std::list<Button>::iterator it = buttons.begin(); it != buttons.end(); ++it)
	{
		if(it->handle == buttonHandle)
		{
			EnableWindow(it->hButton, enabled);

			break;
		}
	}
}
*/