// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c836; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

IntPtr sub_46c836(PSTR a0, HWND a1, void* a2, LPARAM a3, unsigned int a4)
{
    HINSTANCE v0;  // [bp+0x0]

    return DialogBoxParamA(v0, a0, a1, a2, a3);
}
