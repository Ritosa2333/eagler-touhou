// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402760; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_402760_0 {
    char padding_0[32];
    unsigned int field_20;
} st_402760_0;

int sub_402760(void)
{
    st_402760_0 *idx;  // eax

    idx->field_20 = idx->field_20 & 0xfffffffe;
    return;
}
