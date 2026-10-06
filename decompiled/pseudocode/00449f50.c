// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x449f50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_449f50_0 {
    char padding_0[44];
    unsigned int field_2c;
} st_449f50_0;

int sub_449f50(st_449f50_0 *a0)
{
    memset(&a0[1].padding_0[4], 0, 8);
    a0->field_2c = 0;
    return sub_453f10();
}
