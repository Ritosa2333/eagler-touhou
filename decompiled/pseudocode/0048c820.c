// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c820; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48c820_0 {
    char padding_0[4];
    unsigned int field_4;
} st_48c820_0;

typedef struct st_48c820_1 {
    char padding_0[112];
    unsigned int field_70;
} st_48c820_1;


unsigned int sub_48c820(char a0, int a1)
{
    unsigned int v3;  // eax
    st_48c820_0 *v0;  // [bp-0x10]
    st_48c820_1 *idx;  // [bp-0xc]
    char v2;  // [bp-0x8]

    sub_46d3b4(a1);
    if (v0 && v0->field_4 == 932)
    {
        v3 = sub_48c5cb(a1, a0, 0, 3);
        if (!v2)
            return v3;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return v3;
    }
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return 0;
}
