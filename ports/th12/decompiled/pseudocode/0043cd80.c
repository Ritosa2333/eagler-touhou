// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43cd80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43cd80_4 {
    char padding_0[28];
    struct st_43cd80_0 *field_1c;
    struct st_43cd80_1 *field_20;
} st_43cd80_4;

typedef struct st_43cd80_1 {
    char padding_0[4];
    struct st_43cd80_0 *field_4;
} st_43cd80_1;

typedef struct st_43cd80_0 {
    char padding_0[8];
    struct st_43cd80_1 *field_8;
} st_43cd80_0;

st_43cd80_1 * sub_43cd80(st_43cd80_4 *idx)
{
    st_43cd80_1 *v1;  // eax

    if (idx->field_1c)
        idx->field_1c->field_8 = idx->field_20;
    v1 = idx->field_20;
    if (v1)
        v1->field_4 = idx->field_1c;
    idx->field_20 = NULL;
    idx->field_1c = NULL;
    return v1;
}
