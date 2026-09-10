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
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "msimg32.lib")
ULONG_PTR gdiToken;
using namespace Gdiplus;
int live = 0;
int way = 1;
int ap = 0;
int tail = 0;
int tails[400][2] = { -1 , };
int W = 500;
int H = 470;
int score = 0;
static Image* gImg = nullptr;
static Image* apple = nullptr;
static Image* body = nullptr;
static float posX = 500, posY = 470;
static float scale = 1.0f;
const int WIN_W = 1000;
const int WIN_H = 940;
void move() {
    int imgW = static_cast<int>(gImg->GetWidth() * scale);
    int imgH = static_cast<int>(gImg->GetHeight() * scale);
    const int Wspeed = 50;
    const int Hspeed = 47;
    for (int i = tail; i >= 0; i--) {
        tails[i][0] = tails[i - 1][0];
        tails[i][1] = tails[i - 1][1];
        if (posX == tails[i][0] && posY == tails[i][1]) {
            if (posX == 0 && posY == 0)
            {

            }
            else
            {
            live = 1;
            }
        }
        if (W == tails[i][0] && H == tails[i][1]) {
            ap = 0;
        }
    }
    tails[0][0] = posX;
    tails[0][1] = posY;

    switch (way) {
    case 1: posX -= Wspeed; break; 
    case 2: posX += Wspeed; break;
    case 3: posY -= Hspeed; break;
    case 4: posY += Hspeed; break;
    }

    if (W == posX && H == posY)
    {
        ap = 0;
        tail++; 
    }
    if (posX < 0) live = 1;
    if (posY < 0) live = 1;
    if (posX + imgW > WIN_W) posX = WIN_W - imgW, live = 1;
    if (posY + imgH > WIN_H) posY = WIN_H - imgH, live = 1;
    if (posX < 0 || posY < 0 || posX + imgW > WIN_W || posY + imgH > WIN_H) {
        live = 1; 
    }
}
void MoveApple(WPARAM key) {
    switch (key) {
    case VK_LEFT:  if (way == 2) {} else way = 1; break;
    case VK_RIGHT: if (way == 1) {} else way = 2; break;
    case VK_UP:    if (way == 4) {} else way = 3; break;
    case VK_DOWN:  if (way == 3) {} else way = 4; break;
    }
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HDC memDC = nullptr;
    static HBITMAP memBMP = nullptr;
    static HBITMAP oldBMP = nullptr;
    if (ap == 0)
    {
        srand(time(NULL));
        W = (rand() % 20) * 50;
        H = (rand() % 20) * 47;
        ap = 1;
    }
    switch (msg) {
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
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Graphics g(memDC);
        Image image(L"tile.png");
        TextureBrush tBrush(&image);
        Pen blackPen(Color(255, 0, 0, 0));
        int stat = g.FillRectangle(&tBrush, Rect(0, 0, WIN_W, WIN_H));
        int stat2 = g.DrawRectangle(&blackPen, Rect(0, 0, WIN_W, WIN_H));
        int drawW = static_cast<int>(gImg->GetWidth() * scale);
        int drawH = static_cast<int>(gImg->GetHeight() * scale);
        g.DrawImage(apple, W, H, drawW, drawH);
        g.DrawImage(gImg, (int)posX, (int)posY, drawW, drawH);
        for (int cnt = 0; cnt < 400; cnt++) {
            if (cnt >= tail)
            {
                break;
            }
            else
            {
                g.DrawImage(body, tails[cnt][0], tails[cnt][1], drawW, drawH);
            }
        }

        wchar_t buffer[16];

        _itow_s(tail, buffer, 16, 10); 

        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);
        SolidBrush redBrush(Color(255, 255, 0, 0));
        PointF drawPoint(0.0f, 0.0f);
        g.DrawString(
            buffer,     // 변환된 숫자 문자열
            -1,         // 문자열 길이 (-1은 null 종료 문자열을 의미)
            &font,      // 사용할 폰트
            drawPoint,  // 시작 위치
            &redBrush   // 사용할 브러시 (색상)
        );
        BitBlt(hdc, 0, 0, 1000, 1000, memDC, 0, 0, SRCCOPY);
        CreateCompatibleDC(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_KEYDOWN:
        MoveApple(wParam);
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    case WM_TIMER: {
        move();
        InvalidateRect(hWnd, nullptr, FALSE);
        return 0;
    }
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        if (memDC) {
            SelectObject(memDC, oldBMP);
            DeleteObject(memBMP);
            DeleteDC(memDC);
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken = 0;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    gImg = Image::FromFile(L"head.png");
    apple = Image::FromFile(L"apple.png");
    body = Image::FromFile(L"body.png");
    if (!gImg || gImg->GetLastStatus() != Ok) {
        MessageBoxW(nullptr, L"apple.png를 찾을 수 없습니다.", L"Error", MB_ICONERROR);
        return 1;
    }

    posX = (WIN_W - gImg->GetWidth() * scale) / 1.0f;
    posY = (WIN_H - gImg->GetHeight() * scale) / 1.0f;

    const wchar_t* kClass = L"snake";
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = kClass;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);
    HWND hWnd = CreateWindowW(
    kClass, L"snake",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    1015, 980,
    nullptr, nullptr, hInst, nullptr);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    InvalidateRect(hWnd, nullptr, TRUE);
    UpdateWindow(hWnd);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        if( live == 1)
        {
            break;
        }
    }

    delete gImg;
    GdiplusShutdown(gdiplusToken);
    return 0;
}