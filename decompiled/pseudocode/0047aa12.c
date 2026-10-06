// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47aa12; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_47aa12_1 {
    char padding_0[112];
    unsigned int field_70;
} st_47aa12_1;

int sub_47aa12(void)
{
    unsigned int v10;  // edx
    int v11;  // ebx
    int v13;  // ecx
    unsigned int v0;  // [bp-0x28]
    st_46d3b4_0 v1;  // [bp-0x1c]
    st_47aa12_1 *idx;  // [bp-0x14]
    char v3;  // [bp-0x10]
    char v4;  // [bp-0xc]
    char v5;  // [bp-0x8]
    char v6;  // [bp-0x7]
    char v7;  // [bp-0x6]
    int v8;  // [bp+0x4]
    unsigned int *v9;  // [bp+0x8]

    sub_46d3b4(&v1, v10, v9);
    v11 = v8;
    if (v11 >= 0x100)
    {
        if (*((int *)(v1 + 172)) > 1 && !(v8 = v11, v8 >>= 8, !sub_47c5a3(v8 & 0xff, &v1)))
        {
            v0 = 2;
            v5 = v8;
            v6 = v11;
            v7 = 0;
            v13 = 2;
        }
        else
        {
            *((unsigned int *)sub_46e96f()) = 42;
            v5 = v11;
            v6 = 0;
            v13 = 1;
        }
        if (!sub_48364d(&v1, *((int *)(v1 + 20)), 0x100, &v5, v13, &v4, 3, *((int *)(v1 + 4)), 1))
        {
LABEL_47aa73:
            if (!v3)
                return;
            idx->field_70 = idx->field_70 & 0xfffffffd;
            return;
        }
    }
    else if (!(*((int *)(v1 + 172)) <= 1 ? *((short *)(*((int *)(v1 + 200)) + v11 * 2)) & 1 : sub_4758eb(v11, 1, &v1)))
    {
        goto LABEL_47aa73;
    }
    if (v3)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return;
}
