// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410595; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_410595_0 {
    char padding_0[200];
    int field_c8;
} st_410595_0;

typedef struct st_410595_1 {
    char padding_0[1144];
    struct st_410595_0 *field_478;
} st_410595_1;

unsigned int sub_410595(st_410595_1 *a0)
{
    int v1;  // esi

    sub_459a60(a0, v1);
    sub_45d5d0(a0->field_478->field_c8);
    return 0;
}
