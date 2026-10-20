// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4527f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4527f0_0 {
    char padding_0[32];
    int field_20;
    unsigned int field_24;
    int field_28;
} st_4527f0_0;

typedef struct st_4527f0_1 {
    char padding_0[96];
    unsigned int field_60;
} st_4527f0_1;

extern st_4527f0_1 *g_4b44e8;
extern unsigned int g_4ce55c;
extern unsigned int g_4cee04;
extern unsigned int g_4cee08;

unsigned int sub_4527f0(st_4527f0_0 *idx)
{
    unsigned int v2;  // eax
    int v3;  // esi
    unsigned int v4;  // edx
    unsigned int v5;  // ecx
    unsigned int v7;  // edx
    unsigned int v9;  // edx
    st_4527f0_0 *v0;  // [bp-0x4], Other Possible Types: int, unsigned int

    v0 = idx;
    if (g_4ce55c)
    {
        return 7;
    }
    else if (g_4b44e8)
    {
        v2 = g_4b44e8->field_60;
        if ((char)(v2 >> 2 | v2) & 1)
        {
            return 1;
        }
        else if (!((char)v2 & 114))
        {
            sub_464a80(v3);
            v4 = *((int *)&idx[1].padding_0[8]);
            v0 = idx->field_20;
            if (v4 < v0)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v5 = idx->field_24;
                if (v4 < v5 + v0)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else if (v4 < v5 + v0 + idx->field_28)
                {
                    v0 += v5 + idx->field_28;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if (v0 < 0)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v0 = idx->field_28;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if (v0 < 0)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    return 7;
                }
            }
            v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = sub_464440() % 3;
            if (!v7)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                g_4cee04 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else if (v7 == 1)
            {
LABEL_4528d9:
                g_4cee04 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else if (v7 == 2)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_4528d9;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v9 = sub_464440() % 3;
            if (!v9)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                g_4cee08 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                return 1;
            }
            else if (v9 != 1)
            {
                if (v9 != 2)
                    return 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                g_4cee08 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                return 1;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                g_4cee08 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}
