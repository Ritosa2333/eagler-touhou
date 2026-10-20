// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462990; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462990_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[24];
    unsigned int field_20;
} st_462990_0;

int sub_462990(void)
{
    int v2;  // eax
    int v3;  // esi
    st_462990_0 *idx;  // esi
    unsigned int v0;  // [bp+0x0]

    idx = sub_4627e0(v2, v3);
    idx->field_4 = idx->field_4 & 0xfffffffd;
    idx->field_20 = v0;
    sub_462420();
    return;
}
