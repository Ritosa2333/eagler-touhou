// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493efe; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b37a0;

void sub_493efe(void)
{
    unsigned int v5;  // fpround
    unsigned int v6;  // 4124
    int v0;  // [bp-0x2d8]
    char v1;  // [bp-0x2cc]
    unsigned short v2;  // [bp-0xa8]
    double v3;  // [bp-0x8a], Other Possible Types: unsigned long

    v6 = _ccall(v5);
    v2 = v6;
    if (!g_4b37a0)
    {
        if (/* unsupported instruction */)
            v3 = /* unsupported instruction */;
        else
            v3 = (double)nan;
    }
    sub_494140(v0);
    v1 |= 1;
    v1 &= 253;
    sub_493f8a();
    return;
}
