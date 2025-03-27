#include <Windows.h>
#include <CommCtrl.h>
#include <wchar.h>
#include <time.h>

#define CLASS_PICKER_WINDOW  TEXT("FULLMONCOLOR_COLOR_PICKER_WINDOW_CLASS")
#define CLASS_SHOWER_WINDOW  TEXT("FULLMONCOLOR_COLOR_SHOWER_WINDOW_CLASS")
#define CLASS_TRACKER_WINDOW TEXT("TRACKBAR_CLASS")
#define CLASS_BUTTON_WINDOW TEXT("BUTTON")

#define COLOR_MIN 0
#define COLOR_MAX 255
#define HEIGHT_TRACKBAR 35

int COLOR_R = 0,
	COLOR_G = 0,
	COLOR_B = 0;

HINSTANCE hAppInst = NULL;
HWND hPickerWnd = NULL,
	 hShowerWnd = NULL,
	 hPickerRWnd = NULL,
	 hPickerGWnd = NULL,
	 hPickerBWnd = NULL,
	 hPickerButtonWnd = NULL,
	 hPickerShuffleButtonWnd = NULL;

int InRange(int x, int min, int max)
{
	return x >= min && x <= max;
}

int RandomInRange(int min, int max)
{
	return rand() % (max + 1 - min) + min;
}

LRESULT CALLBACK PickerWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND WINAPI CreateTrackbar
(
	HWND hParentWnd,
	INT uValueMin, INT uValueMax,
	UINT uPosX, UINT uPosY,
	UINT uSizeX, UINT uSizeY
);
HWND WINAPI CreateButton
(
	HWND hParentWnd,
	const TCHAR* pText,
	UINT uPosX, UINT uPosY,
	UINT uSizeX, UINT uSizeY
);

int CALLBACK WinMain
(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPSTR     lpCmdLine,
	_In_     int       nCmdShow
)
{
	hAppInst = hInstance;

	int argc    = __argc;
	char** argv = __argv;
	InitCommonControls();

	// for random
	srand((UINT) time(NULL));

	if (argc == 2)
	{
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "/help") == 0)
		{
			MessageBox
			(
				NULL,
				TEXT
				(
					"FullMonColor HELP: Either execute with 3 parameters (being the RGB color between the inclusive range 0-255) or\n"
					"execute with the flag 'random' to get a random color or\n"
					"execute normally to get a window to pick the color or\n"
					"execute with the flag 'shuffled' to get the color picker window initialized with a random color."
				),
				TEXT("Help Command Interface"),
				MB_OK | MB_ICONINFORMATION
			);
			return EXIT_SUCCESS;
		}
		else if (strcmp(argv[1], "--random") == 0 || strcmp(argv[1], "-random") == 0 || strcmp(argv[1], "/random") == 0)
		{
			COLOR_R = RandomInRange(COLOR_MIN, COLOR_MAX);
			COLOR_G = RandomInRange(COLOR_MIN, COLOR_MAX);
			COLOR_B = RandomInRange(COLOR_MIN, COLOR_MAX);
			goto SECT_Show;
		}
		else if (strcmp(argv[1], "--shuffled") == 0 || strcmp(argv[1], "-shuffled") == 0 || strcmp(argv[1], "/shuffled") == 0)
		{
			COLOR_R = RandomInRange(COLOR_MIN, COLOR_MAX);
			COLOR_G = RandomInRange(COLOR_MIN, COLOR_MAX);
			COLOR_B = RandomInRange(COLOR_MIN, COLOR_MAX);
		}
	}
	else if (argc == 4)
	{
		COLOR_R = atoi(argv[1]);
		COLOR_G = atoi(argv[2]);
		COLOR_B = atoi(argv[3]);

		if (!InRange(COLOR_R, COLOR_MIN, COLOR_MAX),
			!InRange(COLOR_G, COLOR_MIN, COLOR_MAX),
			!InRange(COLOR_B, COLOR_MIN, COLOR_MAX))
		{
			MessageBox
			(
				NULL,
				TEXT("Color field values must be in/between the inclusive range of 0-255!"),
				TEXT("Invalid Command Line Color Arguments"),
				MB_OK | MB_ICONHAND
			);
			return EXIT_FAILURE;
		}
	}
	else
	{
		WNDCLASSEX wcPicker;
		ZeroMemory(&wcPicker, sizeof(WNDCLASSEX));
		
		wcPicker.cbSize = sizeof(WNDCLASSEX);
		wcPicker.style = CS_HREDRAW | CS_VREDRAW;
		wcPicker.lpfnWndProc = PickerWindowProc;
		wcPicker.cbClsExtra = 0;
		wcPicker.cbWndExtra = 0;
		wcPicker.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		wcPicker.hInstance = hInstance;
		wcPicker.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
		wcPicker.hCursor = LoadCursor(hInstance, IDC_ARROW);
		wcPicker.hbrBackground = (HBRUSH) COLOR_WINDOW;
		wcPicker.lpszMenuName = NULL;
		wcPicker.lpszClassName = CLASS_PICKER_WINDOW;

		if (!RegisterClassEx(&wcPicker))
		{
			MessageBox
			(
				NULL,
				TEXT("Failed to register the color picker window class!"),
				TEXT("Internal Error"),
				MB_OK | MB_ICONHAND
			);
			return EXIT_FAILURE;
		}

		hPickerWnd = CreateWindowEx
		(
			0,
			CLASS_PICKER_WINDOW,
			TEXT("FullMonColor - Pick Color"),
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT,
			350, 225,
			NULL,
			NULL,
			hInstance,
			NULL
		);

		if (NULL == hPickerWnd)
		{
			MessageBox
			(
				NULL,
				TEXT("Failed to create the color picker window!"),
				TEXT("Internal Error"),
				MB_OK | MB_ICONHAND
			);
			return EXIT_FAILURE;
		}

		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
		HFONT hFont = CreateFontIndirect(&ncm.lfMessageFont);
		SendMessage(hPickerWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
		
		ShowWindow(hPickerWnd, SW_SHOW);
		UpdateWindow(hPickerWnd);

		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

		while (GetMessage(&msg, hPickerWnd, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			Sleep(1);
		}
	}
SECT_Show:
	UnregisterClass(CLASS_PICKER_WINDOW, hInstance);

	WNDCLASSEX wcShower;
	ZeroMemory(&wcShower, sizeof(WNDCLASSEX));

	wcShower.cbSize = sizeof(WNDCLASSEX);
	wcShower.style = CS_HREDRAW | CS_VREDRAW;
	wcShower.lpfnWndProc = MainWindowProc;
	wcShower.cbClsExtra = 0;
	wcShower.cbWndExtra = 0;
	wcShower.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcShower.hInstance = hInstance;
	wcShower.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wcShower.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wcShower.hbrBackground = CreateSolidBrush(RGB(COLOR_R, COLOR_G, COLOR_B));
	wcShower.lpszMenuName = NULL;
	wcShower.lpszClassName = CLASS_SHOWER_WINDOW;

	if (!RegisterClassEx(&wcShower))
	{
		MessageBox
		(
			NULL,
			TEXT("Failed to register the color shower window class!"),
			TEXT("Internal Error"),
			MB_OK | MB_ICONHAND
		);
		return EXIT_FAILURE;
	}

	int MONX = GetSystemMetrics(SM_CXSCREEN);
	int MONY = GetSystemMetrics(SM_CYSCREEN);

	hShowerWnd = CreateWindowEx
	(
		WS_EX_APPWINDOW,
		CLASS_SHOWER_WINDOW,
		TEXT("FullMonColor - Color Shower"),
		WS_POPUP,
		0, 0,
		MONX, MONY,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (NULL == hShowerWnd)
	{
		MessageBox
		(
			NULL,
			TEXT("Failed to create the color shower window!"),
			TEXT("Internal Error"),
			MB_OK | MB_ICONHAND
		);
		return EXIT_FAILURE;
	}

	ShowWindow(hShowerWnd, SW_SHOW);
	UpdateWindow(hShowerWnd);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (GetMessage(&msg, hShowerWnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		Sleep(1);
	}

	return EXIT_SUCCESS;
}

LRESULT CALLBACK PickerWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		hPickerRWnd = CreateTrackbar(hWnd, COLOR_MIN, COLOR_MAX, 80, 25, 200, HEIGHT_TRACKBAR);
		hPickerGWnd = CreateTrackbar(hWnd, COLOR_MIN, COLOR_MAX, 80, 25 + HEIGHT_TRACKBAR, 200, HEIGHT_TRACKBAR);
		hPickerBWnd = CreateTrackbar(hWnd, COLOR_MIN, COLOR_MAX, 80, 25 + HEIGHT_TRACKBAR + HEIGHT_TRACKBAR, 200, HEIGHT_TRACKBAR);

		hPickerButtonWnd = CreateButton(hWnd, TEXT("Confirm"), 65, 25 + HEIGHT_TRACKBAR + HEIGHT_TRACKBAR + HEIGHT_TRACKBAR + 10, 100, 25);
		hPickerShuffleButtonWnd = CreateButton(hWnd, TEXT("Shuffle"), 175, 25 + HEIGHT_TRACKBAR + HEIGHT_TRACKBAR + HEIGHT_TRACKBAR + 10, 100, 25);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		
		HDC hdc =
		BeginPaint(hWnd, &ps);
		{
			const TCHAR* szText = TEXT("Color R:");
			int uSize = (int) wcslen(szText);
			TextOut(hdc, 25, 30, szText, uSize);

			szText = TEXT("Color G:");
			TextOut(hdc, 25, 30 + HEIGHT_TRACKBAR, szText, uSize);

			szText = TEXT("Color B:");
			TextOut(hdc, 25, 30 + HEIGHT_TRACKBAR + HEIGHT_TRACKBAR, szText, uSize);

			TCHAR buffer[6];
			int progress = (int) SendMessage(hPickerRWnd, TBM_GETPOS, 0, 0);
			int count = progress < 10 ? 1 : progress < 100 ? 2 : 3;
			_itow(progress, buffer, 10);
			buffer[count] = 0;
			TextOut(hdc, 280, 30, buffer, count);

			progress = (int) SendMessage(hPickerGWnd, TBM_GETPOS, 0, 0);
			count = progress < 10 ? 1 : progress < 100 ? 2 : 3;
			_itow(progress, buffer, 10);
			buffer[count] = 0;
			TextOut(hdc, 280, 30 + HEIGHT_TRACKBAR, buffer, count);

			progress = (int) SendMessage(hPickerBWnd, TBM_GETPOS, 0, 0);
			count = progress < 10 ? 1 : progress < 100 ? 2 : 3;
			_itow(progress, buffer, 10);
			buffer[count] = 0;
			TextOut(hdc, 280, 30 + HEIGHT_TRACKBAR + HEIGHT_TRACKBAR, buffer, count);
		}
		EndPaint(hWnd, &ps);

		break;
	}
	case WM_HSCROLL:
	case WM_VSCROLL:
	{
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_COMMAND:
	{
		COLOR_R = (int) SendMessage(hPickerRWnd, TBM_GETPOS, 0, 0);
		COLOR_G = (int) SendMessage(hPickerGWnd, TBM_GETPOS, 0, 0);
		COLOR_B = (int) SendMessage(hPickerBWnd, TBM_GETPOS, 0, 0);

		if (HIWORD(wParam) == BN_CLICKED)
		{
			if ((HWND) lParam == hPickerButtonWnd)
			{
				DestroyWindow(hWnd);
			}
			else if ((HWND) lParam == hPickerShuffleButtonWnd)
			{
				SendMessage(hPickerRWnd, TBM_SETPOS, TRUE, (LPARAM) RandomInRange(COLOR_MIN, COLOR_MAX));
				SendMessage(hPickerGWnd, TBM_SETPOS, TRUE, (LPARAM) RandomInRange(COLOR_MIN, COLOR_MAX));
				SendMessage(hPickerBWnd, TBM_SETPOS, TRUE, (LPARAM) RandomInRange(COLOR_MIN, COLOR_MAX));

				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE || wParam == 'Q')
		{
			exit(EXIT_SUCCESS);
		}
		break;
	}
	case WM_CLOSE:
	case WM_QUIT:
	{
		exit(EXIT_SUCCESS);
		break;
	}
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);	
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE || wParam == 'Q')
		{
			DestroyWindow(hWnd);
		}
		break;
	}
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
	{
		PostQuitMessage(EXIT_SUCCESS);
		break;
	}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND WINAPI CreateTrackbar
(
	HWND hParentWnd,
	INT uValueMin, INT uValueMax,
	UINT uPosX, UINT uPosY,
	UINT uSizeX, UINT uSizeY
)
{
	HWND hWnd = CreateWindowEx
	(
		0,
		TRACKBAR_CLASS,
		TEXT("Trackbar Control"),
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
		uPosX, uPosY,
		uSizeX, uSizeY,
		hParentWnd,
		(HMENU) 41231,
		hAppInst,
		NULL
	);

	if (NULL == hWnd)
	{
		MessageBox
		(
			NULL,
			TEXT("Failed to create a trackbar!"),
			TEXT("Internal Error"),
			MB_OK | MB_ICONHAND
		);
		exit(EXIT_FAILURE);
	}

	SendMessage(hWnd, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(uValueMin, uValueMax));
	SendMessage(hWnd, TBM_SETPAGESIZE, 0, (LPARAM) 4);
	SendMessage(hWnd, TBM_SETSEL, (WPARAM) FALSE, (LPARAM) MAKELONG(uValueMin, uValueMax));
	SendMessage(hWnd, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) uValueMin);

	return hWnd;
}

HWND WINAPI CreateButton
(
	HWND hParentWnd,
	const TCHAR* pText,
	UINT uPosX, UINT uPosY,
	UINT uSizeX, UINT uSizeY
)
{
	HWND hWnd = CreateWindow
	(
		CLASS_BUTTON_WINDOW,
		pText,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		uPosX, uPosY,
		uSizeX, uSizeY,
		hParentWnd,
		NULL,
		hAppInst,
		NULL
	);

	if (NULL == hWnd)
	{
		MessageBox
		(
			NULL,
			TEXT("Failed to create a button!"),
			TEXT("Internal Error"),
			MB_OK | MB_ICONHAND
		);
		exit(EXIT_FAILURE);
	}

	return hWnd;
}
