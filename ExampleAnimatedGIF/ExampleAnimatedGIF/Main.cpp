#include "resource.h"
#include "GDIHelper.h"
#include "Header.h"

static TCHAR szWindowClass[] = _T("ExampleAnimatedGIF");
static TCHAR szTitle[] = _T("Example Animated GIF");
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define YOUR_UNIQUE_ID 5576

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

    GdiplusShutdown(gdiplusToken); //dont forget to shut down the gdi+ token.
    return (int)msg.wParam;
}

GDIHelper gdiHelper;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_ACTIVATE:
    {
        break;
    }
    case WM_COMMAND:{
        break;
    }
    case WM_CREATE: {

        /* Display the image from local file (1)*/
        gdiHelper.DisplayImageFromFile(
            "C:\\Users\\USERNAME\\location\\your_gif.gif", //File location
            hWnd,                                                     //Handle to the Window
            YOUR_UNIQUE_ID,                                           //Unique ID of your control, declare your own.
            165,                                                      //xPosition
            100,                                                      //yPosition
            95,                                                       //width 
            95                                                        //height
        );

        /* Display the image from resources (1)*/
        /*gdiHelper.DisplayImageFromResource(
            GetModuleHandle(NULL),                       
            MAKEINTRESOURCE(ID_OF_YOUR_RESOURCE_FILE),   //ID of your image from resource file
            L"GIF",                                      //Type of image
            hWnd,                                        //Handle to the Window
            YOUR_UNIQUE_ID,                              //Unique ID of your control, declare your own.
            165,                                         //xPosition
            100,                                         //yPosition
            95,                                          //width 
            95                                           //height
        ); */

        break;
    }
    case WM_PAINT:
    {
        HDC hdc;
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);

        //Paint other images and text here...
        Graphics graphics(hdc);
        SolidBrush  brush(Color(225, 3, 3, 3));
        FontFamily  fontFamily(L"Segoe UI");
        Font        font(&fontFamily, 14, FontStyleRegular, UnitPixel);
        PointF      pointF(78.0f, 220.0f);
        graphics.DrawString(L"This text is not affected by GIF animation.", -1, &font, pointF, &brush);
        //End of paint.
        
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        /* Don't forget to call destroy. (2) */
        gdiHelper.Destroy(); 
        PostQuitMessage(0);
        break;
    }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
    
}
