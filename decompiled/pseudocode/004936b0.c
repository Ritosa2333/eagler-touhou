// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4936b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52d4;

void sub_4936b0(void)
{
    unsigned int v4;  // sseround
    unsigned int v5;  // 4121
    unsigned int v6;  // eax
    unsigned int v7;  // cc_op
    unsigned int v8;  // cc_dep2
    unsigned int v9;  // fpround
    unsigned int v10;  // 4106
    unsigned int v11;  // 4104
    double v0;  // [bp-0xc], Other Possible Types: unsigned long
    unsigned short v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    if (g_4d52d4)
    {
        v5 = _ccall(v4 & 3);
        v2 = v5;
        v6 = v2 & 8064;
        v7 = 6;
        v8 = 8064;
        if (v6 == 8064)
        {
            v10 = _ccall(v9);
            v1 = v10;
            v7 = 5;
            v6 = v1 & 127;
            v8 = 127;
        }
        v11 = _ccall(4, v7, v6, v8, 0);
        if (v11 & 1)
        {
            if (/* unsupported instruction */)
            {
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v0 = (double)nan;
                /* unsupported instruction */
            }
            sub_495afe();
            return;
        }
    }
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    sub_4956e8();
    sub_493708((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    return;
}
