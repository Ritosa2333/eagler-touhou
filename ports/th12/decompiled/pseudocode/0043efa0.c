// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43efa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43efa0_0 {
    char padding_0[20];
    int field_14;
} st_43efa0_0;

void sub_43efa0(void)
{
    st_43efa0_0 *v1;  // edi
    int v2;  // esi
    int v3;  // ecx

    sub_4615a0(v1->field_14, &v3, v2, 0, v3);
    *((int *)&v1[29].padding_0[16 + 4 * v2]) = v1->field_14;
    return;
}
