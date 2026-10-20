// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48dc40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48dc40_0 {
    char padding_0[112];
    unsigned int field_70;
} st_48dc40_0;


int sub_48dc40(int a0, int a1, int a2)
{
    int v7;  // edi
    int v8;  // esi
    int v9;  // ebx
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0x40]
    char v1;  // [bp-0x2c]
    char v2;  // [bp-0x28]
    st_48dc40_0 *idx;  // [bp-0x20]
    char v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]
    char v6;  // [bp-0x14]

    sub_46d3b4(a2, v7, v8, v9);
    v5 = sub_476077(&v6, &v1, a1, 0, 0, 0, 0, &v2);
    v10 = sub_4895ea(&v6, a0);
    if (!((char)v5 & 3))
    {
        if (v10 == 1)
            goto LABEL_48dc99;
        if (v10 != 2)
            goto LABEL_48dccb;
LABEL_48dcaf:
        if (v4)
            idx->field_70 = idx->field_70 & 0xfffffffd;
        v0 = 4;
    }
    else
    {
        if ((char)v5 & 1)
            goto LABEL_48dcaf;
        if (!((char)v5 & 2))
        {
LABEL_48dccb:
            if (!v4)
                return v12;
            idx->field_70 = idx->field_70 & 0xfffffffd;
            return v12;
        }
LABEL_48dc99:
        if (v4)
            idx->field_70 = idx->field_70 & 0xfffffffd;
        v0 = 3;
    }
    return v11;
}
