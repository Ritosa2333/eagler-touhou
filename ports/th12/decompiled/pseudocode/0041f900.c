// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41f900; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41f900_11 {
    struct st_41f900_9 *field_0;
    char padding_4[4];
    struct st_41f900_11 *field_8;
    unsigned int field_c;
} st_41f900_11;

typedef struct st_41f900_0 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[4];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    char padding_24[4];
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    unsigned int field_34;
    char padding_38[4];
    unsigned int field_3c;
    char padding_40[12];
    unsigned int field_4c;
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[4];
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    unsigned int field_74;
    unsigned int field_78;
    unsigned int field_7c;
    unsigned int field_80;
    unsigned int field_84;
    unsigned int field_88;
    char padding_8c[8];
    unsigned int field_94;
    unsigned int field_98;
    unsigned int field_9c;
    unsigned int field_a0;
    unsigned int field_a4;
    unsigned int field_a8;
} st_41f900_0;

typedef struct st_41f900_9 {
    char padding_0[20];
    struct st_41f900_10 *field_14;
} st_41f900_9;

typedef struct st_41f900_10 {
    unsigned int field_0;
} st_41f900_10;

typedef struct st_41f900_1 {
    char padding_0[1180];
    char field_49c;
} st_41f900_1;

typedef struct st_41f900_2 {
    char padding_0[1181];
    char field_49d;
} st_41f900_2;

typedef struct st_41f900_12 {
    char padding_0[24];
    struct st_41f900_11 *field_18;
} st_41f900_12;

extern char g_4b2ed0;
extern st_41f900_12 *g_4b44f4;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee70;

st_41f900_0 * sub_41f900(unsigned int a0)
{
    st_41f900_0 *idx;  // esi
    int v10;  // edi
    st_41f900_2 *v19;  // eax
    st_41f900_1 *v20;  // eax
    st_41f900_2 *v21;  // eax
    st_41f900_1 *v22;  // eax
    st_41f900_2 *v23;  // eax
    st_41f900_11 *v24;  // ecx
    st_41f900_11 *v25;  // ecx
    int v11;  // ebp
    int v12;  // ebx
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    st_41f900_1 *v15;  // eax
    unsigned int v16;  // ecx
    st_41f900_2 *v17;  // eax
    st_41f900_1 *v18;  // eax
    unsigned int v0;  // [bp-0x4c]
    unsigned int v1;  // [bp-0x48]
    unsigned int v2;  // [bp-0x44]
    unsigned int v3;  // [bp-0x2c]
    unsigned int v4;  // [bp-0x28]
    unsigned int v5;  // [bp-0x24]
    unsigned int v6;  // [bp-0x20]
    unsigned int v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0xc]

    idx->field_14 = idx->field_14 & 0xfffffffe;
    idx->field_28 = idx->field_28 & 0xfffffffe;
    idx->field_3c = idx->field_3c & 0xfffffffe;
    sub_477420(idx, 0, 172, v10, v11, v12);
    /* unsupported instruction */
    /* unsupported instruction */
    v13 = idx->field_14;
    if (!((char)idx->field_14 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_c = /* unsupported instruction */;
        else
            idx->field_c = nan;
        idx->field_8 = 0;
        idx->field_4 = 0xfff0bdc1;
        *((char **)&idx->padding_10[0]) = &g_4b2ed0;
        idx->field_14 = v13 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_c = /* unsupported instruction */;
    else
        idx->field_c = nan;
    idx->field_8 = 0;
    idx->field_4 = 0xffffffff;
    if (!((char)idx->field_28 & 1))
        return sub_41f960();
    if (/* unsupported instruction */)
        idx->field_20 = /* unsupported instruction */;
    else
        idx->field_20 = nan;
    idx->field_1c = 0;
    idx->field_18 = 0xffffffff;
    idx->field_60 = 0;
    v14 = idx->field_3c;
    if (!((char)idx->field_3c & 1))
    {
        if (/* unsupported instruction */)
            idx->field_34 = /* unsupported instruction */;
        else
            idx->field_34 = nan;
        idx->field_30 = 0;
        idx->field_2c = 0xfff0bdc1;
        *((char **)&idx->padding_38[0]) = &g_4b2ed0;
        idx->field_3c = v14 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_34 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_34 = nan;
        /* unsupported instruction */
    }
    v7 = 0;
    idx->field_30 = 0;
    idx->field_2c = 0xffffffff;
    v6 = 0;
    idx->field_64 = a0;
    v5 = &a0;
    v4 = g_4cee70;
    sub_4615a0();
    v3 = 0;
    idx->field_4c = v8;
    sub_4615a0(g_4cee70, &v8, 1, 0);
    idx->field_50 = 0;
    v15 = sub_461920(idx->field_4c, g_4ce8cc, idx->field_4c);
    if (!v15)
        idx->field_4c = 0;
    v15->field_49c = 16;
    v17 = sub_461920(v16, g_4ce8cc, idx->field_4c);
    if (!v17)
        idx->field_4c = 0;
    v17->field_49d = 16;
    v18 = sub_461920(v16, g_4ce8cc, idx->field_50);
    if (!v18)
        idx->field_50 = 0;
    v18->field_49c = 16;
    v19 = sub_461920(idx->field_50, g_4ce8cc, idx->field_50);
    if (!v19)
        idx->field_50 = 0;
    v19->field_49d = 16;
    sub_4615a0(g_4cee70, &v7, 2, 0);
    idx->field_54 = 0;
    sub_4615a0(g_4cee70, &v3, 3, 0);
    idx->field_58 = 0;
    v20 = sub_461920(0, g_4ce8cc, idx->field_54);
    if (!v20)
        idx->field_54 = 0;
    v20->field_49c = 16;
    v21 = sub_461920(v16, g_4ce8cc, idx->field_54);
    if (!v21)
        idx->field_54 = 0;
    v21->field_49d = 16;
    v22 = sub_461920(idx->field_58, g_4ce8cc, idx->field_58);
    if (!v22)
        idx->field_58 = 0;
    v22->field_49c = 16;
    v23 = sub_461920(v16, g_4ce8cc, idx->field_58);
    if (!v23)
        idx->field_58 = 0;
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
    v23->field_49d = 16;
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_68 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_70 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_74 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_78 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_7c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_80 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_94 = 0;
    idx->field_98 = 0;
    idx->field_a0 = 16314511;
    idx->field_a4 = 0x8088ff;
    idx->field_a8 = 0xd8d8d8;
    idx->field_9c = 0;
    idx->field_88 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    sub_40d230();
    v24 = g_4b44f4->field_18;
    if (g_4b44f4->field_18)
    {
        do
        {
            v25 = v24;
            if (v25->field_c != 1)
                v25->field_0->field_14(0, 0);
        } while ((v24 = (st_41f900_11 *)v25->field_8, v25->field_8));
    }
    sub_414c60();
    return idx;
}
