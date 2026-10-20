// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c87e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

HWND sub_46c87e(PSTR a0, PSTR a1, enum WINDOW_STYLE a2, int a3, int a4, int a5, int a6, HWND a7, HMENU a8, HINSTANCE a9, void* a10, unsigned int a11)
{
    enum WINDOW_EX_STYLE v0;  // [bp+0x0]

    return CreateWindowExA(v0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}
