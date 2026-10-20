// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410b30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_410b30_0 {
    char padding_0[20];
    int field_14;
} st_410b30_0;

extern st_410b30_0 *g_4b43d8;
extern void g_4d4f37;
extern char g_4d4f38;

int sub_410b30(void)
{
    unsigned int v5;  // esi
    void* v6;  // eax
    void* iter;  // edi
    unsigned int v16;  // ecx
    unsigned int v17;  // ecx
    unsigned int v18;  // eax
    void* node;  // ecx
    void* v8;  // eax
    void* v9;  // eax
    void* v10;  // eax
    unsigned int v11;  // edi
    unsigned int v12;  // eax
    void* v13;  // edi
    void* v14;  // edi
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]

    v3 = v5;
    g_4d4f38 = 0;
    node = v6;
    do
    {
        v9 = v8;
        v10 = v9 + 1;
        v8 = v10;
    } while (*((char *)v9));
    v2 = v11;
    v12 = v10 - node;
    v13 = &g_4d4f37;
    do
    {
        v14 = v13;
        iter = v14 + 1;
        v13 = iter;
    } while ((char)v14[1]);
    for (v16 = v12 >> 2; v16; node += 4)
    {
        v16 -= 1;
        *((int *)iter) = *((int *)node);
        iter += 4;
    }
    v1 = 0;
    v17 = v12 & 3;
    for (v0 = 0; v17; node += 1)
    {
        v17 -= 1;
        *((char *)iter) = *((char *)node);
        iter += 1;
    }
    v18 = sub_463c10();
    if (g_4b43d8->field_14)
    {
        sub_46c941(g_4b43d8->field_14);
        g_4b43d8->field_14 = 0;
    }
    g_4b43d8->field_14 = v18;
    return;
}
