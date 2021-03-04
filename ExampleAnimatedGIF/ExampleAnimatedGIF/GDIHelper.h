#pragma once

#ifndef GDIHELPER_H
#define GDIHELPER_H
#include "Header.h"

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
