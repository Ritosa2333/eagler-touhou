// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e3b8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e3b8_1 {
    char field_0;
} st_47e3b8_1;

typedef struct st_47e3b8_0 {
    char padding_0[4];
    struct st_47e3b8_1 *field_4;
    int field_8;
} st_47e3b8_0;

extern char g_49ddec;

st_47e3b8_0 * sub_47e3b8(st_47e3b8_0 *idx, unsigned int a1, unsigned int a2, int a3)
{
    unsigned int v2;  // esi
    unsigned int v3;  // edi
    int v4;  // edi
    char *v5;  // eax
    char *iter;  // eax
    int i;  // edi
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    v0 = v3;
    v4 = a3;
    *((char **)&idx->padding_0[0]) = &g_49ddec;
    if (v4 && a2)
    {
        v5 = sub_47dc29(v4, 0);
        idx->field_4 = v5;
        idx->field_8 = v4;
        if (!v5)
        {
            return idx;
        }
        else if (v4)
        {
            iter = v5;
            do
            {
                i = v4;
                *(iter) = *((char *)(a2 - v5 + iter));
                iter += 1;
                v4 = i - 1;
            } while (i != 1);
            return idx;
        }
        else
        {
            return idx;
        }
    }
    idx->field_4 = NULL;
    idx->field_8 = 0;
    return idx;
}
