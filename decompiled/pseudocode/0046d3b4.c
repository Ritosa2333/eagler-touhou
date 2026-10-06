// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d3b4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

extern char g_4ad8d8;
extern unsigned int g_4ad9d4;
extern char g_4adab8;

st_46d3b4_0 * sub_46d3b4(st_46d3b4_0 *idx, unsigned int a1, unsigned int *a2)
{
    void* idx1;  // eax
    void* index;  // eax
    int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    idx->field_c = 0;
    if (a2)
    {
        idx->field_0 = *(a2);
        idx->field_4 = a2[1];
        return idx;
    }
    idx1 = sub_475467(v0, &v1);
    idx->field_8 = idx1;
    idx->field_0 = (int)idx1[108];
    idx->field_4 = (int)idx1[104];
    if (idx->field_0 != *((int *)&g_4adab8) && !(g_4ad9d4 & (int)idx1[112]))
        idx->field_0 = sub_474181();
    if (idx->field_4 != *((int *)&g_4ad8d8) && !(g_4ad9d4 & (int)idx->field_8[112]))
        idx->field_4 = sub_4739a5();
    index = idx->field_8;
    if ((char)index[112] & 2)
        return idx;
    *((unsigned int *)&index[112]) = (int)index[112] | 2;
    idx->field_c = 1;
    return idx;
}
