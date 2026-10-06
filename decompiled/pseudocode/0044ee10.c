// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ee10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44ee10_0 {
    char padding_0[4];
    struct st_44ee10_1 *field_4;
    char padding_8[12];
    int field_14;
    unsigned int field_18;
} st_44ee10_0;

typedef struct st_44ee10_1 {
    char field_0;
} st_44ee10_1;

unsigned int sub_44ee10(st_44ee10_0 *idx, unsigned int a1, int a2, char a3)
{
    int v2;  // ebx
    int v3;  // esi
    int v4;  // ebx
    unsigned int v5;  // eax
    unsigned int v6;  // edi
    int v7;  // edi
    st_44ee10_0 *v8;  // eax
    unsigned int v9;  // ebp
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    v2 = a2;
    if (v2 > -0x2)
        sub_491687(v3, v4);
    v5 = idx->field_18;
    if (v5 < v2)
    {
        sub_44ef10(v2, idx->field_14);
        return -(-(0 < v2));
    }
    if (a3 && v2 < 16)
    {
        v1 = v6;
        v7 = idx->field_14;
        if (v2 < idx->field_14)
            v7 = v2;
        if (v5 >= 16)
        {
            v8 = &idx->field_4;
            v0 = v9;
            if (v7)
                sub_46e210(v8, 16, v8->padding_0, v7);
            sub_46ca4f(v8->padding_0);
        }
        idx->field_14 = v7;
        idx->field_18 = 15;
        *(&idx->padding_0[v7 + 4]) = 0;
        return -(-(0 < v2));
    }
    if (v2)
        return -(-(0 < v2));
    idx->field_14 = 0;
    if (v5 >= 16)
    {
        idx->field_4->field_0 = 0;
        return 0;
    }
    *((char *)&idx->field_4) = (char)NULL;
    return -(-(0 < v2));
}
