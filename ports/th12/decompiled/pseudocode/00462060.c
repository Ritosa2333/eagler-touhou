// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462060; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462060_1 {
    char padding_0[16];
    struct st_462060_0 *field_10;
    struct st_462060_1 *field_14;
} st_462060_1;

typedef struct st_462060_0 {
    unsigned int field_0;
    char padding_4[998];
    short field_3ea;
} st_462060_0;

unsigned int * sub_462060(void)
{
    int *v1;  // esi
    st_462060_1 *v2;  // eax
    st_462060_1 *iter;  // eax
    unsigned int v4;  // edi
    unsigned int *v5;  // ebx
    unsigned int *v6;  // ebx

    v2 = sub_461920(*(v1));
    if (!v2)
        *(v1) = 0;
    iter = &v2->field_10;
    if (v2 != 0xfffffff0)
    {
        do
        {
            if (*((short *)(*((int *)&iter->padding_0[0]) + 1002)) == v4 || v4 == 0xffffffff)
            {
                *(v5) = *((int *)*((int *)&iter->padding_0[0]));
                return v5;
            }
        } while ((iter = (st_462060_1 *)*((int *)&iter->padding_0[4]), iter));
    }
    *(v6) = 0;
    return v6;
}
