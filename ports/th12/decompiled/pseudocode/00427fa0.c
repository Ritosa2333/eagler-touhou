// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427fa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427fa0_0 {
    char padding_0[1132];
    unsigned int field_46c;
} st_427fa0_0;

extern unsigned int g_4b44f4;

st_427fa0_0 * sub_427fa0(void)
{
    unsigned int v1;  // esi
    st_427fa0_0 *v2;  // edi

    sub_427ec0(v1);
    sub_477420(v2, 0, 1168);
    v2->field_46c = 0x10000;
    g_4b44f4 = v2;
    return v2;
}
