// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4757b7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4757b7(unsigned int *idx, int a1, unsigned int a2, unsigned int a3, unsigned int a4, int a5)
{
    unsigned int v7;  // ebx
    int v8;  // edi
    unsigned int v9;  // eax
    unsigned int v10;  // eax
    int v0;  // [bp-0x30]
    int v1;  // [bp-0x2c]
    char v2;  // [bp-0x28], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]
    char v6;  // [bp-0x14]

    v7 = 0;
    v5 = sub_476077(&v6, &v2, a1, 0, 0, 0, 0, a5, v8, v0, v1);
    if ((char)v5 & 4)
    {
        v7 = 0x200;
        v3 = 0;
        v4 = 0;
    }
    else
    {
        v9 = sub_4895ea(&v6, &v3);
        if ((char)v5 & 2 || v9 == 1)
            v7 = 128;
        if ((char)v5 & 1 || v9 == 2)
            v7 |= 0x100;
    }
    idx[1] = v2 - a1;
    idx[4] = v3;
    idx[5] = v4;
    *(idx) = v7;
    return v10;
}
