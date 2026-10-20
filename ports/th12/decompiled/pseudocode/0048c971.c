// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c971; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48c971_0 {
    char padding_0[12];
    unsigned int field_c;
} st_48c971_0;

typedef struct st_48c971_1 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[4];
    struct st_48c971_0 *field_c;
} st_48c971_1;


unsigned int sub_48c971(void)
{
    unsigned int v1;  // eax
    unsigned long v2;  // ldt
    unsigned long v3;  // gdt
    unsigned short v4;  // fs
    unsigned long long v5;  // 4124
    st_48c971_1 *v6;  // ecx

    v1 = 0;
    v5 = _ccall(v2, v3, v4, 0);
    v6 = *((int *)(unsigned int)v5);
    if (v6->field_4 == sub_48c8a8 && v6->padding_8 == v6->field_c->field_c)
        v1 = 1;
    return v1;
}
