// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e9f6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e4f1_1 {
    struct st_47e4f1_0 *field_0;
    char field_4;
} st_47e4f1_1;

typedef struct st_47e9f6_0 {
    char padding_0[4];
    char field_4;
} st_47e9f6_0;

typedef struct st_47e4f1_0 {
    char padding_0[4];
    char field_4;
} st_47e4f1_0;

typedef struct st_47dc29_0 {
    struct st_47dc29_0 *field_0;
} st_47dc29_0;

typedef struct st_47dc29_2 {
    unsigned int field_0;
} st_47dc29_2;

typedef struct st_47dc29_1 {
    struct st_47dc29_2 *field_0;
    char padding_4[4];
    struct st_47dc29_0 *field_8;
    struct st_47dc29_0 *field_c;
    unsigned int field_10;
} st_47dc29_1;

extern char g_49ddbc;
extern st_47dc29_1 g_4b42f8;

st_47e4f1_1 * sub_47e9f6(st_47e4f1_1 *a0, int a1, char a2)
{
    unsigned int v2;  // esi
    unsigned int v3;  // ebx
    st_47e9f6_0 *v4;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    if (a0->field_4 > 1)
        return a0;
    v0 = v3;
    if (!a2)
        return a0;
    if (!a0->field_0)
    {
        sub_47e869(a0, a1, a2);
    }
    else
    {
        v4 = sub_47dc29(&g_4b42f8.field_0, a1, 8, 0);
        if (v4)
        {
            *((char **)&v4->padding_0[0]) = &g_49ddbc;
            v4->field_4 = a2;
        }
        else
        {
            v4 = NULL;
        }
        sub_47e26b(v4);
    }
    return a0;
}
