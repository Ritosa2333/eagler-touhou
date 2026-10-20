// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x436b60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_436b60_0 {
    char padding_0[2580];
    unsigned int field_a14;
    unsigned int field_a18;
    char padding_a1c[47608];
    unsigned int field_c414;
} st_436b60_0;

unsigned int sub_436b60(void)
{
    st_436b60_0 *idx;  // esi
    unsigned int v2;  // eax

    v2 = sub_454d10(&idx->padding_0[20], 0);
    idx->field_c414 = idx->field_c414 & 0xfffffffd;
    idx->field_a14 = 0;
    idx->field_a18 = 0;
    return v2;
}
