// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c81e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_46c81e(LPArray a0, int a1, int *a2, enum DRAW_TEXT_FORMAT a3, unsigned int a4)
{
    HDC v0;  // [bp+0x0]

    return DrawTextA(v0, a0, a1, a2, a3);
}
