// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460b10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460b10_1 {
    unsigned int field_0;
} st_460b10_1;

typedef struct st_460b10_10 {
    char padding_0[72];
    struct st_460b10_1 *field_48;
} st_460b10_10;

typedef struct st_460b10_0 {
    char padding_0[8];
    struct st_460b10_1 *field_8;
} st_460b10_0;

extern char g_4b50c0;

int sub_460b10(void)
{
    unsigned int v2;  // eax
    unsigned int v3;  // edi
    int v12;  // ebx
    unsigned int v4;  // edx
    unsigned int v5;  // ecx
    unsigned int v6;  // ebx
    st_460b10_10 *v7;  // edx
    unsigned int v8;  // eax
    st_460b10_0 **v9;  // edi
    int v10;  // edi
    int v11;  // ebp
    unsigned int v0;  // [bp+0x4]
    unsigned int v1;  // [bp+0x8]

    v3 = v2 * 20;
    if (!*((int *)(v3 + *((int *)(*((int *)&(&g_4b50c0)[4 * v0 + v4]) + 288)))))
        return;
    v6 = v5 * 20;
    if (!*((int *)(v6 + *((int *)(*((int *)&(&g_4b50c0)[4 * v1 + v4]) + 288)))))
        return;
    sub_45a3c0();
    v7 = *((int *)*((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * v0 + v4]) + 288)) + v3)));
    if (v7->field_48(*((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * v0 + v4]) + 288)) + v3)), 0, &v0))
        return;
    v8 = *((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * v5 + v4]) + 288)) + v6));
    if ((*((int *)(*((int *)*((int *)(*((int *)(*((int *)&(&g_4b50c0)[4 * v5 + v4]) + 288)) + v6))) + 72)))(v8, 0, &esi))
    {
        *(v9)->field_8(v9);
        return;
    }
    if (D3DXLoadSurfaceFromSurface(v10, 0, v11, 0, 0, v12, -0x1, 0))
    {
        (*((int *)(*((int *)0xffffffff) + 8)))(0xffffffff);
        (*((int *)(*((int *)NULL) + 8)))(0);
    }
    else
    {
        (*((int *)(*((int *)0xffffffff) + 8)))(0xffffffff);
        (*((int *)(*((int *)NULL) + 8)))(0);
    }
    return;
}
