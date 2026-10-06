// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d870; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43d870_0 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    unsigned int field_38;
    unsigned int field_3c;
    char padding_40[196];
    unsigned int field_104;
    char padding_108[4];
    int field_10c;
    int field_110;
    unsigned int field_114;
    char padding_118[196];
    unsigned int field_1dc;
} st_43d870_0;

extern unsigned int g_4cee40;
extern unsigned int g_4cee78;
extern char g_4d48c0;
extern void g_4d48c4;

int sub_43d870(void)
{
    unsigned int v3;  // esi
    st_43d870_0 *idx;  // eax
    st_43d870_0 *v12;  // esi
    unsigned int v5;  // eax
    unsigned int v6;  // 4101
    unsigned int v7;  // eax
    unsigned int v8;  // 4101
    unsigned int v9;  // edi
    st_43d870_0 *v10;  // edi
    unsigned int v11;  // ecx
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = v3;
    if (!idx->field_30)
    {
        v5 = idx->field_3c;
        if (v5)
        {
            v6 = _ccall(14, 15, v5, 0, 0);
            if (v6 & 1)
                idx->field_34 = v5 - 1;
            else
                idx->field_34 = 0;
        }
        else
        {
            idx->field_34 = 0;
        }
        idx->field_3c = 2;
        idx->field_104 = 1;
        v7 = idx->field_114;
        if (v7)
        {
            v8 = _ccall(14, 15, v7, 0, 0);
            if (v8 & 1)
                idx->field_10c = v7 - 1;
            else
                idx->field_10c = 0;
        }
        else
        {
            idx->field_10c = 0;
        }
        idx->field_114 = 60;
        idx->field_1dc = 1;
        idx->field_30 = 1;
    }
    else if (idx->field_30 != 1)
    {
        return;
    }
    v0 = v9;
    v10 = &idx->field_34;
    *((unsigned int *)&v10->padding_0[4]) = idx->field_34;
    if (g_4d48c4 & 32 || g_4d48c0 & 32)
        sub_464970(1);
    if (g_4d48c4 & 16 || g_4d48c0 & 16)
        sub_464970(-0x1);
    if (!*((int *)&v10->padding_0[0]))
    {
        v12 = &idx->field_10c;
        *((int *)&v12->padding_0[4]) = idx->field_10c;
        if (g_4d48c4 & 128 || g_4d48c0 & 128)
            sub_464970(1);
        if (g_4d48c4 & 64 || g_4d48c0 & 64)
            sub_464970(-0x1);
        if (*((int *)&g_4d48c4) & 524289)
            sub_453d90(v11, *((int *)&v12->padding_0[0]));
        if (*((int *)&g_4d48c4) & 258)
            sub_453f10();
    }
    else if (*((int *)&v10->padding_0[0]) == 1)
    {
        if (!(*((int *)&g_4d48c4) & 524289))
            return;
        g_4cee40 = ~(g_4cee78 >> 13) & *((int *)&v10->padding_0[0]) | 2;
        return;
    }
    return;
}
