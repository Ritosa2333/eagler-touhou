// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x488e30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_488e30(unsigned int a0, int a1)
{
    int v1;  // eax
    unsigned int v2;  // edx
    unsigned int v0;  // [bp-0x8]

    v0 = 31;
    v1 = (int)(a1 + (a1 >> 31 & v0)) >> 5;
    v2 = a1 & 0x8000001f;
    if ((a1 & 0x8000001f) < 0)
        v2 = (v2 - 1 | 0xffffffe0) + 1;
    if (~(0xffffffff << ((char)(v0 - v2) & 31)) & *((int *)(a0 + v1 * 4)))
        return 0;
    while (1)
    {
        v1 += 1;
        if (v1 >= 3)
        {
            return 1;
        }
        else if (*((int *)(a0 + v1 * 4)))
        {
            return 0;
        }
    }
}
