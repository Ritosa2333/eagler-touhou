// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ab60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44ab60_0 {
    char padding_0[56];
    unsigned int field_38;
} st_44ab60_0;

typedef struct st_44ab60_1 {
    unsigned int field_0;
    struct st_44ab60_1 *field_4;
} st_44ab60_1;

typedef struct st_44ab60_2 {
    char padding_0[104];
    struct st_44ab60_1 *field_68;
} st_44ab60_2;

extern st_44ab60_2 *g_4b43dc;

st_44ab60_1 * sub_44ab60(unsigned int a0, st_44ab60_0 *a1)
{
    st_44ab60_1 *iter;  // eax
    unsigned short v4;  // ftop
    st_44ab60_1 *v13;  // eax
    unsigned short v5;  // ftop
    unsigned short v7;  // ftop
    unsigned int v8;  // 4292
    unsigned int v9;  // fc3210
    st_44ab60_1 *v11;  // eax
    unsigned int v12;  // 4145
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    if (a1->field_38)
    {
        iter = g_4b43dc->field_68;
        if (iter)
        {
            do
            {
                if (*((int *)(iter->field_0 + 10164)) == a1->field_38)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
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
                    v5 = v4 - 3;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v7 = v5 - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v8 = _ccall(10, 13, (char)(((v5 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                    if (!(v8 & 1))
                    {
                        v9 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
                        v11 = _INSERT(_INSERT(CONCAT(((char)v5 & 7) * 0, 0), 0, ((char)v7 & 7) * 0), 1, ((v7 & 7) * 0x800 | (unsigned short)v9 & 0x4700) >> 8);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v12 = _ccall(10, 13, (char)(((v7 & 7) * 0x800 | (unsigned short)v9 & 0x4700) >> 8) & 0x44, 0, 0);
                        if (!(v12 & 1))
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            return v11;
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
                    v13 = sub_4937aa();
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
                        return v13;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    return v13;
                }
            } while ((iter = (st_44ab60_1 *)iter->field_4, iter));
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    return iter;
}
