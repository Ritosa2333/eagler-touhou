// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41a9f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41a9f0_1 {
    struct st_41a9f0_0 *field_0;
    struct st_41a9f0_1 *field_4;
} st_41a9f0_1;

typedef struct st_41a9f0_0 {
    char padding_0[9976];
    unsigned int field_26f8;
} st_41a9f0_0;

typedef struct st_41a9f0_2 {
    char padding_0[104];
    struct st_41a9f0_1 *field_68;
} st_41a9f0_2;

extern st_41a9f0_2 *g_4b43dc;

st_41a9f0_0 * sub_41a9f0(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v1;  // ftop
    unsigned int v2;  // ftop
    unsigned int v11;  // ftop
    unsigned int v13;  // ftop
    unsigned int v3;  // ftop
    st_41a9f0_1 *v4;  // eax
    st_41a9f0_0 *v5;  // esi
    unsigned int v6;  // ftop
    st_41a9f0_1 *v7;  // eax
    st_41a9f0_0 *v8;  // ecx
    st_41a9f0_1 *v9;  // edx
    unsigned int v10;  // ftop
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    v2 = v1 - 1;
    if (/* unsupported instruction */)
    {
        v3 = v2 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v3 = v2 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v4 = g_4b43dc->field_68;
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = NULL;
    a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v6 = v3 + 1;
    if (!g_4b43dc->field_68)
        return NULL;
    do
    {
        v7 = v4;
        v8 = v7->field_0;
        v9 = v7->field_4;
        if (!((char)v8->field_26f8 & 33) && !(v8->field_26f8 & 0x6000000))
        {
            v10 = v6 - 1;
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
            /* unsupported instruction */
            v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = v11 - 0;
            if (!((char)((((unsigned short)v13 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                if (/* unsupported instruction */)
                {
                    a2 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v6 = v13 + 1;
                }
                else
                {
                    a2 = nan;
                    /* unsupported instruction */
                    v6 = v13 + 1;
                }
                v5 = v8;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = v13 + 1;
            }
        }
    } while ((v4 = v9, v7->field_4));
    return v5;
}
