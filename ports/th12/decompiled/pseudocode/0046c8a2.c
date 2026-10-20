// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c8a2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

LRESULT sub_46c8a2(unsigned int a0, WPARAM a1, LPARAM a2, unsigned int a3)
{
    HWND v0;  // [bp+0x0]

    return SendMessageA(v0, a0, a1, a2);
}
