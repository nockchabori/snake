#include <windows.h>
#include <gdiplus.h>
#include <gdiplusbrush.h>
#include <Gdiplusimaging.h>
#include <gdiplusheaders.h>
#include <gdiplusgraphics.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <winuser.h>
#include "framework.h"
#include "WindowsProject3.h"
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "msimg32.lib")
ULONG_PTR gdiToken = 0;
using namespace Gdiplus;
int speed = 50;
static int posX = 500, posY = 500;
const int WIN_H = 1000, WIN_W = 1000;
static Image* character = nullptr;
static Image* map = nullptr;
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken = 0;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    character = Image::FromFile(L"mario.png");
    map = Image::FromFile(L"tile.png");
    if (!character || character->GetLastStatus() != Ok)
    {
        // 로드 실패 시 NULL 포인터 대신 GDI+ 종료 및 오류 반환
        GdiplusShutdown(gdiToken);
        return FALSE;
    }
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT3));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


void move(int key) {
    switch (key) {
    case VK_LEFT:posX -= speed;break;
    case VK_RIGHT:posX += speed;break;
    case VK_UP:posY -= speed;break;
    case VK_DOWN:posY += speed;break;
    }
    if (posX < 0) posX = 0;
    if (posY < 0) posY = 0;
    if (posX + speed > WIN_W) posX = WIN_W - speed;
    if (posY + speed > WIN_H) posY = WIN_H - speed;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, WIN_W, WIN_H, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC memDC = nullptr;
    static HBITMAP memBMP = nullptr;
    static HBITMAP oldBMP = nullptr;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE: {
        HDC hdc = GetDC(hWnd);
        memDC = CreateCompatibleDC(hdc);
        memBMP = CreateCompatibleBitmap(hdc, WIN_W, WIN_H);
        oldBMP = (HBITMAP)SelectObject(memDC, memBMP);
        ReleaseDC(hWnd, hdc);
        UINT_PTR TIMER_ID = 1;
        UINT uElapse_ms = 100;
        SetTimer(hWnd, TIMER_ID, uElapse_ms, nullptr);
        return 0;
    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            Graphics g(memDC);
            g.Clear(Color(0, 0, 0, 255));
            int drawW = static_cast<int>(character->GetWidth() * 1.0f);
            int drawH = static_cast<int>(character->GetHeight() * 1.0f);
            Image   image(L"tile.jpg");
            TextureBrush  tBrush(&image);
            Pen     texturedPen(&tBrush, 30);
            g.DrawImage(&image, 100, 100, image.GetWidth(), image.GetHeight());
            g.DrawRectangle(&texturedPen, 100, 20, 200, 100);
            g.DrawImage(character, (int)posX, (int)posY, drawW, drawH);
            BitBlt(hdc, 0, 0, WIN_W, WIN_H, memDC, 0, 0, SRCCOPY);
            CreateCompatibleDC(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_KEYDOWN:
        move(wParam);
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    case WM_TIMER: {
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    }
    case WM_DESTROY:
        KillTimer(hWnd, 1); 
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
