// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e730; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e730_0 {
    char padding_0[4];
    int field_4;
} st_40e730_0;

void sub_40e730(st_40e730_0 *idx, unsigned int a1, int a2)
{
    idx->field_4 = idx->field_4 + ((unsigned int)((int)(1717986919 * a2 >> 0x22)) >> 31) + (int)(1717986919 * a2 >> 0x22);
    if (idx->field_4 >= 1000000000)
        idx->field_4 = 0x3b9ac9ff;
    return;
}
