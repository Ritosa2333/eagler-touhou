// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466592; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466592_2 {
    struct st_466592_0 *field_0;
} st_466592_2;

typedef struct st_466592_1 {
    unsigned int field_0;
} st_466592_1;

typedef struct st_466592_0 {
    char padding_0[48];
    struct st_466592_1 *field_30;
} st_466592_0;

typedef struct st_466592_3 {
    char padding_0[4];
    struct st_466592_2 *field_4;
    char padding_8[24];
    unsigned int field_20;
    unsigned int field_24;
    char padding_28[8];
    unsigned int field_30;
} st_466592_3;

int sub_466592(unsigned int a0)
{
    int v1;  // eax
    st_466592_3 *idx;  // esi
    st_466592_0 **v3;  // eax

    sub_466bc0(v1);
    v3 = idx->field_4->field_0;
    idx->field_30 = 1;
    return *(v3)->field_30(v3, 0, idx->field_20, idx->field_24);
}
