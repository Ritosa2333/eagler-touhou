// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x494310; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b35f0;

unsigned int sub_494310(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    unsigned int v2;  // fpround
    unsigned int v3;  // eax
    unsigned int v4;  // 4140
    unsigned long long v6;  // 4213
    unsigned int v7;  // eax
    unsigned int v8;  // 4143
    unsigned int v0;  // [bp+0x1c]
    unsigned int v1;  // [bp+0x20]

    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    while (1)
    {
        if (a1 * 2 < 0)
        {
            v7 = a1 * 2 ^ 0xe000000;
            if (v7 & 0xe000000)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                return v7;
            }
            else if (!(&g_4b35f0)[v7 >> 28])
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                return v7 >> 28;
            }
            else
            {
                v3 = a2 & 0x7fff;
                if (a2 & 0x7fff)
                {
                    if (v3 != 0x7fff)
                    {
                        v8 = _ccall(v2);
                        *((unsigned short *)&v0) = v8;
                        v1 = (v0 | 831) & 0xf3ff;
                        if ((a5 & 0x7fff) != 1)
                        {
                            if (/* unsupported instruction */)
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
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
                            return a5 & 0x7fff;
                        }
                        else
                        {
                            if (/* unsupported instruction */)
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
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
                            return 1;
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            v3 = a0 | a1;
            if (!a0 && !a1 || !(v3 = a2 & 0x7fff, !(a2 & 0x7fff)))
                break;
            v4 = _ccall(v2);
            *((unsigned short *)&v0) = v4;
            v1 = (v0 | 831) & 0xf3ff;
            if (a5 & 0x7fff)
            {
                if ((a5 & 0x7fff) == 0x7fff)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return a5 & 0x7fff;
                }
                else if (a4 * 2 >= 0)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return a4 * 2;
                }
            }
            else
            {
                if (a4 * 2 < 0)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return a4 * 2;
                }
            }
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
            x86g_dirtyhelper_storeF80le((Reference vvar_0), Reinterpret(F64->I64, (((unsupported_<class 'pyvex.expr.GetI'>() CmpNE 0<8>)) ? (unsupported_<class 'pyvex.expr.GetI'>()) : (nan<64>))))
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = _ccall((unsigned short)v0);
            v2 = v6;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return v3;
}
