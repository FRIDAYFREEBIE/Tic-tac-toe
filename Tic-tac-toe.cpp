#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

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


    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

static int currentTurn = 1; // 1 = 1 2 = 2
static int cussor = 1;
static int board[9] = { 0 }; // 0: 비어있음
static int winner = 0;

// 0: 진행중, 1: 플레이어 1 승리, 2: 플레이어 2 승리
int Win() {
  int win[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
    {0, 4, 8}, {2, 4, 6}
  };

  for (int i = 0; i < 8; i++){
    int a = win[i][0];
    int b = win[i][1];
    int c = win[i][2];
    
    if (board[a] != 0 && board[a] == board[b] && board[b] == board[c]) {
      return board[a];
    }
  }

  return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    case WM_KEYDOWN:
      // 방향키 따라서 커서가 움직임
      switch (wParam) {
        case VK_LEFT:
          if (cussor % 3 != 1) cussor--;
          InvalidateRect(hWnd, NULL, TRUE);
          break;

        case VK_RIGHT:
          if (cussor % 3 != 0) cussor++;
          InvalidateRect(hWnd, NULL, TRUE);
          break;

        case VK_UP:
          if (cussor > 3) cussor -= 3;
          InvalidateRect(hWnd, NULL, TRUE);
          break;
        
        case VK_DOWN:
          if (cussor <= 6) cussor += 3;
          InvalidateRect(hWnd, NULL, TRUE);
          break;

        // 엔터를 누르면 누가 이겼는지 판단함
        case VK_RETURN:
          if (board[cussor - 1] == 0 && winner == 0) {
            board[cussor - 1] = currentTurn;

            winner = Win();

            // 진행중
            if (winner == 0) {
              currentTurn = (currentTurn == 1 ? 2 : 1);
            }

            InvalidateRect(hWnd, NULL, TRUE);
          }

          break;
      }
      
      break;

    case WM_PAINT:
      PAINTSTRUCT ps;
      HDC hdc;
      HPEN MyPen, OldPen;
      HBRUSH MyBrush, OldBrush;

      hdc = BeginPaint(hWnd, &ps);
      MyPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
      OldPen = (HPEN)SelectObject(hdc, MyPen);

      TextOut(hdc, 350, 100, TEXT("방향키로 커서를 움직이고 엔터로 선택"), 20);

      if (currentTurn == 1) {
        TextOut(hdc, 350, 125, TEXT("플레이어 1 차례"), 9);
      }
      else {
        TextOut(hdc, 350, 125, TEXT("플레이어 2 차례"), 9);
      }

      if (winner == 1) {
        TextOut(hdc, 350, 150, TEXT("플레이어 1 승리"), 9);
      }
      else if (winner == 2) {
        TextOut(hdc, 350, 150, TEXT("플레이어 2 승리"), 9);
      }
      else {
        TextOut(hdc, 350, 150, TEXT("--------------"), 9);
      }

      int x = 50, y = 50, size = 75;
      for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;

        int left = col * size + x;
        int top = row * size + y;
        int right = left + size;
        int bottom = top + size;
        
        // 플레이어 1
        if (board[i] == 1) {
          MyBrush = CreateSolidBrush(RGB(255, 0, 0));
        }
        // 플레이어 2
        else if (board[i] == 2) {
          MyBrush = CreateSolidBrush(RGB(0, 255, 0));
        }
        else {
          MyBrush = CreateSolidBrush(RGB(255, 255, 255));
        }

        OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);
        Rectangle(hdc, left, top, right, bottom);
        SelectObject(hdc, OldBrush);
        DeleteObject(MyBrush);

        // 커서
        if (cussor - 1 == i) {
          HPEN cursorPen;
          if (currentTurn == 1) {
            cursorPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
          }
          else {
            cursorPen = CreatePen(PS_SOLID, 3, RGB(0, 225, 0));
          }
          HPEN oldCursorPen = (HPEN)SelectObject(hdc, cursorPen);

          Rectangle(hdc, left, top, right, bottom);

          SelectObject(hdc, oldCursorPen);
          DeleteObject(cursorPen);
        }
      }

      SelectObject(hdc, OldPen);
      DeleteObject(MyBrush);

      EndPaint(hWnd, &ps);

      break;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
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
