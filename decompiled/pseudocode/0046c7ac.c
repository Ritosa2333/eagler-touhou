// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c7ac; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

HRSRC sub_46c7ac(PSTR a0, PSTR a1, unsigned int a2)
{
    HMODULE v0;  // [bp+0x0]

    return FindResourceA(v0, a0, a1);
}
