// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493498; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4110;

int sub_493498(void)
{
    unsigned int v3;  // fpround
    unsigned int v4;  // 4103
    unsigned int v13;  // ftop
    unsigned int v15;  // fc3210
    unsigned short v16;  // ax
    unsigned int v5;  // cc_op
    unsigned int v6;  // cc_dep1
    unsigned int v7;  // cc_dep2
    unsigned int v8;  // cc_ndep
    unsigned int v9;  // 4109
    unsigned int v10;  // eax
    unsigned int v11;  // ftop
    unsigned int v12;  // ftop
    unsigned short v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x4]
    unsigned int v2;  // [bp+0x8]

    v4 = _ccall(v3);
    v0 = v4;
    v9 = _ccall(4, v5, v6, v7, v8);
    if (!(v9 & 1))
    {
        if (v0 != 639)
            v10 = sub_495675();
        if (v10 < 0x3ff00000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            if (v10 > 0x3ff00000 || v2 & 0xfffff || v1)
                goto LABEL_49351a;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v12 = v11 + 1;
            if (v2 & 0x80000000)
            {
                v13 = v12 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v13 = v12 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        if (!g_4b4110)
        {
            if (v0 == 639)
            {
                return;
            }
            else if (v0 & 32)
            {
                v16 = _INSERT(_INSERT(v0 & 32, 0, ((char)v13 & 7) * 0), 1, (((unsigned short)v13 & 7) * 0x800 | (unsigned short)v15 & 0x4700) >> 8);
                if (!(v16 & 32))
                    return;
                sub_495617();
                return;
            }
            else
            {
                return;
            }
        }
    }
    else
    {
        if (v10 & 0xfffff || v1)
        {
            sub_49568c();
        }
        else
        {
LABEL_49351a:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (!g_4b4110)
        {
            sub_495617();
            return;
        }
    }
    return;
}
