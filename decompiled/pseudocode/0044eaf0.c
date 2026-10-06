// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44eaf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44eaf0_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[12];
    int field_14;
    unsigned int field_18;
} st_44eaf0_0;

st_44eaf0_0 * sub_44eaf0(st_44eaf0_0 *idx, unsigned int a1, st_44eaf0_0 *a2, int a3, int a4)
{
    int v0;  // edi
    int v1;  // esi
    int v2;  // ebp
    int v3;  // ebx
    int v4;  // edi
    int v5;  // eax
    st_44eaf0_0 *v6;  // ebx
    st_44eaf0_0 *v7;  // eax
    int v8;  // 4101

    if (a2->field_14 < a3)
        sub_4916e6(v0, v1, v2, v3);
    v4 = a2->field_14 - a3;
    if (a4 < v4)
        v4 = a4;
    if (idx == a2)
    {
        sub_44ed60(v4 + a3, -0x1);
        sub_44ed60(0, a3);
        return idx;
    }
    if (v4 > -0x2)
        sub_491687();
    v5 = idx->field_18;
    if (v5 < v4)
    {
        sub_44ef10(v4, idx->field_14);
        if (v4 <= 0)
            return idx;
    }
    else if (!v4)
    {
        idx->field_14 = 0;
        if (v5 >= 16)
            *((char *)idx->field_4) = 0;
        else
            *((char *)&idx->field_4) = 0;
        return idx;
    }
    else if (v4 <= 0)
    {
        return idx;
    }
    v6 = &idx->field_4;
    if (idx->field_18 >= 16)
        v7 = (st_44eaf0_0 *)v6->padding_0;
    else
        v7 = v6;
    sub_46e210(v7, idx->field_18, &(a2->field_18 < 16 ? (struct st_44eaf0_0 *)&a2->field_4 : a2->field_4)->padding_0[a3], v4);
    v8 = idx->field_18;
    idx->field_14 = v4;
    if (v8 >= 16)
        v6 = (st_44eaf0_0 *)v6->padding_0;
    v6->padding_0[v4] = 0;
    return idx;
}
