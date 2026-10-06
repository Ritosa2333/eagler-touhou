// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c806; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_46c806(void* a0, unsigned int a1, unsigned int a2, PSTR a3, unsigned int a4, SByte **a5, unsigned int a6)
{
    enum FORMAT_MESSAGE_OPTIONS v0;  // [bp+0x0]

    return FormatMessageA(v0, a0, a1, a2, a3, a4, a5);
}
