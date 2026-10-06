// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48df7f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48df7f_0 {
    unsigned int field_0;
    unsigned short field_4[2];
    unsigned short field_6;
} st_48df7f_0;

typedef struct st_48df7f_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned short field_8;
} st_48df7f_1;


void sub_48df7f(st_48df7f_1 *a0, st_48df7f_0 *a1)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // eax
    unsigned short v13;  // cx
    unsigned int v14;  // ecx
    unsigned int v15;  // edx
    unsigned int v16;  // ecx
    unsigned int v5;  // edi
    unsigned short v6;  // cx
    unsigned int v7;  // edx
    unsigned int v8;  // ebx
    unsigned int v9;  // eax
    unsigned short v10;  // cx
    unsigned int v11;  // edi
    st_48df7f_1 *idx;  // eax
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp+0x8]

    v1 = v3;
    v4 = a1->field_4[1];
    v0 = v5;
    v6 = (unsigned short)(v4 >> 4) & 0x7ff;
    v2 = v4 & 0x8000;
    v7 = a1->field_0;
    v8 = v6;
    v9 = (unsigned int)(a1->field_4 & 0xfffff);
    v1 = 0x80000000;
    if (v8)
    {
        if (v8 != 0x7ff)
        {
            v10 = v6 + 0x3c00;
LABEL_48dff0:
            v11 = v10;
        }
        else
        {
            v11 = 0x7fff;
        }
        v14 = v7 >> 21 | v9 * 0x800 | v1;
        idx = a0;
        idx->field_4 = v14;
        idx->field_0 = v7 * 0x800;
        if (!(v14 & 0x80000000))
        {
            do
            {
                v15 = idx->field_4 * 2 | idx->field_0 >> 31;
                v16 = idx->field_0 * 2;
                v11 += 0xffff;
                idx->field_4 = v15;
                idx->field_0 = v16;
            } while (!(v15 & 0x80000000));
        }
        v13 = (unsigned short)v2 | (unsigned short)v11;
    }
    else if (!v9 && !v7)
    {
        idx = a0;
        v13 = v2;
        idx->field_4 = 0;
        idx->field_0 = 0;
    }
    else
    {
        v10 = v6 + 15361;
        v1 = 0;
        goto LABEL_48dff0;
    }
    idx->field_8 = v13;
    return;
}
