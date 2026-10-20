// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x405fa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_405fa0_0 {
    char padding_0[28];
    unsigned int field_1c;
} st_405fa0_0;

int sub_405fa0(void)
{
    st_405fa0_0 *v1;  // eax
    st_405fa0_0 *iter;  // edi
    unsigned int v3;  // ecx
    unsigned int *i;  // esi

    iter = &v1->field_1c;
    for (v3 = 7; v3; i += 1)
    {
        v3 -= 1;
        *((unsigned int *)&iter->padding_0[0]) = *(i);
        iter = &iter->padding_0[4];
    }
    return;
}
