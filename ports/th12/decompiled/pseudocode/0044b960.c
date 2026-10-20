// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44b960; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44b960_6 {
    unsigned int field_0;
    int field_4;
    char padding_8[4];
    struct st_44b960_7 *field_c;
} st_44b960_6;

typedef struct st_44b960_0 {
    struct st_44b960_1 *field_0;
    char padding_4[4];
    struct st_44b960_1 *field_8;
    char padding_c[8];
    struct st_44b960_1 *field_14;
    struct st_44b960_1 *field_18;
    struct st_44b960_1 *field_1c;
} st_44b960_0;

typedef struct st_44b960_7 {
    struct st_44b960_0 *field_0;
} st_44b960_7;

typedef struct st_44b960_1 {
    unsigned int field_0;
} st_44b960_1;

char sub_44b960(unsigned int a0, int a1)
{
    st_44b960_6 *idx;  // esi
    unsigned int v5;  // edi
    unsigned int v6;  // ebp
    unsigned int v7;  // ebx
    unsigned int v2;  // [bp-0x14]

    v2 = 0;
    if (!idx->field_c)
        return 0;
    if ((char)idx->field_c->field_0->field_0(a1, "r", v5, v6, v7) && idx->field_c->field_0->field_8(&v7, 16))
        sub_4639d0(&v5, 16, 55, 16, 16);
    if (idx->field_c)
        idx->field_c->field_0->field_1c(1);
    idx->field_c = NULL;
    return 0;
}
