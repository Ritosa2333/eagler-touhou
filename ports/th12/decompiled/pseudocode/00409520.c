// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x409520; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_409520_0 {
    char padding_0[16];
    unsigned int field_10;
} st_409520_0;

st_409520_0 * sub_409520(st_409520_0 *idx)
{
    idx->field_10 = idx->field_10 & 0xfffffffe;
    return idx;
}
