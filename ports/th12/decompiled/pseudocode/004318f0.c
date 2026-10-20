// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4318f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4318f0_2 {
    char padding_0[12];
    struct st_4318f0_3 *field_c;
} st_4318f0_2;

typedef struct st_4318f0_3 {
    struct st_4318f0_0 *field_0;
} st_4318f0_3;

typedef struct st_4318f0_1 {
    unsigned int field_0;
} st_4318f0_1;

typedef struct st_4318f0_0 {
    char padding_0[8];
    struct st_4318f0_1 *field_8;
} st_4318f0_0;

void sub_4318f0(void)
{
    st_4318f0_2 *idx;  // esi
    st_4318f0_0 **v2;  // eax

    v2 = idx->field_c;
    if (v2)
    {
        *(v2)->field_8(v2);
        idx->field_c = NULL;
    }
    return;
}
