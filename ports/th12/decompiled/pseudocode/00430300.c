// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x430300; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_430300_0 {
    char padding_0[228];
    struct st_430300_1 *field_e4;
} st_430300_0;

typedef struct st_430300_2 {
    char padding_0[8];
    struct st_430300_3 *field_8;
    char padding_c[2436];
    unsigned int field_990;
} st_430300_2;

typedef struct st_430300_3 {
    struct st_430300_0 *field_0;
} st_430300_3;

typedef struct st_430300_1 {
    unsigned int field_0;
} st_430300_1;

unsigned int sub_430300(void)
{
    st_430300_2 *idx;  // edi
    int v2;  // esi
    st_430300_0 **v3;  // eax

    if (!idx->field_990)
        return 0;
    sub_45a3c0(v2);
    v3 = idx->field_8;
    idx->field_990 = 0;
    return *(v3)->field_e4(v3, 28, 0);
}
