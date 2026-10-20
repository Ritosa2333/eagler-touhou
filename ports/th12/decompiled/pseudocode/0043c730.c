// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43c730; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43c730_1 {
    unsigned short field_0;
    unsigned short field_2;
    char padding_4[152];
    unsigned int field_9c;
} st_43c730_1;

typedef struct st_43c730_2 {
    char padding_0[2];
    unsigned short field_2;
    char padding_4[8];
    unsigned int field_c;
    unsigned short field_10;
    char padding_12[2];
    unsigned int field_14;
    unsigned short field_18;
    unsigned short field_1a;
    unsigned short field_1c;
    unsigned short field_1e;
    char padding_20[12];
    unsigned int field_2c;
    char padding_30[8];
    unsigned int field_38;
} st_43c730_2;

typedef struct st_43c730_0 {
    char padding_0[16];
    unsigned int field_10;
} st_43c730_0;

extern unsigned int g_4b0c44;
extern unsigned int g_4b0c48;
extern unsigned int g_4b0c4c;
extern unsigned int g_4b0c50;
extern unsigned int g_4b0c54;
extern unsigned int g_4b0c58;
extern unsigned int g_4b0c5c;
extern unsigned int g_4b0c78;
extern unsigned int g_4b0c98;
extern unsigned int g_4b0c9c;
extern unsigned int g_4b0ca0;
extern unsigned int g_4b0ca4;
extern void g_4b0cb0;
extern unsigned int g_4b0cc4;
extern unsigned int g_4b0ccc;
extern char g_4b0cd0;
extern int g_4b0cd4;
extern unsigned int g_4b0cd8;
extern st_43c730_0 *g_4b4518;
extern unsigned short g_4ce568;
extern unsigned int g_4ce56c;
extern unsigned int g_4cee4c;

void sub_43c730(void)
{
    int v1;  // ebx
    st_43c730_1 *index;  // eax
    unsigned int v3;  // edx
    unsigned int v4;  // eax
    unsigned int v5;  // edx
    st_43c730_2 *idx;  // ebx
    unsigned int *idx1;  // eax
    int v8;  // ecx

    if (!g_4b4518->field_10)
    {
        v1 = sub_46c9ea(160);
        if (v1)
            sub_477420(v1, 0, 160);
        else
            v1 = 0;
        *((int *)&g_4b4518[1].padding_0[12 + 4 * *((int *)&g_4b0cb0)]) = v1;
        index = *((int *)&g_4b4518[1].padding_0[12 + 4 * *((int *)&g_4b0cb0)]);
        index->field_2 = g_4ce568;
        g_4ce56c = 0;
        v3 = index->field_9c;
        index->field_0 = *((short *)&g_4b0cb0);
        index->field_9c = index->field_9c ^ (v3 ^ g_4cee4c) & 1;
        return;
    }
    else if (g_4b4518->field_10 == 1)
    {
        v4 = *((int *)&g_4b0cb0) * 9;
        v5 = (&g_4b4518[8].field_10)[v4];
        idx = *((int *)&g_4b4518[9].padding_0[4 + 4 * v4]);
        idx1 = &g_4b4518->padding_0[4 * v4 + 168];
        idx1[1] = *((int *)&g_4b4518[8].padding_0[8 + 4 * v4]);
        idx1[3] = v5;
        idx1[5] = 0xffffffff;
        g_4ce568 = idx->field_2;
        g_4ce56c = 0;
        g_4b0c44 = idx->field_c;
        v8 = *((int *)&g_4b0cd0);
        g_4b0c48 = idx->field_10;
        if (g_4b0c48 > *((int *)&g_4b0cd0) || (v8 = g_4b0cd4, g_4b0c48 < g_4b0cd4))
            g_4b0c48 = v8;
        g_4b0c78 = idx->field_14;
        g_4b0c98 = idx->field_18;
        g_4b0c9c = idx->field_1a;
        g_4b0ccc = idx->field_2c;
        g_4b0cc4 = idx->field_38;
        g_4b0cd8 = *((int *)(*((int *)&g_4b4518->padding_0[36 * *((int *)&g_4b0cb0) + 184]) + 60));
        g_4b0ca0 = idx->field_1c;
        g_4b0ca4 = idx->field_1e;
        g_4b0c54 = 0;
        g_4b0c50 = 0;
        g_4b0c4c = 0;
        g_4b0c58 = 0;
        g_4b0c5c = 0;
        sub_422e80();
        sub_422e80();
        sub_422e80();
        return;
    }
    else
    {
        return;
    }
}
