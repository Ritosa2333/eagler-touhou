// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43b7e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43b7e0_3 {
    struct st_43b7e0_0 *field_0;
} st_43b7e0_3;

typedef struct st_43b7e0_2 {
    char padding_0[160];
    struct st_43b7e0_3 *field_a0;
    char padding_a4[300];
    int field_1d0;
} st_43b7e0_2;

typedef struct st_43b7e0_0 {
    char padding_0[6304];
    struct st_43b7e0_1 *field_18a0;
} st_43b7e0_0;

typedef struct st_43b7e0_1 {
    char field_0;
} st_43b7e0_1;

extern unsigned int g_4b44e8;
extern unsigned int g_4ceae8;
extern unsigned short g_4d48b8;
extern unsigned int g_4d494c;
extern unsigned int g_4d4958;
extern unsigned int g_4d49cc;
extern unsigned int g_4d49d0;
extern unsigned int g_4d49d4;
extern unsigned short g_4d49dc;
extern unsigned short g_4d49e0;

int sub_43b7e0(void)
{
    unsigned int v3;  // 4099
    unsigned int v4;  // eax
    unsigned int v13;  // 4136
    char v14;  // cl
    unsigned int v15;  // fpround
    unsigned int v16;  // 4110
    st_43b7e0_0 *index;  // eax
    int v18;  // edi
    unsigned int v5;  // eax
    st_43b7e0_2 *idx;  // eax
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v0;  // [bp-0x6]
    unsigned int v1;  // [bp-0x4]

    if (!g_4b44e8)
        return;
    v3 = g_4ceae8 & 0x200;
    g_4d49d4 = g_4d49d0;
    v4 = g_4d48b8;
    g_4d49d0 = g_4d48b8;
    if (v3)
    {
        if ((char)g_4d48b8 & 8)
        {
            v4 = g_4d48b8 | 0x400;
            g_4d49d0 = v4;
        }
        if ((char)v4 & 1)
        {
            g_4d49cc = g_4d49cc + 1;
            if (g_4d49cc >= 8)
            {
                v5 = v4 | 8;
                g_4d49cc = 8;
LABEL_43b87b:
                g_4d49d0 = v5;
            }
        }
        else
        {
            g_4d49cc = 0;
        }
    }
    else if ((char)g_4d48b8 & 1 && g_4d494c < 5 && (char)g_4d48b8 & 8 && g_4d4958 < 5)
    {
        v5 = g_4d48b8 | 0x400;
        goto LABEL_43b87b;
    }
    sub_40f690();
    if (idx->field_1d0 < 0)
        return;
    if (!(int)(idx->field_1d0 % 30))
    {
        v8 = v7 - 1;
        if (/* unsupported instruction */)
        {
            v9 = v8 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v9 = v8 - 1;
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
        /* unsupported instruction */
        v13 = _ccall(10, 13, (char)(((v11 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
        if (!(v13 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = 0xff;
        }
        else
        {
            v16 = _ccall(v15);
            v0 = v16;
            v1 = v0 | 0xc00;
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
            v14 = v1;
        }
        index = idx->field_a0->field_0;
        index->field_18a0->field_0 = v14;
        index->field_18a0 = index->field_18a0 + 1;
    }
    if (sub_43c9b0(g_4d49dc, g_4d49e0))
        idx->field_a0 = sub_43cbb0(v18);
    idx->field_1d0 = idx->field_1d0 + 1;
    return;
}
