// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ed60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_44ed60(unsigned int *idx, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v3;  // eax
    unsigned int v4;  // ecx
    unsigned int *v5;  // ebp
    unsigned int *v6;  // edx
    unsigned int v7;  // eax
    unsigned int v8;  // 4105
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    int v2;  // [bp-0x4]

    if (idx[5] < a2)
        sub_4916e6(v0, v1, v2);
    v3 = idx[5] - a2;
    if (v3 < a3)
        a3 = v3;
    if (!a3)
        return idx;
    v4 = idx[6];
    v5 = idx + 1;
    a2 = (v4 < 16 ? v5 : *(v5));
    if (v4 >= 16)
        v6 = *(v5);
    else
        v6 = v5;
    sub_46e28d((char *)v6 + a2, v4 - a2, (char *)a2 + a2 + a3, v3 - a3);
    v7 = idx[5] - a3;
    v8 = idx[6];
    idx[5] = v7;
    if (v8 >= 16)
        v5 = *(v5);
    *(v7 + (char *)v5) = 0;
    return idx;
}
