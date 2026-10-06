// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428750; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428750_2 {
    struct st_428750_0 *field_0;
    char padding_4[4];
    struct st_428750_2 *field_8;
    unsigned int field_c;
} st_428750_2;

typedef struct st_428750_0 {
    char padding_0[20];
    struct st_428750_1 *field_14;
} st_428750_0;

typedef struct st_428750_1 {
    unsigned int field_0;
} st_428750_1;

typedef struct st_428750_3 {
    char padding_0[24];
    struct st_428750_2 *field_18;
} st_428750_3;

extern st_428750_3 *g_4b44f4;

unsigned int sub_428750(void)
{
    st_428750_2 *v2;  // ecx
    unsigned int v3;  // esi
    st_428750_2 *v4;  // ecx
    st_428750_2 *v5;  // esi
    unsigned int v6;  // ebx
    unsigned int v7;  // edi
    unsigned int v0;  // [bp-0x4]

    v2 = g_4b44f4->field_18;
    if (!g_4b44f4->field_18)
        return 1;
    v0 = v3;
    do
    {
        v4 = v2;
        v5 = v4->field_8;
        if (v4->field_c != 1)
            v4->field_0->field_14(v6, v7);
    } while ((v2 = v5, v4->field_8));
    return 1;
}
