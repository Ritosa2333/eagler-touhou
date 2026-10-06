// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46cbbb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_46cbbb(char *a0, int a1)
{
    int v7;  // esi
    unsigned int v8;  // eax
    int v0;  // [bp-0x28]
    char *v1;  // [bp-0x24]
    int v2;  // [bp-0x20]
    int v3;  // [bp-0x20]
    char *v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]
    char v6;  // [bp+0xc]

    if (a1 && a0)
    {
        v4 = a0;
        v1 = a0;
        v2 = 0x7fffffff;
        v5 = 66;
        v8 = sub_47021f(&v1, a1, 0, &v6, v7);
        v3 = v2 - 1;
        if (v2 - 1 >= 0)
            *(v1) = 0;
        else
            sub_46ffdb(0, &v1);
        return v8;
    }
    *((unsigned int *)sub_46e96f(v0)) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 0xffffffff;
}
