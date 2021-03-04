#include "resource.h"
#include "GDIHelper.h"
#include "Header.h"

static TCHAR szWindowClass[] = _T("ExampleAnimatedGIF");
static TCHAR szTitle[] = _T("Example Animated GIF");
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define ID_OF_YOUR_TIMER 5576

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    // Initialize GDI+.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if(!RegisterClassEx(&wcex)) {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), szTitle, NULL);
        return 1;
    }

    hInst = hInstance;
    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 0, 0, 450, 500, NULL, NULL, hInstance, NULL);

    //Center the Window
    RECT rc;
    GetWindowRect(hWnd, &rc);
    int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
    int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;
    SetWindowPos(hWnd, 0, xPos, yPos, 450, 500, SWP_NOZORDER);

    if(!hWnd) {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), szTitle, NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken); //dont forget to shut it down.
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    GDIHelper gdiHelper;
    switch(message) {
    case WM_ACTIVATE:
    {
        break;
    }
    case WM_COMMAND:{
        break;
    }
    case WM_CREATE: {
        //gdiHelper.LoadImageFromFile(hWnd, ID_OF_YOUR_TIMER, "C:\\Users\\archd\\Downloads\\spinner.gif", L"GIF");
        gdiHelper.LoadImageFromResource(hWnd, ID_OF_YOUR_TIMER, GetModuleHandle(NULL), MAKEINTRESOURCE(ID_OF_YOUR_RESOURCE_FILE), L"GIF"); //(1)
        break;
    }
    case WM_TIMER: {
        gdiHelper.OnTimer(); //(2)
        break;
    }
    case WM_PAINT:
    {
        HDC hdc;
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);
       
        gdiHelper.DrawItem(hdc, 160, 150, 95, 95); //(3) hdc, xposition, yposition, width, height
       
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        gdiHelper.Desrtroy();
        PostQuitMessage(0);
        break;
    }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}