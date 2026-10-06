// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4691b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4691b0_0 {
    char padding_0[4112];
    unsigned int field_1010;
} st_4691b0_0;

typedef struct st_4691b0_1 {
    char padding_0[4144];
    struct st_4691b0_0 *field_1030;
    struct st_4691b0_1 *field_1034;
} st_4691b0_1;


int sub_4691b0(void)
{
    st_4691b0_1 *v1;  // eax
    st_4691b0_1 *v2;  // eax
    st_4691b0_1 *v3;  // eax
    unsigned int v4;  // ecx

    v2 = &v1->field_1030;
    if (v1 != 0xffffefd0)
    {
        do
        {
            v3 = v2;
            if (*((int *)(*((int *)&v3->padding_0[0]) + 0x1010)) == v4)
                return;
        } while ((v2 = (st_4691b0_1 *)*((int *)&v3->padding_0[4]), *((int *)&v3->padding_0[4])));
    }
    return;
}
