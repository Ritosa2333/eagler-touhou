// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4662f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4662f0_0 {
    char padding_0[4];
    struct st_4662f0_1 *field_4;
    char padding_8[8];
    unsigned int field_10;
} st_4662f0_0;

typedef struct st_4662f0_1 {
    unsigned int field_0;
} st_4662f0_1;

unsigned int sub_4662f0(st_4662f0_0 *a0)
{
    if (!a0->field_4)
    {
        return 0;
    }
    else if (a0->field_10 <= NULL)
    {
        return 0;
    }
    else
    {
        return a0->field_4->field_0;
    }
}
