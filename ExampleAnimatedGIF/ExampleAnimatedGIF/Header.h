#include <stdexcept>					//For throwing exceptions
#include <windows.h>					//GDI+ include in order (1) 
#include <tchar.h>
#include <string>
#include <objidl.h>						//GDI+ include in order (2)
#include <gdiplus.h>					//GDI+ include in order (3)
using namespace std;
using namespace Gdiplus;				//GDI+ include in order (4)
#pragma comment (lib,"Gdiplus.lib")		//GDI+ include in order (5)
#pragma comment(lib, "dwmapi.lib")		//GDI+ include in order (6)

/** Don't forget to initialize the GDI+ **/
