// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c7d6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_46c7d6(enum MULTI_BYTE_TO_WIDE_CHAR_FLAGS a0, LPArray a1, int a2, LPArray a3, int a4, unsigned int a5)
{
    unsigned int v0;  // [bp+0x0]

    return MultiByteToWideChar(v0, a0, a1, a2, a3, a4);
}
