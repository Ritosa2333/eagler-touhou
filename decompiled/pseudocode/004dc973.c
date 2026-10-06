// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc973; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc973_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[4];
    struct st_4dc973_0 *field_10;
    char padding_14[140];
    struct st_4dc973_0 *field_a0;
    char padding_a4[140];
    struct st_4dc973_0 *field_130;
    char padding_134[140];
    struct st_4dc973_0 *field_1c0;
    char padding_1c4[152];
    unsigned int field_25c;
    void* field_260;
} st_4dc973_0;


int sub_4dc973(st_4dc973_0 *idx, unsigned int a1, unsigned int a2, unsigned int a3)
{
    st_4dc973_0 *v0;  // eax
    unsigned int v1;  // ecx
    void* iter;  // edi

    idx->field_0 = a3;
    v0 = &idx->field_4;
    v0->field_0 = a2;
    v0->field_4 = 32;
    idx->field_10 = v0;
    idx->field_a0 = v0;
    idx->field_130 = v0;
    idx->field_1c0 = v0;
    v1 = 189;
    memset(&idx->padding_1c4[140], 0, 12);
    iter = idx->field_260;
    for (idx->field_25c = 0; v1; iter += 4)
    {
        v1 -= 1;
        *((unsigned int *)iter) = 0;
    }
    *((char *)iter) = 0;
    return sub_4dc9d4();
}
