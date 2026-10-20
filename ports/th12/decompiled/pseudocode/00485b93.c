// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485b93; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_485b93_0 {
    char padding_0[160];
    int field_a0;
    unsigned int field_a4;
    char padding_a8[8];
    unsigned int field_b0;
    unsigned int field_b4;
    unsigned int field_b8;
} st_485b93_0;


int sub_485b93(unsigned int a0)
{
    st_485b93_0 *idx;  // esi
    int v3;  // edi
    unsigned int v4;  // edi
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    int v0;  // [bp-0x84]
    Byte v1[120];  // [bp-0x80]

    idx = sub_475467() + 156;
    v4 = sub_485b44(v3, v0);
    if (!GetLocaleInfoA(v4, (-(0 < *((int *)&idx->padding_0[20])) & 0xfffff005) + 4098, v1, 120))
    {
        *((unsigned int *)&idx->padding_0[8]) = 0;
        return v5;
    }
    if (sub_46e3f8(*((int *)&idx->padding_0[4]), v1) || !sub_485b20(v4))
        return v6;
    *((unsigned int *)&idx->padding_0[8]) = *((int *)&idx->padding_0[8]) | 4;
    *((unsigned int *)&idx->padding_0[28]) = v4;
    *((unsigned int *)&idx->padding_0[24]) = v4;
    return v6;
}
