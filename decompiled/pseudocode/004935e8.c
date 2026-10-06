// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4935e8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4110;

int sub_4935e8(void)
{
    unsigned int v2;  // fpround
    unsigned int v3;  // 4103
    unsigned int v4;  // cc_op
    unsigned int v5;  // cc_dep1
    unsigned int v6;  // cc_dep2
    unsigned int v7;  // cc_ndep
    unsigned int v8;  // 4109
    unsigned int v9;  // eax
    unsigned short v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x4]

    v3 = _ccall(v2);
    v0 = v3;
    v8 = _ccall(4, v4, v5, v6, v7);
    if (!(v8 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else if (v9 & 0xfffff || v1)
    {
        sub_49568c();
        if (g_4b4110)
            goto LABEL_0x4956fe;
        sub_495617();
        return;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (v9 & 0x80000000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
    }
    if (g_4b4110)
        goto LABEL_0x4956fe;
}
