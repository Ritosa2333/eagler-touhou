// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e17b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47de46_0 {
    char padding_0[4];
    char field_4;
} st_47de46_0;

typedef struct st_47de46_2 {
    char padding_0[4];
    struct st_47de46_0 *field_4;
} st_47de46_2;

void* sub_47e17b(void* idx, unsigned int a1, st_47de46_0 *a2)
{
    unsigned int v1;  // esi
    st_47de46_2 *v2;  // eax
    unsigned int v3;  // edx
    st_47de46_2 *v5;  // eax
    unsigned int v0;  // [bp-0x8]

    v0 = v1;
    if (a2)
    {
        v2 = sub_47dc29(8, 0);
        v5 = (!v2 ? NULL : sub_47de46(v2, v3, a2));
        *((st_47de46_2 **)idx) = v5;
        *((char *)&idx[4]) = (char)((v5) + 1) & 3;
    }
    else
    {
        *((st_47de46_2 **)idx) = NULL;
        *((char *)&idx[4]) = 0;
    }
    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    return idx;
}
