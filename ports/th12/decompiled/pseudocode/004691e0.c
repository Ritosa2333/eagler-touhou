// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4691e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4691e0_2 {
    char padding_0[4148];
    struct st_4691e0_1 *field_1034;
} st_4691e0_2;

typedef struct st_4691e0_0 {
    char padding_0[4];
    unsigned int field_4;
} st_4691e0_0;

typedef struct st_4691e0_1 {
    struct st_4691e0_0 *field_0;
    struct st_4691e0_1 *field_4;
} st_4691e0_1;

int sub_4691e0(void)
{
    st_4691e0_2 *v1;  // eax
    st_4691e0_1 *i;  // eax
    st_4691e0_1 *v3;  // ecx

    i = v1->field_1034;
    if (!v1->field_1034)
        return;
    do
    {
        v3 = i->field_4;
        i->field_0->field_4 = 0;
        i = v3;
    } while (i);
    return;
}
