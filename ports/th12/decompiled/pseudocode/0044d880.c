// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d880; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44d880(void)
{
    unsigned short *v2;  // eax
    unsigned int v3;  // eax
    unsigned int v4;  // esi
    unsigned int v5;  // eax
    unsigned int *idx;  // ecx
    unsigned int *v7;  // edx
    unsigned int v0;  // [bp-0x4]

    v3 = *(v2);
    if (v3 & 0xf000)
    {
        v0 = v4;
        v5 = (unsigned short)v3;
        *(idx) = *(idx) + (v5 >> 8 & 15);
        idx[1] = idx[1] + (v5 >> 4 & 15);
        idx[2] = idx[2] + (v5 & 15);
        *(v7) = *(v7) + 1;
    }
    return;
}
