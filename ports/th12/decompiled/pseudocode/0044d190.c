// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d190; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void g_4b0d18;
extern unsigned long long g_4b0d1c;
extern unsigned long long g_4b0d24;
extern unsigned long long g_4b0d2c;
extern unsigned int g_4b0d44;

void sub_44d190(void)
{
    unsigned int v1;  // ftop
    unsigned int v11;  // 4170
    unsigned int v3;  // 4116
    unsigned int v4;  // ftop
    unsigned int v5;  // ftop
    unsigned int v8;  // ftop
    unsigned int v9;  // ftop

    *((unsigned int *)&g_4b0d18) = *((int *)&g_4b0d18) | 2;
    if (g_4b0d18 & 1)
        return;
    while (1)
    {
        sub_4508b0();
        if (/* unsupported instruction */)
            g_4b0d1c = /* unsupported instruction */;
        else
            g_4b0d1c = nan;
        v3 = _ccall(10, 13, (char)((((unsigned short)v1 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, g_4b0d24) * 0x100 & 0x4500 : CmpF(nan, g_4b0d24) * 0x100 & 0x4500) & 0x4700) >> 8) & 5, 0, 0);
        if (!(v3 & 1))
        {
            v4 = v1 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b0d2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        }
        else
        {
            v5 = v1 - 1;
            if (/* unsupported instruction */)
            {
                v4 = v5 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v4 = v5 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b0d24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        if ((char)((((unsigned short)v4 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = v4 + 2;
        }
        else
        {
            if (!((char)((((unsigned short)v4 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                v8 = v4 - 1;
                if (/* unsupported instruction */)
                {
                    v9 = v8 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v9 = v8 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                while (1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v11 = _ccall(10, 13, (char)((((unsigned short)v9 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                    if (v11 & 1)
                        break;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                g_4b0d2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v1 = v9 + 3;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = v4 + 2;
            }
            g_4b0d44 = g_4b0d44 + 1;
            Sleep(13);
            if (g_4b0d18 & 1)
                break;
        }
    }
    return;
}
