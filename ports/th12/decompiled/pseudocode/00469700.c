// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x469700; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_469700(void)
{
    unsigned int *idx;  // eax
    unsigned int v2;  // ecx
    unsigned int v3;  // edx
    unsigned int v4;  // 4112

    v2 = idx[0x400] - 4;
    v3 = idx[1025];
    v4 = _ccall(8, 3, idx[0x400], 0xfffffffc, 0);
    if (!(v4 & 1))
    {
        idx[0x400] = v2;
        idx[1025] = *((int *)(v2 + (char *)idx));
    }
    idx[0x400] = v3;
    return;
}
