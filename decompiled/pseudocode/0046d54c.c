// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d54c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_46d54c_0 {
    char padding_0[112];
    unsigned int field_70;
} st_46d54c_0;

int sub_46d54c(void)
{
    unsigned int v7;  // edx
    st_46d3b4_0 v0;  // [bp-0x14]
    st_46d54c_0 *idx;  // [bp-0xc]
    char v2;  // [bp-0x8]
    int v3;  // [bp+0x4]
    int v4;  // [bp+0x8]
    int v5;  // [bp+0xc]
    int v6;  // [bp+0x10]

    sub_46d3b4(&v0, v7, NULL);
    sub_47676f(v3, v4, v5, v6, &v0);
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return;
}
