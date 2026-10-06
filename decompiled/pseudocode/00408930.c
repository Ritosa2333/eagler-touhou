// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408930; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408930_0 {
    char padding_0[48];
    unsigned int field_30;
} st_408930_0;

int sub_408930(void)
{
    st_408930_0 *idx;  // eax

    idx->field_30 = idx->field_30 & 0xfffffffc;
    return;
}
