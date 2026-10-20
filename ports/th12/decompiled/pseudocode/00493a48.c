// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493a48; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b4110;

int sub_493a48(void)
{
    unsigned int v2;  // fpround
    unsigned int v3;  // 4103
    unsigned short v12;  // ftop
    unsigned int v14;  // eax
    unsigned int v15;  // 4160
    unsigned int v16;  // cc_dep1
    unsigned short v17;  // ftop
    unsigned short v18;  // ftop
    unsigned int v21;  // 4156
    unsigned int v4;  // cc_op
    unsigned int v22;  // eax
    unsigned int v5;  // cc_dep1
    unsigned int v6;  // cc_dep2
    unsigned int v7;  // cc_ndep
    unsigned int v8;  // 4109
    unsigned int v9;  // fc3210
    unsigned int v11;  // eax
    unsigned short v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x4]

    v3 = _ccall(v2);
    v0 = v3;
    v8 = _ccall(4, v4, v5, v6, v7);
    if (!(v8 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v14 = _INSERT(_INSERT(v11, 0, ((char)v12 & 7) * 0), 1, ((v12 & 7) * 0x800 | ((unsigned short)v9 & 0xfbff | (((int)((unsigned long long)((long long)(/* unsupported instruction */ ? /* unsupported instruction */ : nan)) >> 52) & 0x7ff) > 1085) * 0x400) & 0x4700) >> 8);
        v15 = _ccall(5, v0, 639, 0);
        v16 = v15 & 0x800 | v14 >> 8 & 213;
        if ((v15 & 0x800 | v14 >> 8 & 213) >> 2 & 1)
        {
            v17 = v12 - 1;
            if (/* unsupported instruction */)
            {
                v18 = v17 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v18 = v17 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            do
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v14 = _INSERT(_INSERT(v14, 0, ((char)v18 & 7) * 0), 1, ((v18 & 7) * 0x800 | (short)unsupported_Iop_PRem1C3210F64() & 0x4700) >> 8);
                v21 = _ccall(0, v16, 0, 0);
                v16 = v21 & 0x800 | v14 >> 8 & 213;
            } while ((v21 & 0x800 | v14 >> 8 & 213) >> 2 & 1);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (*((int *)&g_4b4110))
            goto LABEL_0x4956fe;
    }
    else
    {
        if (v22 & 0xfffff || v1)
        {
            sub_49568c();
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (*((int *)&g_4b4110))
            goto LABEL_0x4956fe;
        sub_495617();
        return;
    }
}
