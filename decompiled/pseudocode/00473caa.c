// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473caa; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_473caa_0 {
    char padding_0[112];
    unsigned int field_70;
} st_473caa_0;

unsigned int sub_473caa(unsigned int a0, unsigned int a1)
{
    unsigned int v5;  // eax
    st_46d3b4_0 v0;  // [bp-0x14]
    unsigned int *v1;  // [bp-0x10]
    st_473caa_0 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]

    sub_46d3b4(&v0, a1, NULL);
    if (v1[2])
    {
        v5 = v1[1];
        if (!v3)
            return v5;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return v5;
    }
    else
    {
        if (!v3)
            return 0;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return 0;
    }
}
