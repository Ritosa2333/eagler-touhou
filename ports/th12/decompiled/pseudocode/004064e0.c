// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4064e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4064e0_0 {
    char padding_0[4];
    int field_4;
} st_4064e0_0;

int sub_4064e0(st_4064e0_0 *a0, unsigned int a1, int a2)
{
    return a0->field_4 <= a2;
}
