// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466280; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466280_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[8];
    unsigned int field_10;
} st_466280_0;

unsigned int sub_466280(unsigned int a0)
{
    st_466280_0 *v3;  // edi
    unsigned int v4;  // esi
    unsigned int v5;  // esi
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = a0;
    if (!v3->field_4)
        return 0;
    v0 = v4;
    v5 = 0;
    if (0 < v3->field_10)
    {
        do
        {
        } while ((!*((int *)(v3->field_4 + v5 * 4)) || (v1 = 0, (*((int *)(*((int *)*((int *)(v3->field_4 + v5 * 4))) + 36)))(*((int *)(v3->field_4 + v5 * 4)), &v1), (char)v1 & 1)) && (v5 += 1, v5 < v3->field_10));
        if (v5 != v3->field_10)
            return *((int *)(v3->field_4 + v5 * 4));
    }
    else if (v3->field_10)
    {
        return *((int *)v3->field_4);
    }
    v6 = sub_46e60d();
    return *((int *)(v3->field_4 + v6 % v3->field_10 * 4));
}
