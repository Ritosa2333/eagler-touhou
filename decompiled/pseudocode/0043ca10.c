// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43ca10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43ca10_0 {
    char padding_0[6304];
    struct st_43ca10_1 *field_18a0;
} st_43ca10_0;

typedef struct st_43ca10_1 {
    char field_0;
} st_43ca10_1;

int sub_43ca10(void)
{
    st_43ca10_0 *idx;  // eax
    char v2;  // dl

    idx->field_18a0->field_0 = v2;
    idx->field_18a0 = idx->field_18a0 + 1;
    return;
}
