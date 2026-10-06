// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ac00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44ac00_1 {
    unsigned int field_0;
    struct st_44ac00_1 *field_4;
} st_44ac00_1;

typedef struct st_44ac00_0 {
    char padding_0[56];
    unsigned int field_38;
} st_44ac00_0;

typedef struct st_44ac00_2 {
    char padding_0[104];
    struct st_44ac00_1 *field_68;
} st_44ac00_2;

extern st_44ac00_2 *g_4b43dc;
extern st_44ac00_0 *g_4b4534;

st_44ac00_1 * sub_44ac00(void)
{
    st_44ac00_1 *iter;  // eax
    unsigned short v4;  // ftop
    st_44ac00_1 *v13;  // eax
    unsigned int v14;  // 4145
    st_44ac00_1 *v15;  // eax
    unsigned short v5;  // ftop
    unsigned short v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v9;  // ftop
    unsigned int v10;  // 4274
    unsigned int v11;  // fc3210
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    if (g_4b4534->field_38)
    {
        iter = g_4b43dc->field_68;
        if (iter)
        {
            do
            {
                if (*((int *)(iter->field_0 + 10164)) == g_4b4534->field_38)
                {
                    v5 = v4 - 1;
                    if (/* unsupported instruction */)
                    {
                        v6 = v5 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v6 = v5 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v7 = v6 - 2;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v9 = v7 - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v10 = _ccall(10, 13, (char)(((v7 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                    if (!(v10 & 1))
                    {
                        v11 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
                        v13 = _INSERT(_INSERT(CONCAT(((char)v7 & 7) * 0, 0), 0, ((char)v9 & 7) * 0), 1, ((v9 & 7) * 0x800 | (unsigned short)v11 & 0x4700) >> 8);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v14 = _ccall(10, 13, (char)(((v9 & 7) * 0x800 | (unsigned short)v11 & 0x4700) >> 8) & 0x44, 0, 0);
                        if (!(v14 & 1))
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            return v13;
                        }
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v15 = sub_4937aa();
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
                    if (!/* unsupported instruction */)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        return v15;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return v15;
                }
            } while ((iter = (st_44ac00_1 *)iter->field_4, iter));
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    return iter;
}
