// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4692b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4692b0_0 {
    char padding_0[4];
    unsigned int field_4;
} st_4692b0_0;

typedef struct st_4692b0_1 {
    char padding_0[4];
    struct st_4692b0_0 *field_4;
} st_4692b0_1;

int sub_4692b0(void)
{
    st_4692b0_1 *v1;  // eax
    unsigned int v2;  // edx

    v1->field_4->field_4 = v2;
    return;
}
