// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x494b16; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b360c;

int sub_494b16(void)
{
    unsigned int v8;  // eax
    unsigned int v9;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // fpround
    unsigned int v21;  // 4143
    unsigned int v22;  // ecx
    unsigned int v23;  // eax
    unsigned short v24;  // ftop
    unsigned short v25;  // ftop
    unsigned short v26;  // ftop
    unsigned int j;  // ecx
    unsigned int i;  // ftop
    unsigned int v30;  // eax
    unsigned short v31;  // ftop
    unsigned int v32;  // fc3210
    unsigned short v33;  // ftop
    unsigned long long v34;  // 4231
    unsigned int v35;  // fpround
    unsigned short v36;  // ftop
    unsigned short v37;  // ftop
    unsigned int v11;  // ftop
    unsigned short v38;  // ftop
    unsigned int v39;  // 4110
    unsigned int v12;  // ftop
    unsigned int v13;  // eax
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v16;  // edx
    unsigned int v17;  // ftop
    unsigned int v0;  // [bp-0x24]
    unsigned long long v2;  // [bp+0x8]
    unsigned int v3;  // [bp+0xc]
    unsigned long v4;  // [bp+0x20]
    unsigned int v5;  // [bp+0x28]
    unsigned int v6;  // [bp+0x2c]
    unsigned int v7;  // [bp+0x30]

    v8 = *((unsigned int *)((void*)&v2 + 2)) ^ 0x700;
    if (!(v8 & 0x700) && (&g_4b360c)[v8 >> 11 & 15] && (*((unsigned int *)((void*)&v2 + 2)) & 0x7fff0000) != 0x7fff0000 && *((unsigned int *)((void*)&v4 + 2)) & 0x7fff0000 && (*((unsigned int *)((void*)&v4 + 2)) & 0x7fff0000) != 0x7fff0000 && !(v4 * 2) && !((unsigned int)v2 * 2))
    {
        if ((*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) <= (*((unsigned int *)((void*)&v2 + 4)) & 0x7fff) + 63)
        {
            for (i = v9; (*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) - ((*((unsigned int *)((void*)&v2 + 4)) & 0x7fff) + 10) >= 0; i = v15 + 2)
            {
                v11 = i - 1;
                if (/* unsupported instruction */)
                {
                    v12 = v11 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v12 = v11 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v13 = *((unsigned int *)((void*)&v2 + 4));
                v3 = (*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) - ((*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) - v13 & 7 | 4) | v13 & 0x8000;
                v14 = v12 - 1;
                if (/* unsupported instruction */)
                {
                    v15 = v14 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v15 = v14 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                *((unsigned int *)((char *)&v2 + 4)) = v13;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                x86g_dirtyhelper_storeF80le((Reference vvar_416{s28|1b}), Reinterpret(F64->I64, (((unsupported_<class 'pyvex.expr.GetI'>() CmpNE 0<8>)) ? (unsupported_<class 'pyvex.expr.GetI'>()) : (nan<64>))))
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        else
        {
            if (!(v16 & 2))
            {
                v17 = v9 - 1;
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
                if (/* unsupported instruction */)
                {
                    x86g_dirtyhelper_storeF80le((Reference vvar_415{s16|1b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
                    /* unsupported instruction */
                    v19 = v18 + 1;
                }
                else
                {
                    x86g_dirtyhelper_storeF80le((Reference vvar_415{s16|1b}), Reinterpret(F64->I64, nan<64>))
                    /* unsupported instruction */
                    v19 = v18 + 1;
                }
            }
            v21 = _ccall(v20);
            *((unsigned short *)&v5) = v21;
            v6 = v5 | 831;
            v22 = ((*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) - (*((unsigned int *)((void*)&v2 + 4)) & 0x7fff) & 63 | 32) + 1;
            v23 = *((unsigned int *)((void*)&v2 + 4)) & 0x8000;
            v3 = *((unsigned int *)((void*)&v4 + 4)) & 0x7fff | v23;
            v24 = v19 - 1;
            if (/* unsupported instruction */)
            {
                v25 = v24 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v25 = v24 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v26 = v25 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            do
            {
                j = v22;
                v30 = _INSERT(_INSERT(v23, 0, ((char)v26 & 7) * 0), 1, ((v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8);
                v23 = v30 & 0x100;
                if (!(v30 & 0x100))
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
                v22 = j - 1;
            } while (j != 1);
            if (/* unsupported instruction */)
            {
                x86g_dirtyhelper_storeF80le((Reference vvar_416{s28|1b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
                /* unsupported instruction */
                v31 = v26 + 1;
            }
            else
            {
                x86g_dirtyhelper_storeF80le((Reference vvar_416{s28|1b}), Reinterpret(F64->I64, nan<64>))
                /* unsupported instruction */
                v31 = v26 + 1;
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
            v32 = unsupported_Iop_PRemC3210F64();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v33 = v31 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v34 = _ccall((unsigned short)v5);
            v35 = v34;
            if (*((unsigned int *)((void*)&v4 + 4)) & 0x8000)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_494cba;
            }
        }
    }
    v36 = v19 - 1;
    if (/* unsupported instruction */)
    {
        v37 = v36 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v37 = v36 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v38 = v37 - 1;
    if (/* unsupported instruction */)
    {
        v33 = v38 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v33 = v38 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v32 = unsupported_Iop_PRemC3210F64();
LABEL_494cba:
    if (!(v16 & 3))
        return;
    *((unsigned short *)&v7) = (v33 & 7) * 0x800 | (unsigned short)v32 & 0x4700;
    if (v16 & 1)
    {
        v39 = _ccall(v35);
        *((unsigned short *)&v5) = v39;
        v6 = v5 | 0x300;
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
    x86g_dirtyhelper_FSTENV(unsupported_<class 'pyvex.expr.GSPTR'>(), (Reference vvar_412{s-40|1b}))
    v0 &= 0xbcff;
    v0 |= v7 & 0x4300;
    return;
}
