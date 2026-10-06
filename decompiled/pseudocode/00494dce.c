// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x494dce; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b360c;

int sub_494dce(void)
{
    unsigned int v8;  // eax
    unsigned int v9;  // eax
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // fpround
    unsigned int v22;  // 4143
    unsigned int v23;  // ecx
    unsigned int v24;  // eax
    unsigned short v25;  // ftop
    unsigned short v26;  // ftop
    unsigned short v27;  // ftop
    unsigned int v10;  // ebx
    unsigned int j;  // ecx
    unsigned int v31;  // eax
    unsigned short v32;  // ftop
    unsigned int v33;  // fc3210
    unsigned short v34;  // ftop
    unsigned long long v35;  // 4231
    unsigned int v36;  // fpround
    unsigned short v37;  // ftop
    unsigned int v11;  // ftop
    unsigned short v38;  // ftop
    unsigned short v39;  // ftop
    unsigned int v40;  // edx
    unsigned int v41;  // 4110
    unsigned int i;  // ftop
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned int v15;  // eax
    unsigned int v16;  // ftop
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
        v9 = (*((unsigned int *)((void*)&v2 + 4)) & 0x7fff) + 63;
        v10 = *((unsigned int *)((void*)&v4 + 4)) & 0x7fff;
        if (v10 <= v9)
        {
            for (i = v11; (*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) - ((*((unsigned int *)((void*)&v2 + 4)) & 0x7fff) + 10) >= 0; i = v17 + 2)
            {
                v13 = i - 1;
                if (/* unsupported instruction */)
                {
                    v14 = v13 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v14 = v13 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v15 = *((unsigned int *)((void*)&v2 + 4));
                v3 = (*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) - ((*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) - v15 & 7 | 4) | v15 & 0x8000;
                v16 = v14 - 1;
                if (/* unsupported instruction */)
                {
                    v17 = v16 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v17 = v16 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                *((unsigned int *)((char *)&v2 + 4)) = v15;
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
            if (!(v10 - v9 & 2))
            {
                v18 = v11 - 1;
                if (/* unsupported instruction */)
                {
                    v19 = v18 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v19 = v18 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                if (/* unsupported instruction */)
                {
                    x86g_dirtyhelper_storeF80le((Reference vvar_415{s16|1b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
                    /* unsupported instruction */
                    v20 = v19 + 1;
                }
                else
                {
                    x86g_dirtyhelper_storeF80le((Reference vvar_415{s16|1b}), Reinterpret(F64->I64, nan<64>))
                    /* unsupported instruction */
                    v20 = v19 + 1;
                }
            }
            v22 = _ccall(v21);
            *((unsigned short *)&v5) = v22;
            v6 = v5 | 831;
            v23 = ((*((unsigned int *)((void*)&v4 + 4)) & 0x7fff) - (*((unsigned int *)((void*)&v2 + 4)) & 0x7fff) & 63 | 32) + 1;
            v24 = *((unsigned int *)((void*)&v2 + 4)) & 0x8000;
            v3 = *((unsigned int *)((void*)&v4 + 4)) & 0x7fff | v24;
            v25 = v20 - 1;
            if (/* unsupported instruction */)
            {
                v26 = v25 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v26 = v25 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v27 = v26 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            do
            {
                j = v23;
                v31 = _INSERT(_INSERT(v24, 0, ((char)v27 & 7) * 0), 1, ((v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8);
                v24 = v31 & 0x100;
                if (!(v31 & 0x100))
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
                v23 = j - 1;
            } while (j != 1);
            if (/* unsupported instruction */)
            {
                x86g_dirtyhelper_storeF80le((Reference vvar_416{s28|1b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
                /* unsupported instruction */
                v32 = v27 + 1;
            }
            else
            {
                x86g_dirtyhelper_storeF80le((Reference vvar_416{s28|1b}), Reinterpret(F64->I64, nan<64>))
                /* unsupported instruction */
                v32 = v27 + 1;
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
            v33 = unsupported_Iop_PRem1C3210F64();
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v34 = v32 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v35 = _ccall((unsigned short)v5);
            v36 = v35;
            if (*((unsigned int *)((void*)&v4 + 4)) & 0x8000)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_494f72;
            }
        }
    }
    v37 = v20 - 1;
    if (/* unsupported instruction */)
    {
        v38 = v37 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v38 = v37 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v39 = v38 - 1;
    if (/* unsupported instruction */)
    {
        v34 = v39 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v34 = v39 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v33 = unsupported_Iop_PRem1C3210F64();
LABEL_494f72:
    if (!(v40 & 3))
        return;
    *((unsigned short *)&v7) = (v34 & 7) * 0x800 | (unsigned short)v33 & 0x4700;
    if (v40 & 1)
    {
        v41 = _ccall(v36);
        *((unsigned short *)&v5) = v41;
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
