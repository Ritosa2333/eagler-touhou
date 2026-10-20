// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bfb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bfb0_0 {
    char padding_0[8];
    struct st_44bfb0_1 *field_8;
    char padding_c[4];
    struct st_44bfb0_1 *field_10;
    struct st_44bfb0_1 *field_14;
    struct st_44bfb0_1 *field_18;
} st_44bfb0_0;

typedef struct st_44bfb0_1 {
    unsigned int field_0;
} st_44bfb0_1;

typedef struct st_44bfb0_5 {
    struct st_44bfb0_0 *field_0;
    char padding_4[4];
    unsigned int field_8;
} st_44bfb0_5;

int sub_44bfb0(st_44bfb0_5 *a0, unsigned int a1, int a2)
{
    unsigned int v1;  // esi
    unsigned int v2;  // ebp
    int v3;  // ebp
    int v4;  // edi
    int v5;  // edi
    unsigned int v6;  // ebx
    unsigned int v7;  // ebx
    unsigned int v0;  // [bp-0x4]

    v0 = v1;
    if (a0->field_8 != 0x80000000)
        return 0;
    v3 = a0->field_0->field_14(v2);
    if (v3 > a2)
        return 0;
    v5 = sub_46d04a(v3, v4);
    if (!v5)
        return v5;
    v7 = a0->field_0->field_10(v6);
    if (!(char)a0->field_0->field_18(v7, 0))
    {
        return 0;
    }
    else if (!a0->field_0->field_8(v5, v3))
    {
        sub_46c941(v5);
        return 0;
    }
    else
    {
        a0->field_0->field_18(v7, 0);
        return v5;
    }
}
