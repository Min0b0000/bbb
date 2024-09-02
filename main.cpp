// bbb.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "bbb.h"

#include "Game/Game.h"
#include <crtdbg.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE   hInst = nullptr;                // current instance
HWND        g_hMainWindow = nullptr;
WCHAR       szTitle[MAX_LOADSTRING];        // The title bar text
WCHAR       szWindowClass[MAX_LOADSTRING];  // the main window class name
CGame*      g_pGame = nullptr;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_      HINSTANCE hInstance,
                     _In_opt_   HINSTANCE hPrevInstance,
                     _In_       LPWSTR    lpCmdLine,
                     _In_       int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BBB, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    g_hMainWindow = InitInstance(hInstance, nCmdShow);
    if (!g_hMainWindow)
    {
        return FALSE;
    }

    MSG     msg = {};

    g_pGame = new CGame;
    if (!g_pGame->Initialize(g_hMainWindow))
    {
        goto lb_exit;
    }
    
    g_pGame->CreatePlayer(0, 0);

    g_pGame->CreateVirtualResolution(320, 180);
    // g_pGame->DeleteVirtualResolution();

    // Main message loop:
    for (;;)
    {        
        BOOL bMsg = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (bMsg)
        {
            if (WM_QUIT == msg.message)
            {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else
        {
            g_pGame->Run();
        }
    }

lb_exit:
    if (g_pGame)
    {
        delete g_pGame;
        g_pGame = nullptr;
    }

    _ASSERT(_CrtCheckMemory());
    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BBB));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BBB);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd = NULL;
   hInst = hInstance; // Store instance handle in our global variable
   
   DWORD dwStyle = WS_OVERLAPPEDWINDOW;
   RECT rcWindowSize = { 0, 0, 1600, 900 };
   AdjustWindowRect(&rcWindowSize, dwStyle, 0);

   rcWindowSize.right   -= rcWindowSize.left;
   rcWindowSize.bottom  -= rcWindowSize.top;

   hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, 0, rcWindowSize.right, rcWindowSize.bottom, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
       return nullptr;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEMOVE:
        g_pGame->OnMouseMove(wParam, lParam);
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        g_pGame->OnKeyDown(message, wParam, lParam);
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        g_pGame->OnKeyUp(message, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        g_pGame->OnMouseButtonDown(message, wParam, lParam);
        break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        g_pGame->OnMouseButtonUp(message, wParam, lParam);
        break;
    case WM_MOUSEWHEEL:
        g_pGame->OnMouseWheel(message, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_MOVE:
        if (g_pGame)
        {
            g_pGame->UpdateWindowPos();
        } break;
    case WM_SIZE:
        if (g_pGame)
        {
            g_pGame->UpdateWindowSize();
        } break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}