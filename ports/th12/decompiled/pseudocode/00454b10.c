// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454b10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454b10_0 {
    char padding_0[4];
    unsigned int field_4;
    struct st_454b10_1 *field_8;
    unsigned int field_c;
} st_454b10_0;

typedef struct st_454b10_1 {
    unsigned int field_0;
} st_454b10_1;

extern unsigned int g_4ae5a0;

unsigned int * sub_454b10(st_454b10_0 *idx, unsigned int a1)
{
    unsigned int *v1;  // eax
    unsigned int *v2;  // eax

    idx->field_4 = 0xffffffff;
    v1 = &g_4ae5a0;
    do
    {
        v2 = v1;
        v1 = v2;
    } while (*(v1) != a1 && (v1 = v2 + 20, v2 != 0xffffffec));
    idx->field_c = a1;
    idx->field_8 = v1;
    return v1;
}
