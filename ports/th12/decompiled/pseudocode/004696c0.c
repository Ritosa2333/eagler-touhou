// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4696c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4696c0(void)
{
    unsigned int *idx;  // eax
    int v2;  // edx
    unsigned int v3;  // ecx
    int v4;  // ecx

    v2 = idx[0x400];
    v4 = v3 + v2;
    if (v4 >= 0x1000)
        return;
    idx[0x400] = v4;
    if (v4 + 4 < 0x1000)
    {
        *((unsigned int *)(v4 + (char *)idx)) = idx[1025];
        idx[0x400] = idx[0x400] + 4;
    }
    idx[1025] = v2;
    return;
}
