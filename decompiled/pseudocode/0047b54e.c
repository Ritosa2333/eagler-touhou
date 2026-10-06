// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b54e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47b478_0 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
} st_47b478_0;

typedef struct st_47b54e_0 {
    char padding_0[24];
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[8];
    struct st_47b478_0 *field_28;
} st_47b54e_0;

void sub_47b54e(st_47b54e_0 *a0)
{
    sub_47b478(a0->field_28, a0->field_18, a0->field_1c);
    return;
}
