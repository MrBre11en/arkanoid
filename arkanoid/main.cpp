// arkanoid.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "arkanoid.h"
#include <algorithm>
#include "windows.h"

#define MAX_LOADSTRING 100


//////////////////////////////////////////////////////////////////////////////

struct vec2 {
    float x, y;

    vec2(float X = 0.0f, float Y = 0.0f)
        : x(X), y(Y)
    {}
};

struct vec2int {
    int x, y;

    vec2int(int X = 0.0f, int Y = 0.0f)
        : x(X), y(Y)
    {}
};

//////////////////////////////////////////////////////////////////////////////


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

POINT MousePos;
bool started;

const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

struct {
    int radius = 15;

    vec2 pos;
    vec2 dir;
} ball;

struct {
    const int width = 200;
    const int height = 20;

    int xpos;
} platform;

namespace ginfo {
    const int platformYPos = screenHeight - platform.height / 2 - 25;
}

//////////////////////////////////////////////////////////////////////////////


void GetMousePosition()
{
    GetCursorPos(&MousePos);
}


void Update()
{
    platform.xpos = min(max(MousePos.x, platform.width / 2), window.width - platform.width / 2);

    if (started)
    {

    }
    else
    {
        ball.pos = vec2(platform.xpos, ginfo::platformYPos - platform.height / 2 - ball.radius);
    }
}


void DrawWindow(HDC hdc)
{
    RECT rect;
    HBRUSH hBrush;
    HPEN hPen;

    rect = { 0, 0, window.width, window.height };
    hBrush = CreateSolidBrush(RGB(50, 50, 50));
    FillRect(hdc, &rect, hBrush);

    rect = { platform.xpos - platform.width / 2, ginfo::platformYPos + platform.height / 2, platform.xpos + platform.width / 2, ginfo::platformYPos - platform.height / 2 };
    hBrush = CreateSolidBrush(RGB(255, 0, 0));
    FillRect(hdc, &rect, hBrush);

    hBrush = CreateSolidBrush(RGB(255, 0, 0));
    hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    Ellipse(hdc, ball.pos.x - ball.radius, ball.pos.y - ball.radius, ball.pos.x + ball.radius, ball.pos.y + ball.radius);

    DeleteObject(hBrush);
    DeleteObject(hPen);
}


//////////////////////////////////////////////////////////////////////////////


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ARKANOID, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ARKANOID));

    MSG msg;

    // Цикл основного сообщения:
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(window.hWnd, &ps);

        GetMousePosition();
        Update();
        DrawWindow(window.context);
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);

        EndPaint(window.hWnd, &ps);

        if (!TranslateAccelerator(window.hWnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Sleep(2);
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ARKANOID));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP | WS_VISIBLE,
      CW_USEDEFAULT, 0, screenWidth, screenHeight, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   started = false;

   window.hWnd = hWnd;
   window.device_context = GetDC(window.hWnd); //из хэндла окна достаем хэндл контекста устройства для рисования
   RECT r;
   GetClientRect(window.hWnd, &r);
   window.width = r.right - r.left; //определяем размеры и сохраняем
   window.height = r.bottom - r.top;
   window.context = CreateCompatibleDC(window.device_context); //второй буфер
   SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height)); //привязываем окно к контексту
   GetClientRect(window.hWnd, &r);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
        
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}