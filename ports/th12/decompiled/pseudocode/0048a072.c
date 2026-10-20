// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a072; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48a072_0 {
    unsigned short field_0;
    unsigned int field_2;
    unsigned int field_6;
    unsigned short field_a;
} st_48a072_0;

typedef struct st_48a072_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned short field_8;
} st_48a072_1;


unsigned int sub_48a072(st_48a072_0 *iter, st_48a072_1 *idx)
{
    unsigned int v5;  // edx
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    unsigned int v8;  // edi
    unsigned int v9;  // edi
    unsigned int v10;  // ecx
    unsigned int v11;  // ebx
    unsigned int v12;  // ecx
    unsigned int *v13;  // ecx
    unsigned int v14;  // ebx
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]

    v5 = *((short *)((char *)&iter->field_6 + 2));
    v3 = 0;
    v6 = *((int *)((char *)&iter->field_2 + 2));
    v7 = *((int *)&(&iter->field_0)[1]);
    v0 = v8;
    v9 = v5 & 0x7fff;
    v1 = v6;
    v2 = iter->field_0 * 0x10000;
    if (v2 & 0x80000000 && v2 & 0x7fffffff)
    {
        v10 = v7 + 1;
        v11 = 0;
        if (v10 < v7 || v10 < 1)
            v11 = 1;
        iter = 0;
        v7 = v10;
        v12 = v11;
        do
        {
            if (!v12)
                goto LABEL_48a10c;
            v4 = 0;
            v13 = &(&v1)[iter];
            v14 = *(v13) + 1;
            if (v14 < *(v13) || v14 < 1)
                v4 = 1;
            iter -= 1;
            *(v13) = v14;
            v12 = v4;
        } while (iter - 1 >= 0);
        if (v12)
        {
            v6 = 0x80000000;
            v9 += 1;
        }
        else
        {
LABEL_48a10c:
            v6 = v1;
        }
    }
    if ((unsigned short)v9 == 0x7fff)
        v3 = 1;
    idx->field_0 = v7;
    idx->field_4 = v6;
    idx->field_8 = v5 & 0x8000 | v9;
    return v3;
}
