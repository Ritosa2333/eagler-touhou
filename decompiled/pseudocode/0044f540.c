// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f540; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44f540_1 {
    unsigned int field_0;
} st_44f540_1;

typedef struct st_44f540_0 {
    char padding_0[8];
    struct st_44f540_1 *field_8;
} st_44f540_0;

st_44f540_0 ** sub_44f540(void)
{
    st_44f540_0 *idx;  // esi
    st_44f540_0 **v2;  // eax
    st_44f540_0 **v3;  // eax

    v2 = idx->field_8;
    if (!v2)
        return v2;
    v3 = *(v2)->field_8(v2);
    idx->field_8 = NULL;
    return v3;
}
