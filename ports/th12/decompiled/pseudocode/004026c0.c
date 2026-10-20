// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4026c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4026c0_0 {
    char padding_0[1144];
    int field_478;
} st_4026c0_0;

int sub_4026c0(void)
{
    st_4026c0_0 *idx;  // esi
    int v2;  // eax

    v2 = idx->field_478;
    if (v2)
        v2 = sub_46c941(v2);
    idx->field_478 = 0;
    return v2;
}
