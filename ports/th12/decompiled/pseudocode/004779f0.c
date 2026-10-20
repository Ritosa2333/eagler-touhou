// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4779f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4779f0_0 {
    char padding_0[6];
    unsigned short field_6;
    char padding_8[12];
    unsigned short field_14;
    char padding_16[38];
    unsigned int field_3c;
} st_4779f0_0;


unsigned int * sub_4779f0(st_4779f0_0 *a0, unsigned int a1)
{
    st_4779f0_0 *v1;  // ecx
    unsigned int v2;  // esi
    unsigned int v3;  // edx
    unsigned int v4;  // edi
    unsigned int *v5;  // eax
    unsigned int v0;  // [bp-0x10]

    v1 = &a0->padding_0[a0->field_3c];
    v2 = v1->field_6;
    v3 = 0;
    v0 = v4;
    v5 = &v1->padding_0[v1->field_14 + 24];
    if (v2)
    {
        do
        {
            if (a1 >= v5[3] && a1 < v5[2] + v5[3])
                return v5;
        } while ((v3 += 1, v5 += 40, v3 < v2));
    }
    return NULL;
}
