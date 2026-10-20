// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e02b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46e02b_0 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
} st_46e02b_0;


st_46e02b_0 * sub_46e02b(st_46e02b_0 *idx, unsigned int a1, st_46e02b_0 *index)
{
    unsigned int v0;  // eax
    unsigned int v1;  // eax
    int v3;  // esi
    int v4;  // eax

    if (idx == index)
        return idx;
    v0 = index->field_8;
    idx->field_8 = v0;
    v1 = index->field_4;
    if (!v0)
    {
        idx->field_4 = v1;
        return idx;
    }
    else if (v1)
    {
        v3 = sub_475860(v1) + 1;
        v4 = sub_46d04a(v3);
        idx->field_4 = v4;
        if (!v4)
            return idx;
        sub_47a2b9(v4, v3, index->field_4);
        return idx;
    }
    else
    {
        idx->field_4 = 0;
        return idx;
    }
}
