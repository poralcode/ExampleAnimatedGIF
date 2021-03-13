# ExampleAnimatedGIF
A Win32API example for displaying image such as PNG, JPEG, and animated GIF using GDI+ library written in C++.

![Example-Output-GIF](https://github.com/SeynalKim/ExampleAnimatedGIF/blob/main/Example.gif)

### Features

- Load image from file.
- Load image from resources.
- Automatically animate GIF image.
- Compatible with JPEG, JPG, BITMAP, GIF, PNG.
- Non-Blocking UI animation.

## How to use
You can use the `DisplayImageFromFile()` function to display image from local file.

	/* Display the image from local file (1)*/
        gdiHelper.DisplayImageFromFile(
            "C:\\Users\\archd\\Downloads\\tsaw_spinner_original.gif", //File location
            hWnd,                                                     //Handle to the Window
            YOUR_UNIQUE_ID,                                           //Unique ID of your control, declare your own.
            165,                                                      //xPosition
            100,                                                      //yPosition
            95,                                                       //width 
            95                                                        //height
        );

Or `DisplayImageFromResource()` to display the image from your resources. 

	/* Display the image from resources (1)*/
        gdiHelper.DisplayImageFromResource(
            GetModuleHandle(NULL),                       
            MAKEINTRESOURCE(ID_OF_YOUR_RESOURCE_FILE),   //ID of your image from resource file
            L"GIF",                                      //Type of image, PNG, GIF, JPEG
            hWnd,                                        //Handle to the Window
            YOUR_UNIQUE_ID,                              //Unique ID of your control, declare your own.
            165,                                         //xPosition
            100,                                         //yPosition
            95,                                          //width 
            95                                           //height
        );
	
Displaying image uses thread, so make sure to call `Destroy()` after using it to stop the thread.

	gdiHelper.Destroy(); 

### The Code
Create a Header file and name it **GDIHelper.h**.

	#pragma once
	#ifndef GDIHELPER_H
	#define GDIHELPER_H
	
	#include <windows.h>					//GDI+ include in order (1) 
	#include <commctrl.h>
	#include <tchar.h>
	#include <thread>
	#include <chrono>
	#include <string>
	#include <objidl.h>					//GDI+ include in order (2)
	#include <gdiplus.h>					//GDI+ include in order (3)
	using namespace std;
	using namespace Gdiplus;				//GDI+ include in order (4)
	#pragma comment (lib,"Gdiplus.lib")			//GDI+ include in order (5)
	#pragma comment(lib, "dwmapi.lib")			//GDI+ include in order (6)
	#pragma comment(lib, "comctl32.lib")

	class GDIHelper {

	public:
		GDIHelper();
		void DisplayImageFromResource(HMODULE hMod, const wchar_t* resid, const wchar_t* restype, HWND hWnd, UINT_PTR unique_id, int xPosition, int yPosition, int width, int hheight);
		void DisplayImageFromFile(string file_location, HWND hWnd, UINT_PTR unique_id, int xPosition, int yPosition, int width, int height);
		void Destroy();

	private:
		static LRESULT CALLBACK StaticControlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
		void GetImageFrame();
		void SetImage(int uunique_id, int xxPosition, int yyPosition, int wwidth, int hheight, Image* image, HWND hwnd);
		bool IsFileExist(string file_name);
		static void run();

		static HWND staticControl;
		static Image* m_pImage;
		static GUID* m_pDimensionIDs;
		static UINT m_FrameCount;
		static PropertyItem* m_pItem;
		static UINT m_iCurrentFrame;
		static UINT_PTR unique_id;
		static BOOL m_bIsPlaying;
		static BOOL isPlayable;
		static int xPosition, yPosition, width, height, animation_duration;
	};

	#endif

Create a CPP file and name it **GDIHelper.cpp**.
    
    	#include "GDIHelper.h" //include the GDIHelper.h you created.

	HWND GDIHelper::staticControl = NULL;
	Image* GDIHelper::m_pImage = NULL;
	GUID* GDIHelper::m_pDimensionIDs = NULL;
	UINT GDIHelper::m_FrameCount = 0;
	PropertyItem* GDIHelper::m_pItem = NULL;
	UINT GDIHelper::m_iCurrentFrame = 0;
	UINT_PTR GDIHelper::unique_id = 0;
	BOOL GDIHelper::m_bIsPlaying = FALSE;
	BOOL GDIHelper::isPlayable = FALSE;
	int GDIHelper::xPosition = 0;
	int GDIHelper::yPosition = 0;
	int GDIHelper::width = 0;
	int GDIHelper::height = 0;
	int GDIHelper::animation_duration = 0;

	/** GDIHelper is a class helper to display images and animated GIF **/
	GDIHelper::GDIHelper() {}

	/** Function to destroy objects and arrays, call this function on WM_DESTROY of WinProc. **/
	void GDIHelper::Destroy() {
	    if(m_pDimensionIDs) {
		delete[] m_pDimensionIDs;
	    }

	    if(m_pItem) {
		free(m_pItem);
	    }

	    if(m_pImage) {
		delete m_pImage;
	    }
	    m_bIsPlaying = FALSE;
	    isPlayable = FALSE;
	    RemoveWindowSubclass(staticControl, &StaticControlProc, unique_id);
	}

	/** Private function, call this function as thread to animate the GIF image. **/
	void GDIHelper::run() {
	    if(m_bIsPlaying == TRUE) {
		return;
	    }
	    m_iCurrentFrame = 0;
	    GUID Guid = FrameDimensionTime;
	    m_pImage->SelectActiveFrame(&Guid, m_iCurrentFrame);
	    ++m_iCurrentFrame;
	    m_bIsPlaying = TRUE;
	    animation_duration = ((UINT*)m_pItem[0].value)[m_iCurrentFrame] * 10;

	    while(isPlayable) {
		std::this_thread::sleep_for(std::chrono::milliseconds(animation_duration));
		m_pImage->SelectActiveFrame(&Guid, m_iCurrentFrame);
		m_iCurrentFrame = (++m_iCurrentFrame) % m_FrameCount;
		InvalidateRect(staticControl, NULL, FALSE);
		UpdateWindow(staticControl);
	    }
	}

	/** Private function, accessible only in this class, check if file exist. **/
	bool GDIHelper::IsFileExist(string file_name) {
	    struct stat buffer;
	    return (stat(file_name.c_str(), &buffer) == 0);
	}

	/** Private function, function to count and get the frame of image. **/
	void GDIHelper::GetImageFrame() {
	    UINT count = m_pImage->GetFrameDimensionsCount();
	    m_pDimensionIDs = new GUID[count];
	    m_pImage->GetFrameDimensionsList(m_pDimensionIDs, count);

	    m_FrameCount = m_pImage->GetFrameCount(&m_pDimensionIDs[0]);

	    UINT TotalBuffer = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
	    m_pItem = (PropertyItem*)malloc(TotalBuffer);
	    m_pImage->GetPropertyItem(PropertyTagFrameDelay, TotalBuffer, m_pItem);

	    if(m_FrameCount > 1) {  // frame of GIF is more than one, all good, we don't want the error of `Access violation reading location`
		OutputDebugString(_T("NOTICED: GDIHelper::InitializeImage >> Image file has more than 1 frame, its playable (2).\n"));
		isPlayable = TRUE;  // is playable
		std::thread t(run); // Start the animation as thread.
		t.detach();         // this will be non-blocking thread.
	    }
	}

	LRESULT CALLBACK GDIHelper::StaticControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	    switch(uMsg) {
		case WM_PAINT:{
		    PAINTSTRUCT ps;
		    HDC hdc = BeginPaint(hwnd, &ps);

		    Graphics g(hdc);
		    g.DrawImage(m_pImage, 0, 0, width, height);

		    EndPaint(hwnd, &ps);
		    return TRUE; 
		}
	    }
	   return DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}

	/** Function to set the image and initialize all required variables. **/
	void GDIHelper::SetImage(int uunique_id, int xxPosition, int yyPosition, int wwidth, int hheight, Image* image, HWND hwnd) {
	    unique_id = uunique_id;
	    xPosition = xxPosition;
	    yPosition = yyPosition;
	    width = wwidth;
	    height = hheight;

	    staticControl = CreateWindowEx(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, xPosition, yPosition, width, height, hwnd, NULL, NULL, NULL); //create the static control.
	    SetWindowSubclass(staticControl, &StaticControlProc, unique_id, 0);

	    m_pImage = image;
	    GetImageFrame(); //Initialize the image.
	}

	/** Function to Display Image from Local File. **/
	void GDIHelper::DisplayImageFromFile(string file_name, HWND hWnd, UINT_PTR uunique_id, int xxPosition, int yyPosition, int wwidth, int hheight) {
	    if(!IsFileExist(file_name)) {
		OutputDebugString(_T("ERROR: GDIHelper::LoadImageFromFile >> Invalid file or not exist\n"));
		return; 
	    }

	    std::wstring widestr = std::wstring(file_name.begin(), file_name.end()); // Convert the string file_name to wstring.
	    SetImage(uunique_id, xxPosition, yyPosition, wwidth, hheight, Image::FromFile(widestr.c_str()), hWnd); //Set image and Control
	}

	/** Function to Load Image from Resources. **/
	void GDIHelper::DisplayImageFromResource(HMODULE hMod, const wchar_t* resid, const wchar_t* restype, HWND hWnd, UINT_PTR uunique_id, int xxPosition, int yyPosition, int wwidth, int hheight) {

	    IStream* pStream = nullptr;
	    Gdiplus::Bitmap* pBmp = nullptr;
	    HGLOBAL hGlobal = nullptr;

	    HRSRC hrsrc = FindResourceW(GetModuleHandle(NULL), resid, restype);     // get the handle to the resource
	    if(hrsrc) {
		DWORD dwResourceSize = SizeofResource(hMod, hrsrc);
		if(dwResourceSize > 0) {
		    HGLOBAL hGlobalResource = LoadResource(hMod, hrsrc); // load it
		    if(hGlobalResource) {
			void* imagebytes = LockResource(hGlobalResource); // get a pointer to the file bytes

			hGlobal = ::GlobalAlloc(GHND, dwResourceSize); // copy image bytes into a real hglobal memory handle
			if(hGlobal) {
			    void* pBuffer = ::GlobalLock(hGlobal);
			    if(pBuffer) {
				memcpy(pBuffer, imagebytes, dwResourceSize);
				HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
				if(SUCCEEDED(hr)) {
				    hGlobal = nullptr; // pStream now owns the global handle and will invoke GlobalFree on release
				    pBmp = new Gdiplus::Bitmap(pStream);
				}
			    }
			}
		    }
		}
	    }

	    if(pStream) {
		pStream->Release();
		pStream = nullptr;
	    }

	    if(hGlobal) {
		GlobalFree(hGlobal);
	    }

	    SetImage(uunique_id, xxPosition, yyPosition, wwidth, hheight, pBmp, hWnd);
	}
    

Don't forget to initialize the GDI+ library, for example in your **WinMain**.


    int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//Some of your code here...

	GdiplusShutdown(gdiplusToken); //dont forget to shut down the GID+ token.
	return (int)msg.wParam;
    }

Then on your **WndProc**,  you can load from a file or resources to draw the image, for example.

    	#define YOUR_UNIQUE_ID 12345678
	
        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        GDIHelper gdiHelper; //Initialize the GDIHelper you created.
		switch(message) {
			case WM_CREATE: {
			    /* Display the image from local file (1)*/
				gdiHelper.DisplayImageFromFile(
				    "C:\\Users\\archd\\Downloads\\tsaw_spinner_original.gif", //File location
				    hWnd,                                                     //Handle to the Window
				    YOUR_UNIQUE_ID,                                           //Unique ID of your control, declare your own.
				    165,                                                      //xPosition
				    100,                                                      //yPosition
				    95,                                                       //width 
				    95                                                        //height
				);

			    /* OR Display the image from resources (1)*/
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
			}case WM_PAINT:{
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
			}case WM_DESTROY:{
			    	/* Don't forget to call destroy. (2) */
        			gdiHelper.Destroy(); 
			    	PostQuitMessage(0);
			    break;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	    }
    
 
 For more details, refer to the code of the project.

**IDE**
Visual Studio 2019 Enterprise

**Language**
C++, Win32Api, GDI+
