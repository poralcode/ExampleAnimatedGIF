#pragma once

#ifndef GDIHELPER_H
#define GDIHELPER_H
#include "Header.h"

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
