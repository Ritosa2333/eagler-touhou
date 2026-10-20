// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4695e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4695e0_0 {
    unsigned int field_0;
    int field_4;
} st_4695e0_0;

typedef struct st_4695e0_1 {
    char padding_0[4];
    struct st_4695e0_0 *field_4;
} st_4695e0_1;

int sub_4695e0(unsigned int a0)
{
    int v0;  // eax
    int v1;  // eax
    st_4695e0_1 *idx;  // esi
    st_4695e0_0 *v3;  // eax

    v1 = sub_4694f0(v0);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_4->field_4 = v1;
    v3 = idx->field_4;
    v3->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    return !v3->field_4;
}
