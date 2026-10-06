// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493290; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b35b8;
extern unsigned int g_4d52d4;

int sub_493290(void)
{
    unsigned int v11;  // sseround
    unsigned int v12;  // 4121
    int v21;  // xmm0
    unsigned int v22;  // eax
    uint128_t v23;  // xmm0
    unsigned int v24;  // xmm2
    uint128_t v25;  // xmm1
    int v27;  // xmm0
    uint128_t v28;  // xmm1
    int v29;  // esi
    int v30;  // ebx
    unsigned int v13;  // eax
    int v31;  // ecx
    int v32;  // ebp
    int v33;  // eax
    unsigned short v34;  // ftop
    unsigned short v35;  // ftop
    unsigned short v36;  // ftop
    int v37;  // ebx
    unsigned short v38;  // ftop
    int v39;  // eax
    unsigned int v40;  // ecx
    unsigned int v14;  // cc_op
    unsigned short v41;  // ftop
    unsigned short v42;  // ftop
    unsigned short v43;  // ftop
    unsigned int v44;  // fc3210
    unsigned short v45;  // ftop
    unsigned int v46;  // 4119
    unsigned int v15;  // cc_dep2
    unsigned int v16;  // fpround
    unsigned int v17;  // 4106
    unsigned int v18;  // 4104
    int v20;  // xmm0
    int v0;  // [bp-0x30]
    int v1;  // [bp-0x2c]
    double v2;  // [bp-0x28]
    unsigned int v3;  // [bp-0x24]
    unsigned int v4;  // [bp-0x20], Other Possible Types: double, unsigned long
    double v5;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x18], Other Possible Types: int
    double v7;  // [bp-0xc]
    unsigned short v8;  // [bp-0x8]
    unsigned int v9;  // [bp-0x4]
    int v10;  // [bp+0x4], Other Possible Types: unsigned int, unsigned long long, unsigned long

    if (g_4d52d4)
    {
        v12 = _ccall(v11 & 3);
        v9 = v12;
        v13 = v12 & 8064;
        v14 = 6;
        v15 = 8064;
        if (v13 == 8064)
        {
            v17 = _ccall(v16);
            v8 = v17;
            v14 = 5;
            v13 = (unsigned short)v17 & 127;
            v15 = 127;
        }
        v18 = _ccall(4, v14, v13, v15, 0);
        if (v18 & 1)
        {
            v20 = (int)_INSERT(_INSERT(0, 8, 0), 0, *((unsigned long long *)&v10));
            v21 = ShrNV(v20, 52);
            v22 = *((unsigned int *)&v21);
            v23 = (uint128_t)(v21 & 0x7ff);
            v24 = SubV(19830249879237767988275, v23);
            v25 = (uint128_t)(v24 < 64 ? ShrNV(v20, (char)v24) : 0);
            if (v22 & 0x800)
            {
                v27 = (int)_INSERT(_INSERT(v23, 8, 0), 0, *((unsigned long long *)&v10));
                v28 = (v24 < 64 ? ShlNV(v25, (char)v24) : 0);
                if (v22 >= 0xbff)
                {
                    if (v22 <= 3122)
                    {
                        v10 = SubV(v28, CmpLTV(v27, v28) & 0x3ff00000000000003ff0000000000000);
                        if (!/* unsupported instruction */)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            return;
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return;
                    }
                }
                else
                {
                    v10 = (CmpLTV(v27, 0x80000000000000008000000000000000) | 0x80000000000000008000000000000000) & 0x4330000000000000bff0000000000000;
                    if (!/* unsupported instruction */)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return;
                }
            }
            else if (v22 < 0x3ff)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                return;
            }
            else if (v22 <= 1074)
            {
                v10 = (v24 < 64 ? (unsigned long long)(ShlNV(v25, (char)v24)) : 0);
                if (!/* unsupported instruction */)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                return;
            }
            if ((CmpF(*((unsigned long long *)&v20), *((unsigned long long *)&v20)) & 69) >> 2 & 1)
                sub_493b07(&v10, &v10, &v10, 1005);
            if (!/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                return;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            return;
        }
    }
    v33 = sub_491181(g_4b35b8, 0xffff, v29, v30, v31, v31, v32);
    v35 = v34 - 1;
    if (/* unsupported instruction */)
    {
        v36 = v35 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v36 = v35 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v37 = v33;
    v6 = *((unsigned int *)(&v10 + 6)) & 0x7ff0;
    v5 = *((unsigned int *)(&v10 + 6)) & 0x7ff0;
    if (/* unsupported instruction */)
    {
        v5 = (double)/* unsupported instruction */;
        /* unsupported instruction */
        v38 = v36 + 1;
    }
    else
    {
        v5 = (double)nan;
        /* unsupported instruction */
        v38 = v36 + 1;
    }
    if ((unsigned short)(*((unsigned int *)(&v10 + 6)) & 0x7ff0) == 0x7ff0)
    {
        v39 = sub_496a9d(*((unsigned int *)&v5));
        v40 = *((unsigned int *)((void*)&v5 + 4));
        if (v39 > 0)
        {
            if (v39 <= 2)
            {
                sub_491181(v37, 0xffff);
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
                return;
            }
            else if (v39 == 3)
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
                v6 = v37;
                v5 = v40;
                v4 = v40;
                if (/* unsupported instruction */)
                {
                    v4 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v4 = (double)nan;
                    /* unsupported instruction */
                }
                v3 = 11;
                sub_49679a();
                return;
            }
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
        v6 = v37;
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
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = (double)nan;
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
        if (/* unsupported instruction */)
        {
            v2 = (double)/* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = (double)nan;
            /* unsupported instruction */
        }
        v1 = 11;
        v0 = 8;
    }
    else
    {
        sub_4969f1(*((unsigned int *)&v5));
        if (/* unsupported instruction */)
        {
            v7 = (double)/* unsupported instruction */;
            /* unsupported instruction */
            v41 = v38 + 1;
        }
        else
        {
            v7 = (double)nan;
            /* unsupported instruction */
            v41 = v38 + 1;
        }
        v42 = v41 - 1;
        if (/* unsupported instruction */)
        {
            v43 = v42 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v43 = v42 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v44 = CmpF(/* unsupported instruction */, v7) * 0x100 & 0x4500;
            /* unsupported instruction */
            v45 = v43 + 1;
        }
        else
        {
            v44 = CmpF(nan, v7) * 0x100 & 0x4500;
            /* unsupported instruction */
            v45 = v43 + 1;
        }
        v46 = _ccall(10, 13, (char)(((v45 & 7) * 0x800 | (unsigned short)v44 & 0x4700) >> 8) & 0x44, 0, 0);
        if (!(v46 & 1) || (char)v37 & 32)
        {
            sub_491181(v37, 0xffff);
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
            return;
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
        v6 = v37;
        if (/* unsupported instruction */)
        {
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = (double)nan;
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
        if (/* unsupported instruction */)
        {
            v2 = (double)/* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = (double)nan;
            /* unsupported instruction */
        }
        v1 = 11;
        v0 = 16;
    }
    sub_496850(v0, v1, *((unsigned int *)&v2));
    return;
}
