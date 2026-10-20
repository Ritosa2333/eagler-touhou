// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47dd86; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dd86_0 {
    char padding_0[4];
    unsigned int field_4;
} st_47dd86_0;


void sub_47dd86(st_47dd86_0 *idx)
{
    idx->field_4 = idx->field_4 | 0x8000;
    return;
}
