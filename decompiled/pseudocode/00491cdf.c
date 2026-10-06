// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491cdf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_491cdf_0 {
    char padding_0[4];
    int field_4;
} st_491cdf_0;

int sub_491cdf(unsigned int *idx, int a1, int a2, unsigned int *a3, unsigned int *a4)
{
    unsigned int v2;  // ecx
    unsigned int v3;  // edi
    unsigned int v4;  // esi
    unsigned int v5;  // ebx
    st_491cdf_0 *v6;  // eax
    unsigned int v7;  // esi
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    v0 = v3;
    v4 = idx[3];
    v1 = idx[4];
    v5 = v4;
    while (1)
    {
        idx = v4;
        do
        {
            if (a1 < 0)
            {
                v7 = v4 + 1;
                *(a3) = v7;
                *(a4) = v5;
                if (v5 <= idx[3] && v7 <= v5)
                    return v7 * 20 + v1;
                sub_472b94(); /* do not return */
            }
            if (v4 == 0xffffffff)
                sub_472b94(); /* do not return */
            v4 -= 1;
            v6 = v4 * 20 + v1;
        } while ((v6->field_4 >= a2 || a2 > v6[1].padding_0) && v4 != 0xffffffff);
        a1 -= 1;
        v5 = idx;
    }
}
