// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4123c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4123c0_0 {
    char padding_0[636];
    unsigned int field_27c;
    char padding_280[76];
    unsigned int field_2cc;
    char padding_2d0[72];
    unsigned int field_318;
    char padding_31c[56];
    unsigned int field_354;
    char padding_358[56];
    unsigned int field_390;
    char padding_394[56];
    unsigned int field_3cc;
    char padding_3d0[56];
    unsigned int field_408;
    char padding_40c[56];
    unsigned int field_444;
    char padding_448[56];
    unsigned int field_480;
    char padding_484[528];
    unsigned int field_694;
    char padding_698[4104];
    unsigned int field_16a0;
    char padding_16a4[16];
    unsigned int field_16b4;
} st_4123c0_0;

st_4123c0_0 * sub_4123c0(unsigned int a0, unsigned int a1, unsigned int a2)
{
    st_4123c0_0 *idx;  // esi
    st_4123c0_0 *iter;  // edi
    int v2;  // ebx
    int v3;  // ebx

    idx->field_27c = idx->field_27c & 0xfffffffe;
    idx->field_2cc = idx->field_2cc & 0xfffffffe;
    idx->field_318 = idx->field_318 & 0xfffffffe;
    idx->field_354 = idx->field_354 & 0xfffffffe;
    idx->field_390 = idx->field_390 & 0xfffffffe;
    idx->field_3cc = idx->field_3cc & 0xfffffffe;
    idx->field_408 = idx->field_408 & 0xfffffffe;
    idx->field_444 = idx->field_444 & 0xfffffffe;
    idx->field_480 = idx->field_480 & 0xfffffffe;
    iter = &idx->padding_484[8];
    v2 = 7;
    do
    {
        v3 = v2;
        sub_477420(iter, 0, 532);
        *((unsigned int *)&iter->padding_0[520]) = 0xffffffff;
        iter = &iter->padding_0[532];
        v2 = v3 - 1;
    } while (v3 - 1 >= 0);
    idx->field_16a0 = idx->field_16a0 & 0xfffffffe;
    idx->field_16b4 = idx->field_16b4 & 0xfffffffe;
    return idx;
}
