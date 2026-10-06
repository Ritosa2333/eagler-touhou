// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43e720; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43e720_2 {
    struct st_43e720_0 *field_0;
} st_43e720_2;

typedef struct st_43e720_0 {
    unsigned int field_0;
} st_43e720_0;

typedef struct st_43e720_1 {
    struct st_43e720_2 *field_0;
    char padding_4[4];
    int field_8;
    char padding_c[4228];
    unsigned int field_1090;
    unsigned int field_1094;
} st_43e720_1;

typedef struct st_43e720_5 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    char padding_38[220];
    unsigned int field_114;
    char padding_118[212];
    unsigned int field_1ec;
    char padding_1f0[4];
    int field_1f4;
    char padding_1f8[224];
    struct st_43e720_1 *field_2d8;
    struct st_43e720_2 *field_2dc;
    int field_2e0;
} st_43e720_5;

extern char g_49fc28;
extern unsigned int g_4ad138;
extern st_43e720_5 *g_4b4528;

void sub_43e720(void)
{
    int v3;  // esi
    int v4;  // ebx
    unsigned int v5;  // ebx
    int v6;  // eax
    int v7;  // edi
    unsigned int v8;  // eax
    st_43e720_1 *idx;  // edi
    st_43e720_1 *v10;  // ecx
    int v11;  // eax
    int v12;  // eax
    char v0;  // [bp-0x108]
    unsigned int v1;  // [bp-0x4]

    v1 = g_4ad138 ^ &v0;
    sub_46cbbb(&v0, "%s", *((int *)(g_4b4528->field_34 + g_4b4528->field_114 * 4)), v3, v4);
    v5 = 0;
    if (g_4b4528->field_2dc)
    {
        g_4b4528->field_2dc->field_0[5].field_0(1);
        g_4b4528->field_2dc = NULL;
    }
    v6 = g_4b4528->field_2e0;
    g_4b4528->field_2dc = NULL;
    if (v6)
    {
        sub_46c941(v6);
        g_4b4528->field_2e0 = 0;
    }
    v8 = sub_463c10(0, 0, v7);
    g_4b4528->field_2e0 = v8;
    idx = sub_46c9ea(4248);
    if (idx)
    {
        idx->field_0 = &g_49fc28;
        idx->field_1090 = 0;
        idx->field_1094 = 0;
        sub_477420(idx, 0, 4248);
        v10 = idx;
    }
    else
    {
        v10 = NULL;
    }
    v11 = g_4b4528->field_2e0;
    g_4b4528->field_2d8 = v10;
    v10->field_0->field_0(v11);
    v12 = g_4b4528->field_2d8->field_8;
    g_4b4528->field_1f4 = g_4b4528->field_2d8->field_8;
    if (v12 && v12 <= 0)
        v5 = v12 - 1;
    g_4b4528->field_1ec = v5;
    g_4b4528->field_30 = 1;
    _security_check_cookie();
    return;
}
