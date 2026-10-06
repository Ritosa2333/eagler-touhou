// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402750; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_402750_0 {
    char padding_0[64];
    unsigned int field_40;
} st_402750_0;

int sub_402750(void)
{
    st_402750_0 *idx;  // eax

    idx->field_40 = idx->field_40 & 0xfffffffe;
    return;
}
