// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ba20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_48ba20_1 {
    char padding_0[112];
    unsigned int field_70;
} st_48ba20_1;

int sub_48ba20(void)
{
    unsigned int v5;  // edx
    st_46d3b4_0 v0;  // [bp-0x14]
    st_48ba20_1 *idx;  // [bp-0xc]
    char v2;  // [bp-0x8]
    int v3;  // [bp+0x4]
    unsigned int *v4;  // [bp+0x8]

    sub_46d3b4(&v0, v5, v4);
    if (*((int *)(v0 + 172)) > 1)
        sub_4758eb(v3, 4, &v0);
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return;
}
