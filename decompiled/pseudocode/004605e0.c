// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4605e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4605e0_3 {
    struct st_4605e0_0 *field_0;
} st_4605e0_3;

typedef struct st_4605e0_1 {
    unsigned int field_0;
} st_4605e0_1;

typedef struct st_4605e0_2 {
    struct st_4605e0_3 *field_0;
    int field_4;
} st_4605e0_2;

typedef struct st_4605e0_0 {
    char padding_0[8];
    struct st_4605e0_1 *field_8;
} st_4605e0_0;

int sub_4605e0(void)
{
    st_4605e0_2 *idx;  // esi
    st_4605e0_0 **v2;  // eax
    int v3;  // eax
    int v4;  // eax

    v2 = idx->field_0;
    if (v2)
    {
        *(v2)->field_8(v2);
        idx->field_0 = NULL;
    }
    v3 = idx->field_4;
    if (!v3)
        return v3;
    v4 = sub_46c941(v3);
    idx->field_4 = 0;
    return v4;
}
