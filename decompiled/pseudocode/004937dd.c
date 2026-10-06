// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4937dd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4110;

int sub_4937dd(unsigned int a0, unsigned int a1)
{
    unsigned int v1;  // fpround
    unsigned int v2;  // 4104
    unsigned int v3;  // eax
    unsigned int v4;  // cc_op
    unsigned int v5;  // cc_dep1
    unsigned int v6;  // cc_dep2
    unsigned int v7;  // cc_ndep
    unsigned int v8;  // 4113
    unsigned short v0;  // [bp-0x4]

    v2 = _ccall(v1);
    v0 = v2;
    v3 = a1;
    v8 = _ccall(4, v4, v5, v6, v7);
    if (!(v8 & 1))
    {
        if (v0 != 639)
            v3 = sub_495675();
        if (!(v3 & 0x80000000))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            if (g_4b4110)
                goto LABEL_0x4956fe;
        }
        else if (!(v3 & 0x7ff00000) && !(v3 & 0xfffff) && !a0)
        {
            if (g_4b4110)
                goto LABEL_0x4956fe;
        }
    }
    else
    {
        if (v3 & 0xfffff || a0)
        {
            sub_49568c();
        }
        else
        {
            if (!(v3 & 0x80000000))
                if (g_4b4110)
                    goto LABEL_0x4956fe;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (g_4b4110)
            goto LABEL_0x4956fe;
        return sub_495617();
    }
}
