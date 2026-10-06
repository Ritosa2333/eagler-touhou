// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422d70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_422d70(void)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // edi
    unsigned int *idx;  // eax
    unsigned int v6;  // ecx
    unsigned int v7;  // ebx
    unsigned int v8;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x4]

    v1 = v3;
    v0 = v4;
    v6 = idx[36];
    if (idx[2] >= v6)
        return;
    v8 = idx[2] + v7;
    idx[2] = v8;
    if (v8 <= v6)
        return;
    idx[2] = v6;
    sub_420f90(0);
    return;
}
