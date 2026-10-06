// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ddbf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_476077_2 {
    unsigned short field_0;
    unsigned int field_2;
    unsigned int field_6;
    unsigned short field_a;
} st_476077_2;

typedef struct st_48ddbf_0 {
    char padding_0[112];
    unsigned int field_70;
} st_48ddbf_0;

int sub_48ddbf(void)
{
    unsigned int v11;  // edi
    unsigned int v12;  // edx
    unsigned int v13;  // eax
    unsigned int v0;  // [bp-0x3c]
    unsigned int v1;  // [bp-0x38]
    char *v2;  // [bp-0x2c]
    st_46d3b4_0 v3;  // [bp-0x28]
    st_48ddbf_0 *idx;  // [bp-0x20]
    char v5;  // [bp-0x1c]
    int v6;  // [bp-0x18]
    st_476077_2 v7;  // [bp-0x14]
    int v8;  // [bp+0x4]
    char *v9;  // [bp+0x8]
    unsigned int *v10;  // [bp+0xc]

    v1 = v11;
    sub_46d3b4(&v3, v12, v10);
    v6 = sub_476077(&v7, &v2, v9, 0, 0, 0, 0, &v3);
    v13 = sub_489b2e(&v7, v8);
    if (!((char)v6 & 3))
    {
        if (v13 == 1)
            goto LABEL_48de18;
        if (v13 != 2)
            goto LABEL_48de4a;
LABEL_48de2e:
        if (v5)
            idx->field_70 = idx->field_70 & 0xfffffffd;
        v0 = 4;
    }
    else
    {
        if ((char)v6 & 1)
            goto LABEL_48de2e;
        if (!((char)v6 & 2))
        {
LABEL_48de4a:
            if (!v5)
                return;
            idx->field_70 = idx->field_70 & 0xfffffffd;
            return;
        }
LABEL_48de18:
        if (v5)
            idx->field_70 = idx->field_70 & 0xfffffffd;
        v0 = 3;
    }
    return;
}
