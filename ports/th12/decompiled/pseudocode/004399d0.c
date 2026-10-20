// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4399d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4399d0_0 {
    char padding_0[32];
    unsigned int field_20;
} st_4399d0_0;

typedef struct st_4399d0_1 {
    char padding_0[2604];
    struct st_4399d0_0 *field_a2c;
    char padding_a30[47976];
    unsigned int field_c598;
} st_4399d0_1;

extern int g_4b0c48;
extern int g_4b0cd4;

int sub_4399d0(void)
{
    unsigned int v4;  // ecx
    unsigned int v5;  // ebx
    unsigned int v6;  // eax
    unsigned int v7;  // esi
    st_4399d0_1 *idx;  // eax
    char *v9;  // esi
    char v10;  // al
    char *v11;  // esi
    unsigned int v12;  // edi
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v1 = v5;
    v6 = g_4b0c48 / g_4b0cd4;
    v0 = v7;
    if (idx->field_c598)
        v6 = v6 + idx->field_a2c->field_20 + 1;
    v9 = *((int *)&idx->field_a2c[0x11].padding_0[4 + 8 * v6]);
    v10 = *(v9);
    if (*(v9) < 0)
        return;
    do
    {
        v11 = v9;
        if (v12 % v10 == v11[1])
            sub_439630(v11, v12);
    } while ((v10 = v11[52], v9 = v11 + 52, v11[52] >= 0));
    return;
}
