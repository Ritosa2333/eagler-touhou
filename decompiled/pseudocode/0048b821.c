// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b821; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_48b821(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v6;  // edi
    unsigned int v7;  // esi
    int v8;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    v2 = v3;
    v4 = 0;
    if (a1 > 0)
    {
        v1 = 0xffffffe0;
        if (0xffffffe0 / a1 < a2)
        {
            *((unsigned int *)sub_46e96f()) = 12;
            sub_470f2d(0, 0, 0, 0, 0);
            return 0;
        }
    }
    v1 = v5;
    v0 = v6;
    v7 = a2 * a1;
    if (a0)
        v4 = sub_4778ff(a0);
    v8 = sub_48b606(a0, v7);
    if (v8 && v4 < v7)
        sub_477420(v4 + v8, 0, v7 - v4);
    return v8;
}
