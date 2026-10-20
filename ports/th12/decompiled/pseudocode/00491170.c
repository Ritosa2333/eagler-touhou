// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491170; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_491170(void)
{
    unsigned short v2;  // ftop
    unsigned int v3;  // fc3210
    unsigned short v0;  // [bp-0x8]

    v0 = (v2 & 7) * 0x800 | (unsigned short)v3 & 0x4700;
    return v0;
}
