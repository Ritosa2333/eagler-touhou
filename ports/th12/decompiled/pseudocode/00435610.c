// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x435610; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_435610_0 {
    char padding_0[212];
    unsigned int field_d4;
} st_435610_0;

int sub_435610(void)
{
    st_435610_0 *idx;  // eax
    unsigned int v2;  // edx

    *((unsigned int *)&idx->padding_0[144 + 4 * idx->field_d4]) = v2;
    idx->field_d4 = idx->field_d4 + 1;
    return;
}
