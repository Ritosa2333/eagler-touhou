// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db939; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_b2296db0;

int sub_4db939(unsigned int a0, unsigned short a1)
{
    unsigned short v3;  // ds
    unsigned int v4;  // esi
    unsigned int v0;  // [bp-0x8]
    unsigned short v1;  // [bp-0x4]

    v1 = v3;
    v0 = v4 + 1;
    g_b2296db0 = g_b2296db0 + *((char *)((void*)&a1 + 1)) + ((char)v0 & 213 & 1);
}
