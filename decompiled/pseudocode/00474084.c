// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474084; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_474084_0 {
    char padding_0[72];
    unsigned int field_48;
    unsigned int field_4c;
    int field_50;
    int field_54;
    char padding_58[88];
    int field_b0;
    int field_b4;
    int field_b8;
    char padding_bc[4];
    int field_c0;
    char padding_c4[16];
    unsigned int field_d4;
} st_474084_0;


st_474084_0 * sub_474084(st_474084_0 *i)
{
    unsigned int v2;  // ebx
    unsigned int v3;  // esi
    st_474084_0 *v4;  // ebx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    if (!i)
        return i;
    v1 = v2;
    v0 = v3;
    InterlockedDecrement(i);
    if (i->field_b0)
        InterlockedDecrement(i->field_b0);
    if (i->field_b8)
        InterlockedDecrement(i->field_b8);
    if (i->field_b4)
        InterlockedDecrement(i->field_b4);
    if (i->field_c0)
        InterlockedDecrement(i->field_c0);
    v4 = &i->field_50;
    i = 6;
    do
    {
        if (*((int *)(&v4->padding_0[0] - 8)) != "C" && *((int *)&v4->padding_0[0]))
            InterlockedDecrement(*((int *)&v4->padding_0[0]));
        if (*((int *)(&v4->padding_0[0] - 4)) && *((int *)&v4->padding_0[4]))
            InterlockedDecrement(*((int *)&v4->padding_0[4]));
        v4 = &v4->padding_0[16];
        i -= 1;
    } while (i != 1);
    InterlockedDecrement(i->field_d4 + 180);
    return i;
}
