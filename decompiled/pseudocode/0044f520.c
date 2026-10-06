// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f520; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44f520_3 {
    struct st_44f520_0 *field_0;
} st_44f520_3;

typedef struct st_44f520_1 {
    unsigned int field_0;
} st_44f520_1;

typedef struct st_44f520_2 {
    char padding_0[4];
    struct st_44f520_3 *field_4;
} st_44f520_2;

typedef struct st_44f520_0 {
    char padding_0[8];
    struct st_44f520_1 *field_8;
} st_44f520_0;

st_44f520_0 ** sub_44f520(void)
{
    st_44f520_2 *idx;  // esi
    st_44f520_0 **v2;  // eax
    st_44f520_0 **v3;  // eax

    v2 = idx->field_4;
    if (!v2)
        return v2;
    v3 = *(v2)->field_8(v2);
    idx->field_4 = NULL;
    return v3;
}
