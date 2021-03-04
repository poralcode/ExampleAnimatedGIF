# ExampleAnimatedGIF
A Win32API example for displaying image such as PNG, JPEG, and animated GIF using GDI+ library written in C++.

### Features

- Load image from file.
- Load image from resources.
- Automatically animate GIF image.
- Compatible with JPEG, JPG, BITMAP, GIF, PNG.

Create a Header file **GDIHelper.h**.

    #pragma once
    
    #ifndef GDIHELPER_H
    #define GDIHELPER_H
    #include <windows.h>			//GDI+ include in order (1) 
    #include <tchar.h>
    #include <string>
    #include <objidl.h>				//GDI+ include in order (2)
    #include <gdiplus.h>			//GDI+ include in order (3)
    using namespace std;
    using namespace Gdiplus;			//GDI+ include in order (4)
    #pragma comment (lib,"Gdiplus.lib")		//GDI+ include in order (5)
    #pragma comment(lib, "dwmapi.lib")		//GDI+ include in order (6)

     /** Don't forget to initialize the GDI+ **/
    
    class GDIHelper {
    
    public:
    	GDIHelper();
    	void LoadImageFromResource(HWND hWnd, UINT_PTR timer_id, HMODULE hMod, const wchar_t* resid, const wchar_t* restype);
    	void LoadImageFromFile(HWND hWnd, UINT_PTR timer_id, string file_location, const wchar_t* restype);
    	void OnTimer();
    	void Desrtroy();
    	void Stop(UINT_PTR timer_id);
    	void DrawItem(HDC hdc, int xPosition, int yPosition, int width, int height);
    	
    private:
    	void InitializeImage();
    	bool IsFileExist(string file_name);
    	void AnimateGIF();
    	HWND hwnd;
    	Image* m_pImage;
    	GUID* m_pDimensionIDs;
    	UINT m_FrameCount;
    	PropertyItem* m_pItem;
    	UINT m_iCurrentFrame;
    	UINT_PTR timer_id;
    	BOOL m_bIsPlaying;
    	BOOL isPlayable;
    };
    
    #endif

Create a CPP file **GDIHelper.cpp**.
    #include "GDIHelper.h"
    
    /** GDIHelper is a class helper to display images and animated GIF **/
    GDIHelper::GDIHelper() {
        timer_id = 0;
        m_FrameCount = 0;
        m_iCurrentFrame = 0;
        m_pImage = NULL;
        m_pDimensionIDs = NULL;
        m_pItem = NULL;
        hwnd = NULL;
        m_bIsPlaying = FALSE;
        isPlayable = FALSE;
    }
    
    /** Function to destroy objects and arrays, call this function on WM_DESTROY of WinProc. **/
    void GDIHelper::Desrtroy() {
        if(m_pDimensionIDs) {
            delete[] m_pDimensionIDs;
        }
    
        if(m_pItem) {
            free(m_pItem);
        }
    
        if(m_pImage) {
            delete m_pImage;
        }
    }
    
    /** Function to stop the animation of GIF. **/
    void GDIHelper::Stop(UINT_PTR ttimer_id) {
        if(m_bIsPlaying == FALSE) {
            return;
        }
    
        KillTimer(hwnd, ttimer_id);
        m_bIsPlaying = FALSE;
    }
    
    
    /** Functon to load the next frame of GIF, must be call on WM_TIMER. **/
    void GDIHelper::OnTimer() {
        if(isPlayable) {
            KillTimer(hwnd, timer_id);
            GUID Guid = FrameDimensionTime;
            m_pImage->SelectActiveFrame(&Guid, m_iCurrentFrame);
            SetTimer(hwnd, 120, ((UINT*)m_pItem[0].value)[m_iCurrentFrame] * 10, NULL);
            m_iCurrentFrame = (++m_iCurrentFrame) % m_FrameCount;
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }
    
    /** Private function, call this to animate the GIF image, should be call before drawing the image usually on WM_PAINT. **/
    void GDIHelper::AnimateGIF() {
        if(m_bIsPlaying == TRUE) {
            return;
        }
        if(isPlayable) {
            m_iCurrentFrame = 0;
            GUID Guid = FrameDimensionTime;
            m_pImage->SelectActiveFrame(&Guid, m_iCurrentFrame);
            SetTimer(hwnd, 120, ((UINT*)m_pItem[0].value)[m_iCurrentFrame] * 10, NULL);
            ++m_iCurrentFrame;
            InvalidateRect(hwnd, NULL, FALSE);
            m_bIsPlaying = TRUE;
        }
    }
    /** Function to draw the image in Window, must be call on WM_PAINT. **/
    void GDIHelper::DrawItem(HDC hdc, int xPosition, int yPosition, int width, int height) {
        AnimateGIF(); //This will only works if the image has more than one frame.
        Graphics g(hdc);
        g.DrawImage(m_pImage, xPosition, yPosition, width, height);
    }
    
    
    /** Private function, accessible only in this class, check if file exist. **/
    bool GDIHelper::IsFileExist(string file_name) {
        struct stat buffer;
        return (stat(file_name.c_str(), &buffer) == 0);
    }
    
    /** Private function, function to count and get the frame of image. **/
    void GDIHelper::InitializeImage() {
        UINT count = m_pImage->GetFrameDimensionsCount();
        m_pDimensionIDs = new GUID[count];
        m_pImage->GetFrameDimensionsList(m_pDimensionIDs, count);
    
        WCHAR strGuid[39];
        (void)StringFromGUID2(m_pDimensionIDs[0], strGuid, 39);
        m_FrameCount = m_pImage->GetFrameCount(&m_pDimensionIDs[0]);
    
        if(m_FrameCount > 1) { //frame of GIF is more than one, all good, we don't want the error of `Access violation reading location`
            isPlayable = TRUE;
            OutputDebugString(_T("ERROR: GDIHelper::Load >> GIF file has only 1 frame, its not playable.\n"));
        }
    
        UINT TotalBuffer = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
        m_pItem = (PropertyItem*)malloc(TotalBuffer);
        m_pImage->GetPropertyItem(PropertyTagFrameDelay, TotalBuffer, m_pItem);
    }
    
    /** Function to Load Image from Local File. **/
    void GDIHelper::LoadImageFromFile(HWND hWnd, UINT_PTR ttimer_id, string file_name, const wchar_t* restype) {
        hwnd = hWnd;
        timer_id = ttimer_id;
        
        if(!IsFileExist(file_name)) {
            OutputDebugString(_T("ERROR: GDIHelper::LoadImageFromFile >> Invalid file or not exist\n"));
            return; 
        }
        std::wstring widestr = std::wstring(file_name.begin(), file_name.end()); // Convert the string file_name to wstring.
        m_pImage = Image::FromFile(widestr.c_str()); //Convert the wtring to wchar and initialize.
        InitializeImage(); //Initialize the image.
    }
    
    /** Function to Load Image from Resources. **/
    void GDIHelper::LoadImageFromResource(HWND hWnd, UINT_PTR ttimer_id, HMODULE hMod, const wchar_t* resid, const wchar_t* restype) {
        hwnd = hWnd;
        timer_id = ttimer_id;
    
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
    
                    // copy image bytes into a real hglobal memory handle
                    hGlobal = ::GlobalAlloc(GHND, dwResourceSize);
                    if(hGlobal) {
                        void* pBuffer = ::GlobalLock(hGlobal);
                        if(pBuffer) {
                            memcpy(pBuffer, imagebytes, dwResourceSize);
                            HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
                            if(SUCCEEDED(hr)) {
                                // pStream now owns the global handle and will invoke GlobalFree on release
                                hGlobal = nullptr;
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
    
        m_pImage = pBmp; //Set image from resources.
        InitializeImage(); //Initialize the image.
    }
    

Don't forget to initialize the GDI+ library, for example in your **WinMain**.


    int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	
			// Initialize GDI+.
			GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR           gdiplusToken;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			//Some of your code here...

			GdiplusShutdown(gdiplusToken); //dont forget to shut it down.
			return (int)msg.wParam;
	}

Then on your **WndProc**,  you can load from a file or resources and draw the image, for example.

    	#define ID_OF_YOUR_TIMER 123456
	
        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        GDIHelper gdiHelper; //Initialize the GDIHelper you created.
        switch(message) {
    	
        case WM_CREATE: {
    		/** Load image from local file. **/
            //gdiHelper.LoadImageFromFile(hWnd, ID_OF_YOUR_TIMER, "C:\\Users\\archd\\Downloads\\spinner.gif", L"GIF");
    		
    		/** Load image from resources. **/
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
    
 

**IDE**
Visual Studio 2019 Enterprise

**Language**
C++
