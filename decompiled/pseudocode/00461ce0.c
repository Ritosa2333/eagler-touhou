// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461ce0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461ce0_1 {
    struct st_461ce0_0 *field_0;
    struct st_461ce0_1 *field_4;
} st_461ce0_1;

typedef struct st_461ce0_0 {
    char padding_0[1148];
    unsigned int field_47c;
} st_461ce0_0;

typedef struct st_461ce0_2 {
    char padding_0[20];
    struct st_461ce0_1 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_461ce0_2;

int sub_461ce0(void)
{
    int *v1;  // eax
    st_461ce0_2 *idx;  // eax
    unsigned int v3;  // 4105
    st_461ce0_1 *v4;  // eax
    st_461ce0_1 *v5;  // eax

    idx = sub_461920(*(v1));
    if (!idx)
        return;
    v3 = idx->field_18;
    idx->field_47c = idx->field_47c & 0xfffbffff | 2;
    if (v3)
        return;
    v4 = idx->field_14;
    if (!idx->field_14)
        return;
    do
    {
        v5 = v4;
        v5->field_0->field_47c = v5->field_0->field_47c | 2;
        v5->field_0->field_47c = v5->field_0->field_47c & 0xfffbffff;
        v4 = v5->field_4;
    } while (v5->field_4);
    return;
}
