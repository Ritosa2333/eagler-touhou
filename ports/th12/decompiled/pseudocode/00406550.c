// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406550; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406550_2 {
    char padding_0[8];
    struct st_406550_3 *field_8;
    char padding_c[1344];
    unsigned int field_54c;
    unsigned int field_550;
} st_406550_2;

typedef struct st_406550_0 {
    char padding_0[188];
    struct st_406550_1 *field_bc;
} st_406550_0;

typedef struct st_406550_3 {
    struct st_406550_0 *field_0;
} st_406550_3;

typedef struct st_406550_1 {
    unsigned int field_0;
} st_406550_1;

void sub_406550(void)
{
    st_406550_2 *idx;  // esi
    unsigned int v3;  // ebx
    int v0;  // [bp-0x4]

    idx->field_54c = &(idx->padding_0)[35 * v3 + 516];
    sub_430910(v0);
    idx->field_8->field_0->field_bc(idx->field_8, idx->field_54c + 0xcc);
    idx->field_550 = v3;
    return;
}
