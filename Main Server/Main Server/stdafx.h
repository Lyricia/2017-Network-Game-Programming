// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib, "ws2_32")

#include "targetver.h"

#include <winsock2.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <list>

// ComPtr
#include <wrl.h>
using namespace Microsoft::WRL;

// Direct2D
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
using namespace D2D1;


#include "NGPmsg.h"

using namespace std;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
