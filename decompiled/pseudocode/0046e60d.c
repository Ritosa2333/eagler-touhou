// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e60d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46e60d_0 {
    char padding_0[20];
    unsigned int field_14;
} st_46e60d_0;


unsigned int sub_46e60d(void)
{
    st_46e60d_0 *idx;  // eax
    unsigned int v2;  // ecx

    idx = sub_475467();
    v2 = idx->field_14 * 0x343fd + 2531011;
    idx->field_14 = v2;
    return v2 >> 16 & 0x7fff;
}
