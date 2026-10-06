// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4303c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4303c0_0 {
    char padding_0[228];
    unsigned int field_e4;
} st_4303c0_0;

typedef struct st_4303c0_2 {
    struct st_4303c0_0 *field_0;
} st_4303c0_2;

typedef struct st_4303c0_1 {
    char padding_0[8];
    struct st_4303c0_2 *field_8;
} st_4303c0_1;

int sub_4303c0(st_4303c0_1 *iter)
{
    int v0;  // [bp-0x4]

    sub_45a3c0(v0);
    iter = iter->field_8;
    goto *((void *)(iter->field_8->field_0->field_e4));
}
