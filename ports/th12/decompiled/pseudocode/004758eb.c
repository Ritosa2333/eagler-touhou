// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4758eb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4758eb_0 {
    char padding_0[4];
    int field_4;
    char padding_8[12];
    int field_14;
    char padding_18[176];
    unsigned int field_c8;
} st_4758eb_0;

typedef struct st_4758eb_1 {
    char padding_0[112];
    unsigned int field_70;
} st_4758eb_1;


unsigned int sub_4758eb(int a0, unsigned int a1, int a2)
{
    int v8;  // ebx
    unsigned int v9;  // eax
    int v0;  // [bp-0x28]
    st_4758eb_0 *v1;  // [bp-0x1c]
    st_4758eb_1 *idx;  // [bp-0x14]
    char v3;  // [bp-0x10]
    char v4;  // [bp-0xc]
    char v5;  // [bp-0xb]
    char v6;  // [bp-0xa]
    char v7;  // [bp-0x8], Other Possible Types: unsigned short

    sub_46d3b4(a2, v8);
    if (a0 + 1 <= 0x100)
    {
        v9 = *((short *)(v1->field_c8 + a0 * 2));
    }
    else
    {
        a0 >>= 8;
        if (sub_47c5a3(a0 & 0xff, &v1))
        {
            v0 = 2;
            v4 = a0;
            v5 = a0;
            v6 = 0;
        }
        else
        {
            v4 = a0;
            v5 = 0;
            v0 = 1;
        }
        if (!sub_48384c(&v1, 1, &v4, v0, &v7, v1->field_4, v1->field_14, 1))
        {
            if (!v3)
                return 0;
            idx->field_70 = idx->field_70 & 0xfffffffd;
            return 0;
        }
        v9 = v7;
    }
    if (v3)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v9 & a1;
}
