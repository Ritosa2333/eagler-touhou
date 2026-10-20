// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43a810; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43a810_0 {
    char padding_0[10164];
    unsigned int field_27b4;
} st_43a810_0;

typedef struct st_43a810_1 {
    char padding_0[4];
    int field_4;
    char padding_8[24];
    unsigned int field_20;
    unsigned int field_24;
    char padding_28[4];
    unsigned int field_2c;
    unsigned int field_30;
    char padding_34[20];
    unsigned int field_48;
    int field_4c;
    char padding_50[4];
    struct st_43a810_0 *field_54;
    char padding_58[12];
    unsigned int field_64;
    char padding_68[8];
    unsigned int field_70;
} st_43a810_1;

extern st_43a810_0 *g_4b43dc;

unsigned int sub_43a810(unsigned int a0, st_43a810_1 *idx)
{
    unsigned int v6;  // esi
    st_43a810_0 *v7;  // ecx
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // 4139
    unsigned short v22;  // ftop
    unsigned short v23;  // ftop
    unsigned short v24;  // ftop
    unsigned short v25;  // ftop
    unsigned int v8;  // ebx
    unsigned short v27;  // ftop
    int v28;  // ecx
    unsigned short v29;  // ftop
    unsigned short v30;  // ftop
    unsigned short v31;  // ftop
    unsigned short v32;  // ftop
    unsigned int v34;  // ecx
    unsigned int v9;  // ftop
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    unsigned int v12;  // edi
    unsigned int v13;  // ftop
    st_43a810_0 *v14;  // eax
    unsigned int v15;  // ftop
    st_43a810_0 *v0;  // [bp-0x14], Other Possible Types: unsigned int
    st_43a810_0 *v1;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v4 = a0;
    v3 = v6;
    if (idx->field_48 == 2)
        return 0;
    v7 = g_4b43dc;
    v2 = v8;
    if (!g_4b43dc)
        idx->field_54 = NULL;
    if (idx->field_4 < 1000 && idx->field_4 >= 5)
    {
        if (g_4b43dc)
        {
            v10 = v9 - 1;
            if (/* unsupported instruction */)
            {
                v11 = v10 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v11 = v10 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v1 = v12;
            v0 = g_4b43dc;
            if (/* unsupported instruction */)
            {
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
                v13 = v11 + 1;
            }
            else
            {
                v0 = nan;
                /* unsupported instruction */
                v13 = v11 + 1;
            }
            v14 = sub_41a9f0();
            idx->field_54 = v14;
            if (v14)
                idx->field_64 = v14->field_27b4;
        }
        if (!sub_412500())
        {
            idx->field_54 = NULL;
            idx->field_64 = 0;
        }
        v7 = idx->field_54;
        if (v7)
        {
            v15 = v13 - 1;
            if (/* unsupported instruction */)
            {
                v16 = v15 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v16 = v15 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v17 = v16 - 1;
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
            v20 = v18 + 2;
            v21 = _ccall(10, 13, (char)((((unsigned short)v20 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v21 & 1))
            {
                v22 = v20 - 1;
                if (/* unsupported instruction */)
                {
                    v23 = v22 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v23 = v22 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v24 = v23 - 1;
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
                v27 = v25 + 2;
                if (!(((v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v28 = idx->field_4c;
                    idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    idx->field_20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    idx->field_24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    sub_461970(v28, v1);
                    sub_4067e0(1000);
                    v29 = v27 - 1;
                    if (/* unsupported instruction */)
                    {
                        v30 = v29 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v30 = v29 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    v31 = v30 - 1;
                    if (/* unsupported instruction */)
                    {
                        v32 = v31 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v32 = v31 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((char)(((v32 + 2 & 7) * 0x800 | (unsigned short)(CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500) & 0x4700) >> 8) & 65)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else if (/* unsupported instruction */)
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
                        v3 = /* unsupported instruction */;
                        /* unsupported instruction */
                    }
                    else
                    {
                        v3 = nan;
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
                        idx->field_70 = /* unsupported instruction */;
                        /* unsupported instruction */
                    }
                    else
                    {
                        idx->field_70 = nan;
                        /* unsupported instruction */
                    }
                }
            }
        }
    }
    if (idx->field_4 != 1004)
        return 0;
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
    v1 = v7;
    if (/* unsupported instruction */)
    {
        idx->field_2c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_2c = nan;
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
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
        /* unsupported instruction */
    }
    sub_4646e0();
    v0 = v34;
    if (/* unsupported instruction */)
    {
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    sub_4646e0();
    if (!/* unsupported instruction */)
    {
        idx->field_30 = nan;
        /* unsupported instruction */
        return 0;
    }
    idx->field_30 = /* unsupported instruction */;
    /* unsupported instruction */
    return 0;
}
