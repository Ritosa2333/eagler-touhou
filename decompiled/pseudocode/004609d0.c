// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4609d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4609d0_1 {
    unsigned int field_0;
} st_4609d0_1;

typedef struct st_4609d0_9 {
    char padding_0[84];
    struct st_4609d0_1 *field_54;
} st_4609d0_9;

typedef struct st_4609d0_0 {
    char padding_0[8];
    struct st_4609d0_1 *field_8;
} st_4609d0_0;

typedef struct st_4609d0_2 {
    char padding_0[72];
    struct st_4609d0_1 *field_48;
} st_4609d0_2;

typedef struct st_4609d0_4 {
    struct st_4609d0_2 *field_0;
} st_4609d0_4;

extern char g_4b50c0;
extern st_4609d0_4 *g_4ce8f0;

int sub_4609d0(void)
{
    unsigned int v11;  // ecx
    unsigned int *idx;  // eax
    int v13;  // edi
    int v14;  // esi
    unsigned int index;  // eax
    unsigned int v16;  // eax
    st_4609d0_9 **v17;  // ecx
    st_4609d0_0 **v0;  // [bp-0x78]
    unsigned int v1;  // [bp-0x44]
    unsigned int v2;  // [bp-0x40]
    st_4609d0_0 *v3;  // [bp-0x3c]
    unsigned int v4;  // [bp-0x3c]
    unsigned int v5;  // [bp-0x38]
    unsigned int v6;  // [bp-0x34]
    unsigned int v7;  // [bp-0x30]
    char v8;  // [bp-0x2c], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x28]

    if (!*((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * *(idx) + v11]) + 288)) + idx[1] * 20)))
        return;
    sub_45a3c0(v13, v14);
    v3 = NULL;
    if (g_4ce8f0->field_0->field_48(g_4ce8f0, 0, 0, 0, &v8))
        return;
    index = idx[1] * 5;
    v16 = *((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * *(idx) + v11]) + 288)) + index * 4));
    if ((*((int *)(*((int *)*((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * *(idx) + v11]) + 288)) + index * 4))) + 72)))(v16, 0, &v3))
    {
        v3->field_8(&v3);
        return;
    }
    v1 = idx[2];
    v5 = idx[5] + idx[3];
    v4 = idx[4] + idx[2];
    v2 = idx[3];
    v6 = idx[6];
    v7 = idx[7];
    v8 = idx[8] + idx[6];
    v9 = idx[9] + idx[7];
    if (!D3DXLoadSurfaceFromSurface(g_4ce8f0, 0, &v6, &v4, 0, &v1, 2, 0))
    {
        v17 = *((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * *(idx) + v11]) + 288)) + idx[1] * 20));
        v0 = NULL;
        *(v17)->field_54(*((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * *(idx) + v11]) + 288)) + idx[1] * 20)), 0);
    }
    (*((int *)(*((int *)NULL) + 8)))(0);
    *(v0)->field_8(v0);
    return;
}
