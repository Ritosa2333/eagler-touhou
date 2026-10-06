// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47dfd9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dfd9_5 {
    struct st_47dfd9_0 *field_0;
} st_47dfd9_5;

typedef struct st_47dfd9_1 {
    unsigned int field_0;
} st_47dfd9_1;

typedef struct st_47dfd9_0 {
    char padding_0[8];
    struct st_47dfd9_1 *field_8;
} st_47dfd9_0;

typedef struct st_47dfd9_4 {
    char padding_0[4];
    struct st_47dfd9_5 *field_4;
    struct st_47dfd9_5 *field_8;
} st_47dfd9_4;

unsigned int sub_47dfd9(st_47dfd9_4 *a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v1;  // esi
    unsigned int v2;  // eax
    char v0;  // [bp+0x0]

    v2 = a0->field_4->field_0->field_8(a2, a3, v1, &v0);
    if (v2 >= a3)
        return v2;
    return a0->field_8->field_0->field_8(v2, a3);
}
