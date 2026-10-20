// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473ff5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_473ff5_0 {
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
} st_473ff5_0;


void sub_473ff5(st_473ff5_0 *i)
{
    unsigned int v1;  // edi
    st_473ff5_0 *v2;  // ebx
    unsigned int v0;  // [bp-0x10]

    v0 = v1;
    InterlockedIncrement(i);
    if (i->field_b0)
        InterlockedIncrement(i->field_b0);
    if (i->field_b8)
        InterlockedIncrement(i->field_b8);
    if (i->field_b4)
        InterlockedIncrement(i->field_b4);
    if (i->field_c0)
        InterlockedIncrement(i->field_c0);
    v2 = &i->field_50;
    i = 6;
    do
    {
        if (*((int *)(&v2->padding_0[0] - 8)) != "C" && *((int *)&v2->padding_0[0]))
            InterlockedIncrement(*((int *)&v2->padding_0[0]));
        if (*((int *)(&v2->padding_0[0] - 4)) && *((int *)&v2->padding_0[4]))
            InterlockedIncrement(*((int *)&v2->padding_0[4]));
        v2 = &v2->padding_0[16];
        i -= 1;
    } while (i != 1);
    InterlockedIncrement(i->field_d4 + 180);
    return;
}
