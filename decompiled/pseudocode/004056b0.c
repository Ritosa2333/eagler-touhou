// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4056b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4056b0_0 {
    char padding_0[10096];
    unsigned int field_2770;
    char padding_2774[3716];
    int field_35f8;
    int field_35fc;
} st_4056b0_0;

int sub_4056b0(void)
{
    st_4056b0_0 *v1;  // esi

    v1->field_2770 = 0;
    sub_461a70(v1->field_35f8);
    return sub_461a70(v1->field_35fc);
}
