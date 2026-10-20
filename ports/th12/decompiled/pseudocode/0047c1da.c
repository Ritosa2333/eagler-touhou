// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c1da; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47c1da_0 {
    char padding_0[16];
    unsigned int field_10;
} st_47c1da_0;

unsigned int sub_47c1da(st_47c1da_0 *a0)
{
    if (a0)
        return a0->field_10;
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 0xffffffff;
}
