// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ddc1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47ddc1_0 {
    char padding_0[4];
    char field_4;
} st_47ddc1_0;

void sub_47ddc1(st_47ddc1_0 *a0, unsigned int a1, st_47ddc1_0 *a2)
{
    if (a0->field_4 != 3 && a2->field_4 > 1)
        a0->field_4 = a2->field_4;
    return;
}
