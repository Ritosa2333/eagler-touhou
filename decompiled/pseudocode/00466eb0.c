// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466eb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466eb0_0 {
    char padding_0[4100];
    unsigned int field_1004;
} st_466eb0_0;

st_466eb0_0 * sub_466eb0(st_466eb0_0 *a0, unsigned int a1, unsigned int a2)
{
    return &a0->padding_0[a2 + a0->field_1004];
}
