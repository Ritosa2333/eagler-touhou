// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492121; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492121_0 {
    char padding_0[144];
    unsigned int field_90;
} st_492121_0;

void sub_492121(void)
{
    st_492121_0 *idx;  // eax

    if (*((int *)(sub_475467() + 144)) > 0)
    {
        idx = sub_475467();
        idx->field_90 = idx->field_90 - 1;
    }
    return;
}
