// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462020_1 {
    char padding_0[16];
    struct st_462020_0 *field_10;
    struct st_462020_1 *field_14;
} st_462020_1;

typedef struct st_462020_0 {
    char padding_0[1002];
    short field_3ea;
} st_462020_0;

st_462020_1 * sub_462020(unsigned int a0, unsigned int a1)
{
    st_462020_1 *v1;  // esi
    st_462020_1 *v2;  // eax
    st_462020_1 *v3;  // eax
    st_462020_1 *v4;  // ecx

    v2 = &v1->field_10;
    if (!v2)
        return NULL;
    while (1)
    {
        v3 = v2;
        v4 = *((int *)&v3->padding_0[0]);
        if (*((short *)((char *)&v4[41].field_10 + 2)) == a1)
            return v4;
        if (a1 == 0xffffffff && v4 != v1)
            return v4;
        v2 = *((int *)&v3->padding_0[4]);
        if (!*((int *)&v3->padding_0[4]))
            return NULL;
    }
}
