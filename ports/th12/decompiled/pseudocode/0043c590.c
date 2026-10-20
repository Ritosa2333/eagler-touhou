// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43c590; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43c590_5 {
    char padding_0[64];
    unsigned int field_40;
} st_43c590_5;

typedef struct st_43c590_6 {
    char padding_0[12];
    unsigned int field_c;
    unsigned short field_10;
    char padding_12[2];
    unsigned int field_14;
    unsigned short field_18;
    unsigned short field_1a;
    unsigned short field_1c;
    unsigned short field_1e;
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    unsigned int field_34;
    unsigned int field_38;
    unsigned int field_3c;
    char padding_40[4];
    unsigned int field_44;
} st_43c590_6;

typedef struct st_43c590_0 {
    char padding_0[4];
    unsigned int field_4;
} st_43c590_0;

typedef struct st_43c590_3 {
    char padding_0[8];
    struct st_43c590_0 *field_8;
    struct st_43c590_0 *field_c;
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[136];
    unsigned int field_a0;
    char padding_a4[300];
    unsigned int field_1d0;
    struct st_43c590_0 *field_1d4;
    unsigned int field_1d8;
} st_43c590_3;

typedef struct st_43c590_4 {
    char padding_0[2440];
    unsigned int field_988;
    unsigned int field_98c;
    char padding_990[48136];
    unsigned int field_c598;
} st_43c590_4;

extern unsigned int g_4b0c44;
extern unsigned short g_4b0c48;
extern unsigned int g_4b0c4c;
extern unsigned int g_4b0c50;
extern unsigned int g_4b0c54;
extern unsigned int g_4b0c78;
extern unsigned short g_4b0c98;
extern unsigned short g_4b0c9c;
extern unsigned short g_4b0ca0;
extern unsigned short g_4b0ca4;
extern char g_4b0cb0;
extern unsigned int g_4b0cc4;
extern unsigned int g_4b0ccc;
extern unsigned int g_4b0cd8;
extern unsigned int g_4b0cdc;
extern st_43c590_4 *g_4b4514;
extern st_43c590_3 *g_4b4518;
extern unsigned int g_4cee4c;

void sub_43c590(unsigned int a0)
{
    st_43c590_0 *idx;  // eax
    st_43c590_0 *idx1;  // eax
    st_43c590_0 *idx2;  // eax
    st_43c590_6 *v5;  // esi
    unsigned int v6;  // eax
    st_43c590_5 *v7;  // esi
    unsigned int v8;  // eax
    unsigned int v9;  // ecx
    unsigned int *index;  // eax
    unsigned int v11;  // ecx
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    idx = g_4b4518->field_8;
    if (idx)
        idx->field_4 = idx->field_4 | 2;
    idx1 = g_4b4518->field_1d4;
    if (idx1)
        idx1->field_4 = idx1->field_4 | 2;
    idx2 = g_4b4518->field_c;
    if (idx2)
        idx2->field_4 = idx2->field_4 | 2;
    if (!g_4b4518->field_10)
    {
        v5 = *((int *)&g_4b4518->padding_18[8 + 4 * *((int *)&g_4b0cb0)]);
        sub_43ccd0();
        v6 = sub_43cbb0();
        g_4b4518->field_a0 = v6;
        if (!g_4cee4c)
        {
            v5->field_c = g_4b0c44;
            v5->field_10 = g_4b0c48;
            v5->field_14 = g_4b0c78;
            v5->field_18 = g_4b0c98;
            v5->field_1a = g_4b0c9c;
            v5->field_2c = g_4b0ccc;
            v5->field_38 = g_4b0cc4;
            v5->field_44 = g_4b0cdc;
            v5->field_1c = g_4b0ca0;
            v5->field_1e = g_4b0ca4;
            v5->field_20 = g_4b0c4c;
            v5->field_24 = g_4b0c50;
            v5->field_28 = g_4b0c54;
        }
        v5->field_30 = g_4b4514->field_988;
        v5->field_34 = g_4b4514->field_98c;
        v5->field_3c = g_4b0cd8;
        g_4b4518->field_1d0 = 0;
        g_4b4518->field_1d8 = *((int *)&g_4b0cb0);
        *((unsigned int *)(*((int *)&g_4b4518->padding_18[8 + 4 * *((int *)&g_4b0cb0)]) + 64)) = g_4b4514->field_c598;
        g_4b4518->field_1d0 = 0;
        return;
    }
    else
    {
        if (g_4b4518->field_10 == 1)
        {
            v7 = *((int *)&g_4b4518->padding_a4[20 + 36 * *((int *)&g_4b0cb0)]);
            g_4b4518->field_1d8 = *((int *)&g_4b0cb0);
            sub_43add0();
            g_4b4514->field_c598 = v7->field_40;
            v8 = *((int *)&g_4b0cb0);
            if (*((int *)&g_4b0cb0) == 3)
            {
                g_4b4518->field_14 = 1;
                v8 = *((int *)&g_4b0cb0);
            }
            v9 = v8 * 9;
            index = &g_4b4518->padding_0[4 * v9 + 168];
            v11 = index[2];
            index[1] = *((int *)&g_4b4518->padding_a4[4 + 4 * v9]);
            index[3] = v11;
            index[5] = 0;
        }
        g_4b4518->field_1d0 = 0;
        return;
    }
}
