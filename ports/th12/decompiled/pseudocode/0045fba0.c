// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45fba0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45fba0_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_45fba0_1;

typedef struct st_45fba0_0 {
    char padding_0[12];
    struct st_45fba0_1 *field_c;
    unsigned int field_10;
} st_45fba0_0;

extern unsigned int g_4a2338[4];
extern unsigned int g_4a235c[4];
extern int g_4ce8f0;

unsigned int * sub_45fba0(int a0)
{
    st_45fba0_0 *idx;  // esi
    int v1;  // ebx
    unsigned int index;  // edi
    int v3;  // ebp

    idx->field_10 = idx->field_10 & 0xfffffffe;
    D3DXCreateTexture(g_4ce8f0, a0, v1, 1, 0, g_4a2338[index], 1, idx, v3);
    idx->field_c = g_4a235c[index];
    return g_4a235c[index] * v1 * a0;
}
