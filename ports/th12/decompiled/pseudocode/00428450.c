// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428450; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428450_2 {
    struct st_428450_0 *field_0;
    struct st_428450_2 *field_4;
    struct st_428450_2 *field_8;
    char padding_c[116];
    unsigned int field_80;
} st_428450_2;

typedef struct st_428450_1 {
    unsigned int field_0;
} st_428450_1;

typedef struct st_428450_0 {
    char padding_0[4];
    struct st_428450_1 *field_4;
} st_428450_0;

typedef struct st_428450_3 {
    char padding_0[1124];
    struct st_428450_2 *field_464;
    int field_468;
    int field_46c;
} st_428450_3;

extern st_428450_3 *g_4b44f4;

int sub_428450(unsigned int a0)
{
    unsigned int v1;  // ebx
    st_428450_2 *idx;  // eax
    st_428450_2 *v3;  // ecx
    unsigned int v4;  // edi
    unsigned int v0;  // [bp-0x8]

    if (g_4b44f4->field_468 >= 0x100)
        return 0;
    v0 = v1;
    g_4b44f4->field_46c = g_4b44f4->field_46c + 1;
    if (g_4b44f4->field_46c < 0x10000)
        g_4b44f4->field_46c = 0x10000;
    if (a0)
    {
        if (a0 != 1)
        {
            if (a0 != 2)
                return g_4b44f4->field_46c;
            if (!sub_46c9ea(4004))
                goto LABEL_4284e5;
            idx = sub_4285b0();
        }
        else
        {
            if (!sub_46c9ea(4040))
                goto LABEL_4284e5;
            idx = sub_428560();
        }
    }
    else
    {
        if (sub_46c9ea(4004))
        {
            idx = sub_428520();
        }
        else
        {
LABEL_4284e5:
            idx = NULL;
        }
    }
    idx->field_80 = g_4b44f4->field_46c;
    v3 = g_4b44f4->field_464;
    idx->field_4 = v3;
    v3->field_8 = idx;
    g_4b44f4->field_468 = g_4b44f4->field_468 + 1;
    g_4b44f4->field_464 = idx;
    idx->field_0->field_4(v4);
    return g_4b44f4->field_46c;
}
