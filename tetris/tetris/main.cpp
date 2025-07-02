#include <windows.h>

// 定数
const int FIELD_WIDTH = 12;  // 10 + 左右壁1ずつ
const int FIELD_HEIGHT = 22; // 20 + 上2待機行

// グローバル変数
int block_size = 20; // ブロックの1マスサイズ（ピクセル）

// フィールド配列
int field[FIELD_HEIGHT][FIELD_WIDTH] = { 0 };

// 壁・床初期化
void InitField() {
    for (int y = 0; y < FIELD_HEIGHT; ++y) {
        for (int x = 0; x < FIELD_WIDTH; ++x) {
            if (x == 0 || x == FIELD_WIDTH - 1) field[y][x] = 9; // 壁
            else if (y == FIELD_HEIGHT - 1) field[y][x] = 8;      // 床
            else field[y][x] = 0;                                // 空き
        }
    }
}

// ブロック描画
void DrawBlock(HDC hdc, int x, int y, COLORREF color) {
    RECT rect = {
        x * block_size,
        y * block_size,
        (x + 1) * block_size,
        (y + 1) * block_size
    };
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        InitField();
        return 0;

    case WM_SIZE: {
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        int w = rcClient.right - rcClient.left;
        int h = rcClient.bottom - rcClient.top;

        int size_x = w / FIELD_WIDTH;
        int size_y = h / FIELD_HEIGHT;
        block_size = (size_x < size_y) ? size_x : size_y;
        if (block_size < 1) block_size = 1; // 最小1px

        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        for (int y = 0; y < FIELD_HEIGHT; ++y) {
            for (int x = 0; x < FIELD_WIDTH; ++x) {
                if (field[y][x]) {
                    COLORREF color = RGB(0, 100, 255);
                    if (field[y][x] == 9) color = RGB(50, 50, 50);       // 壁
                    else if (field[y][x] == 8) color = RGB(100, 50, 0);  // 床
                    DrawBlock(hdc, x, y, color);
                }
            }
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}

// エントリーポイント
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"TetrisResizableWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Tetris - Resizable Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        FIELD_WIDTH * block_size,
        FIELD_HEIGHT * block_size,
        NULL, NULL, hInst, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, SW_SHOWMAXIMIZED); // 最大化表示
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
