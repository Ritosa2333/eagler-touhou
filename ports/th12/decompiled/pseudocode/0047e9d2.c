// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e9d2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e802_0 {
    char padding_0[4];
    char field_4;
} st_47e802_0;

typedef struct st_47e26b_0 {
    int field_0;
    char field_4;
} st_47e26b_0;

st_47e26b_0 * sub_47e9d2(unsigned int *a0, unsigned int a1, st_47e26b_0 *a2, st_47e802_0 *a3)
{
    a2->field_0 = *(a0);
    *((unsigned int *)&a2->field_4) = a0[1];
    sub_47e802(a2, a1, a3);
    return a2;
}
