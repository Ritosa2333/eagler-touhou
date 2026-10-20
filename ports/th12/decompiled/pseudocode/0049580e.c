// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49580e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned long long g_4a87d8[4];
extern unsigned long long g_4a87e0[4];
extern unsigned long long g_4a87e8[4];

int sub_49580e(unsigned int a0)
{
    unsigned long v3;  // xmm7
    int v4;  // xmm7
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    uint128_t v16;  // xmm1
    uint128_t v17;  // xmm3
    uint128_t v19;  // xmm5
    unsigned int v20;  // eax
    unsigned int v21;  // eax
    int v22;  // xmm6
    uint128_t v5;  // xmm2
    uint128_t v24;  // xmm0
    int v26;  // xmm5
    int v28;  // xmm3
    int v29;  // xmm3
    unsigned int v30;  // eax
    int v31;  // xmm5
    int v32;  // xmm5
    unsigned int v6;  // 4101
    int v34;  // xmm3
    uint128_t v35;  // xmm2
    int v38;  // xmm2
    int v39;  // xmm0
    int v40;  // xmm0
    int v41;  // xmm4
    int v42;  // xmm4
    uint128_t v7;  // xmm5
    uint128_t v43;  // xmm5
    unsigned int v45;  // eax
    unsigned int v8;  // eax
    unsigned int v9;  // eax
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned int v12;  // eax
    double v0;  // [bp-0x8], Other Possible Types: unsigned long
    unsigned long long v1;  // [bp+0x4], Other Possible Types: unsigned long
    unsigned int v2;  // [bp+0x8]

    v4 = (int)(CONCAT(v3, v3));
    v5 = (uint128_t)(v4 & 0x7fffffffffffffff7fffffffffffffff);
    if (!((CmpF((unsigned long long)v5, 4849535219099880885) & 69) >> 2 & 1))
    {
        v6 = _ccall(2, 0, CmpF((unsigned long long)v5, 4849535219099880885) & 69, 0, 0);
        if (v6 & 1)
        {
            if (CmpF((unsigned long long)v5, 0x3fa0000000000000) & 1)
            {
                if ((CmpF((unsigned long long)v5, 0x3e40000000000000) & 1) != 1)
                {
                    v7 = MulV(((254815108819345326510830319739201359112 * v5 * v5 * v5 * v5 + 254861006519217718044098576153024022543) * v5 * v5 * v5 * v5 + 0xbfc99999999767183fc249249014497e) * v5 * v5 * v5 * v5 + 0x3fd55555555554eb, v5 * v5);
                    v1 = *((unsigned long long *)&SubV(v4, MulV(AddV(v7, CONCAT((unsigned long long)v7, (unsigned long long)(v7 >> 64))), v4)));
                    if (!/* unsupported instruction */)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return v9;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return v8;
                }
                else if (((CmpF((unsigned long long)v5, 0) & 69) >> 6 & 1) == 1)
                {
                    if (/* unsupported instruction */)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return v10;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return v11;
                }
                else
                {
                    if (CmpF((unsigned long long)v5, 0x10000000000000) & 1)
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
                            v0 = /* unsupported instruction */;
                            /* unsupported instruction */
                        }
                        else
                        {
                            v0 = (double)nan;
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
                        if (!/* unsupported instruction */)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            return v13;
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return v12;
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
                        if (!/* unsupported instruction */)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            return v15;
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return v14;
                    }
                }
            }
            else
            {
                if (CmpF((unsigned long long)v5, 0x3fd8000000000000) & 1)
                {
                    v16 = v5 * v5;
                    v17 = v16 * v16;
                    v19 = MulV(((((((254580552878233084881241189415529789605 * v17 + 254719253487887784188584142538100482722) * v17 + 254755321886754339136259495066161715902) * v17 + 0xbfae1c1704144b683faae4492fe3a600) * v17 + 254815562838268672877083810267873636042) * v17 + 254861006691527917784542035344369114762) * v17 + 0xbfc99999999992ac3fc249249246aa76) * v17 + 0x3fd5555555555552, v16);
                    v1 = *((unsigned long long *)&SubV(v4, MulV(AddV(v19, CONCAT((unsigned long long)v19, (unsigned long long)(v19 >> 64))), v4)));
                    if (!/* unsupported instruction */)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return v21;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return v20;
                }
                else
                {
                    v22 = (int)_INSERT(0, 0, *((unsigned long long *)&v4));
                    if (CmpF((unsigned long long)v5, 0x4020000000000000) & 1)
                    {
                        v24 = _INSERT(_INSERT(0, 8, 0), 0, 0x4020000000000000);
                        v26 = (int)_INSERT(_INSERT(0, 8, 0), 0, 262657);
                        v28 = (int)_INSERT(_INSERT(0, 0, v5), 8, 0);
                        v29 = SubV(ShrNV(AddV(v28, v24), 44), v26);
                        v30 = *((unsigned int *)&v29) * 3;
                        v31 = (int)_INSERT(v26, 8, 0);
                        v32 = (int)_INSERT(v31, 0, g_4a87e8[v30]);
                        v34 = (int)_INSERT(_INSERT(v29, 0, v5), 8, 0);
                        v35 = CONCAT((unsigned long long)(DivV(SubV(v5, v32), AddV(MulV(v34, v32), 0x3ff0000000000000))), (unsigned long long)(DivV(SubV(v5, v32), AddV(MulV(v34, v32), 0x3ff0000000000000))));
                    }
                    else
                    {
                        v30 = 0x300;
                        v24 = _INSERT(_INSERT(0, 0, v5), 8, 0);
                        v38 = (int)_INSERT(_INSERT(v5, 8, 0), 0, 0xbff0000000000000);
                        v35 = CONCAT((unsigned long long)(DivV(v38, v24)), (unsigned long long)(DivV(v38, v24)));
                    }
                    v39 = (int)_INSERT(v24, 8, 0);
                    v40 = (int)_INSERT(v39, 0, g_4a87d8[v30]);
                    v41 = (int)_INSERT(0, 8, 0);
                    v42 = (int)_INSERT(v41, 0, g_4a87e0[v30]);
                    v43 = MulV(((254815108819345326510830319739201359112 * v35 * v35 * v35 * v35 + 254861006519217718044098576153024022543) * v35 * v35 * v35 * v35 + 0xbfc99999999767183fc249249014497e) * v35 * v35 * v35 * v35 + 0x3fd55555555554eb, v35 * v35);
                    v1 = *((unsigned long long *)&SubV(v40, SubV(SubV(MulV(AddV(v43, CONCAT((unsigned long long)v43, (unsigned long long)(v43 >> 64))), v35), v42), v35)) | _INSERT(v22, 8, 0) & 0xffffffffffffffff ^ v5);
                    if (!/* unsupported instruction */)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return v30;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return v30;
                }
            }
        }
        else
        {
            if (!((CmpF((unsigned long long)(_INSERT(_INSERT(0, 0, v5), 8, 0) & 0xffffffffffffffff & 0x7ff0000000000000), 0x7ff0000000000000) & 69) >> 2 & 1))
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
                return v2 >> 31;
            }
        }
    }
    v45 = sub_493b07(&v1, &v1, &v1, 1003);
    if (!/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return v45;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    return v45;
}
