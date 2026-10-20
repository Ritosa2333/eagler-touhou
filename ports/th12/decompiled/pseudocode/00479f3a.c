// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x479f3a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_479f3a_1 {
    char padding_0[4];
    int field_4;
    char padding_8[4];
    int field_c;
    char padding_10[13];
    char field_1d;
    char padding_1e[255];
    char field_11d;
} st_479f3a_1;

typedef struct st_479f3a_0 {
    char padding_0[112];
    unsigned int field_70;
} st_479f3a_0;


unsigned int sub_479f3a(unsigned int a0, int a1)
{
    int v8;  // ebx
    unsigned int v9;  // ebx
    st_479f3a_1 *v10;  // eax
    char v0;  // [bp-0x1c]
    st_479f3a_1 *v1;  // [bp-0x18]
    st_479f3a_0 *idx;  // [bp-0x14]
    char v3;  // [bp-0x10]
    char v4;  // [bp-0xc]
    char v5;  // [bp-0xb]
    char v6;  // [bp-0x8]
    char v7;  // [bp-0x7]

    sub_46d3b4(a1, v8);
    v9 = a0;
    if (v9 > 0xff)
    {
        v6 = v9 >> 8;
        v7 = v9;
        if (!((&v1->field_1d)[v6] & 4) || !sub_48364d(&v0, v1->field_c, 0x200, &v6, 2, &v4, 2, v1->field_4, 1))
        {
            if (!v3)
                return v9;
            idx->field_70 = idx->field_70 & 0xfffffffd;
            return v9;
        }
        v9 = v4 * 0x100 + v5;
    }
    else
    {
        v10 = &v1->padding_0[v9];
        if (v10->field_1d & 32)
            v9 = v10->field_11d;
    }
    if (v3)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v9;
}
