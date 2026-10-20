// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c89c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct MSG {
    HWND hwnd;
    unsigned int message;
    WPARAM wParam;
    LPARAM lParam;
    unsigned int time;
    POINT pt;
} MSG;

typedef struct POINT {
    int x;
    int y;
} POINT;

int sub_46c89c(unsigned int a0)
{
    MSG *v0;  // [bp+0x0]

    return TranslateMessage(v0);
}
