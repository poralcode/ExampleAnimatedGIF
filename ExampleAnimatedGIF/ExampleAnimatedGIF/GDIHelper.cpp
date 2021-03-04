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
