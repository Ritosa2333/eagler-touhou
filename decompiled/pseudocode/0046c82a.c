// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c82a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct WNDCLASSA {
    enum WNDCLASS_STYLES style;
    LRESULT (*lpfnWndProc)(int *, unsigned int, unsigned int *, int *);
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    PSTR lpszMenuName;
    PSTR lpszClassName;
} WNDCLASSA;

unsigned short sub_46c82a(unsigned int a0)
{
    WNDCLASSA *v0;  // [bp+0x0]

    return RegisterClassA(v0);
}
