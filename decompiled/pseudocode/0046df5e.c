// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46df5e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46df5e_0 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
} st_46df5e_0;

extern char g_49cd28;

st_46df5e_0 * sub_46df5e(st_46df5e_0 *idx, unsigned int a1, int *a2)
{
    unsigned int v1;  // edi
    int v3;  // esi
    int v4;  // eax
    unsigned int v0;  // [bp-0x10]

    v0 = v1;
    *((char **)&idx->padding_0[0]) = &g_49cd28;
    if (*(a2))
    {
        v3 = sub_475860(*(a2)) + 1;
        v4 = sub_46d04a(v3);
        idx->field_4 = v4;
        if (v4)
            sub_47a2b9(v4, v3, *(a2));
    }
    else
    {
        idx->field_4 = 0;
    }
    idx->field_8 = 1;
    return idx;
}
