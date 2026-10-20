// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d3b5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48d3b5_0 {
    unsigned int field_0;
    char padding_4[4];
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[8];
    unsigned int field_18;
} st_48d3b5_0;

void sub_48d3b5(unsigned int a0, st_48d3b5_0 *idx)
{
    unsigned int v1;  // esi
    unsigned int v0;  // [bp-0x8]

    if (!a0)
        return;
    v0 = v1;
    if (!(idx->field_c & 0x1000))
        return;
    sub_48d12a(idx);
    idx->field_c = idx->field_c & 0xffffeeff;
    idx->field_18 = 0;
    idx->field_0 = 0;
    idx->field_8 = 0;
    return;
}
