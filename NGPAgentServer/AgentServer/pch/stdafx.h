// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// C++ 런타임 헤더 파일입니다.
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>
#include <functional>
#include <string>
using namespace std;

#include <filesystem>
using namespace experimental::filesystem;

// network
#pragma comment(lib, "ws2_32")
#include <winsock2.h>

// ComPtr
#include <wrl.h>
using namespace Microsoft::WRL;

// Direct2D
#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwrite")
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>

#include "D2DOperator.h"
using namespace D2D1;

// Bitmap
#include "D2DLoadBitmap.h"
using namespace D2DBitmap;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include "SystemDefine.h"
#include "inlineFuncs.h"
