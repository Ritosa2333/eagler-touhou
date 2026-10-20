// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d5b7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_46d5b7(void)
{
    unsigned int v4;  // ecx
    unsigned int v5;  // eax
    int v6;  // edx
    unsigned int v7;  // edx
    unsigned int v0[2];  // [bp-0xc]
    unsigned int v1[2];  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int *v3;  // [bp+0x4]

    v2 = v4;
    v0 = (unsigned int (32 bits)[2])v4;
    GetSystemTimeAsFileTime(v1);
    v5 = sub_4767d0(*(&v1[0]) + 0x2ac18000, *(&v1[1]) - 27111903 + (*(&v1[0]) + 0x2ac18000 < *(&v1[0])), 10000000, 0);
    if (v6 >= 7 && (v6 > 7 || v5 > 0x93406fff))
    {
        v5 = 0xffffffff;
        v7 = 0xffffffff;
    }
    if (v3)
    {
        *(v3) = v5;
        v3[1] = v7;
    }
    return;
}
