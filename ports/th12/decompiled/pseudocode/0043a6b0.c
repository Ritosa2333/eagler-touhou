// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43a6b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43a6b0_6 {
    char padding_0[27952];
    unsigned int field_6d30;
} st_43a6b0_6;

typedef struct st_43a6b0_0 {
    char padding_0[28];
    char field_1c;
} st_43a6b0_0;

typedef struct st_43a6b0_1 {
    char padding_0[20];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[44];
    int field_4c;
    int field_50;
    char padding_54[24];
    unsigned int field_6c;
    char padding_70[4];
    struct st_43a6b0_0 *field_74;
} st_43a6b0_1;

typedef struct st_43a6b0_3 {
    unsigned int field_0;
} st_43a6b0_3;

typedef struct st_43a6b0_2 {
    char padding_0[64];
    struct st_43a6b0_3 *field_40;
} st_43a6b0_2;

typedef struct st_43a6b0_4 {
    struct st_43a6b0_2 *field_0;
} st_43a6b0_4;

typedef struct st_43a6b0_5 {
    char padding_0[50212];
    int field_c424;
} st_43a6b0_5;

extern unsigned int g_4b43dc;
extern st_43a6b0_6 *g_4b43e4;
extern st_43a6b0_5 *g_4b4514;
extern st_43a6b0_4 *g_4d1068;

unsigned int sub_43a6b0(unsigned int a0, st_43a6b0_1 *idx)
{
    unsigned int v7;  // esi
    unsigned int v8;  // ftop
    unsigned int v17;  // edi
    unsigned int v18;  // eax
    unsigned short v19;  // ftop
    unsigned short v20;  // ftop
    unsigned int v21;  // fc3210
    unsigned short v22;  // ftop
    unsigned int v23;  // 4116
    unsigned short v9;  // ftop
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v12;  // ftop
    unsigned int v13;  // 4101
    unsigned short v14;  // ftop
    unsigned short v15;  // ftop
    unsigned int v16;  // ebx
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    v2 = v7;
    v9 = v8 - 1;
    if (/* unsupported instruction */)
    {
        v10 = v9 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v10 = v9 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v11 = v10 - 1;
    if (/* unsupported instruction */)
    {
        v12 = v11 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v12 = v11 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v13 = *((int *)&idx->padding_20[40]);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_14 = v3;
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_18 = v4;
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_1c = v5;
    if (v13 == 2)
        return 0;
    v14 = v12 + 1;
    if (/* unsupported instruction */)
    {
        v15 = v14 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v15 = v14 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v1 = v16;
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
    v0 = v17;
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
    v18 = sub_4931e0();
    g_4d1068->field_0->field_40(g_4d1068, v18);
    v19 = v15 - 1;
    if (/* unsupported instruction */)
    {
        v20 = v19 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v20 = v19 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v21 = CmpF(/* unsupported instruction */, 0x407c000000000000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v22 = v20 + 1;
    }
    else
    {
        v21 = CmpF(nan, 0x407c000000000000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v22 = v20 + 1;
    }
    v23 = _ccall(10, 13, (char)(((v22 & 7) * 0x800 | (unsigned short)v21 & 0x4700) >> 8) & 5, 0, 0);
    if (!(v23 & 1))
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
            idx->field_6c = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_6c = nan;
            /* unsupported instruction */
        }
    }
    if (*((int *)&idx->padding_20[40]) == 1 && (g_4b4514->field_c424 < 0 || idx->field_74->field_1c - 1 >= *((int *)&g_4b4514->padding_0[50204]) || g_4b43e4 && g_4b43e4->field_6d30 || !g_4b43dc))
    {
        *((unsigned int *)&idx->padding_20[40]) = 2;
        sub_461970(idx->field_4c);
        sub_461970(idx->field_50);
        *((unsigned int *)&g_4b4514[1].padding_0[8 + 4 * idx->field_74->field_1c]) = 0;
        sub_453f10(idx->field_50);
    }
    if (*((int *)&idx->padding_54[4]) || *((int *)&idx->padding_20[40]) != 1 || *((int *)&idx->padding_54[8]) != 1)
        return 0;
    sub_461970(idx->field_4c);
    *((unsigned int *)&idx->padding_54[8]) = 0;
    return 0;
}
