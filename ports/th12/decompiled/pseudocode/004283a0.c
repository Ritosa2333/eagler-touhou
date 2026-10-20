// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4283a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4283a0_0 {
    char padding_0[12];
    struct st_4283a0_1 *field_c;
} st_4283a0_0;

typedef struct st_4283a0_2 {
    struct st_4283a0_0 *field_0;
    char padding_4[4];
    struct st_4283a0_2 *field_8;
    unsigned int field_c;
} st_4283a0_2;

typedef struct st_4283a0_3 {
    char padding_0[24];
    struct st_4283a0_2 *field_18;
} st_4283a0_3;

typedef struct st_4283a0_1 {
    unsigned int field_0;
} st_4283a0_1;

int sub_4283a0(void)
{
    st_4283a0_3 *v2;  // eax
    st_4283a0_2 *v3;  // ecx
    unsigned int v4;  // esi
    st_4283a0_2 *v5;  // ecx
    unsigned int v0;  // [bp-0x4]

    v3 = v2->field_18;
    if (!v2->field_18)
        return;
    v0 = v4;
    do
    {
        v5 = v3;
        if (v5->field_c != 1)
            v5->field_0->field_c();
    } while ((v3 = (st_4283a0_2 *)v5->field_8, v5->field_8));
    return;
}
