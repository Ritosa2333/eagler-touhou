// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461d80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461d80_1 {
    struct st_461d80_0 *field_0;
    struct st_461d80_1 *field_4;
} st_461d80_1;

typedef struct st_461d80_0 {
    char padding_0[1148];
    unsigned int field_47c;
} st_461d80_0;

typedef struct st_461d80_2 {
    char padding_0[20];
    struct st_461d80_1 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_461d80_2;

int sub_461d80(void)
{
    int *v1;  // eax
    st_461d80_2 *idx;  // eax
    st_461d80_1 *v3;  // eax
    st_461d80_1 *v4;  // eax

    idx = sub_461920(*(v1));
    if (!idx)
        return;
    idx->field_47c = idx->field_47c & 0xfffffffd;
    if (idx->field_18)
        return;
    v3 = idx->field_14;
    if (!idx->field_14)
        return;
    do
    {
        v4 = v3;
        v4->field_0->field_47c = v4->field_0->field_47c & 0xfffffffd;
        v3 = v4->field_4;
    } while (v4->field_4);
    return;
}
