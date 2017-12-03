#pragma once

constexpr LONG CLIENT_WIDTH = 1200;
constexpr LONG CLIENT_HEIGHT = 720;

constexpr LONG CLIENT_MINIMUM_WIDTH = 800;
constexpr LONG CLIENT_MINIMUM_HEIGHT = 600;

// 캡션 FPS 출력 여부 -------------------
// 항상 캡션에 FPS를 출력		(0 : 비활성 | 1 : 활성)
#define USE_CAPTIONFPS_ALWAYS	 1
#define USE_TITLE_STRING		 1
#define USE_LIMIT_MAX_FPS		 1
#define USE_CONSOLE_WINDOW		 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug에서는 항상 실행
#define SHOW_CAPTIONFPS 
#endif

#if USE_LIMIT_MAX_FPS
#define MAX_UPDATE_FPS 1.0 / 3.0
#else
#define MAX_UPDATE_FPS 0.0
#endif

#if USE_LIMIT_MAX_FPS
#define MAX_PROCESS_MSG_FPS 1.0 / 30.0
#else
#define MAX_UPDATE_FPS 0.0
#endif

// 최대 FPS
#if USE_LIMIT_MAX_FPS
#define MAX_FPS 1.0 / 144.0
#else
#define MAX_FPS 0.0
#endif

#define TITLE_MAX_LENGTH 64

#if USE_TITLE_STRING
#define TITLESTRING	TEXT("NGPClient")
#endif

#define DEBUG_CLIENT_WIDTH	400
#define DEBUG_CLIENT_HEIGHT	600

#define UNIT_SIZE			64.f