// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422c90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void g_4d4f37;
extern char g_4d4f38;

int sub_422c90(void)
{
    void* v3;  // eax
    void* node;  // ecx
    void* iter;  // edi
    unsigned int v14;  // ecx
    unsigned int v15;  // ecx
    void* v5;  // eax
    void* v6;  // eax
    void* v7;  // eax
    unsigned int v8;  // esi
    unsigned int v9;  // edi
    unsigned int v10;  // eax
    void* v11;  // edi
    void* v12;  // edi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    g_4d4f38 = 0;
    node = v3;
    do
    {
        v6 = v5;
        v7 = v6 + 1;
        v5 = v7;
    } while (*((char *)v6));
    v1 = v8;
    v0 = v9;
    v10 = v7 - node;
    v11 = &g_4d4f37;
    do
    {
        v12 = v11;
        iter = v12 + 1;
        v11 = iter;
    } while ((char)v12[1]);
    for (v14 = v10 >> 2; v14; node += 4)
    {
        v14 -= 1;
        *((int *)iter) = *((int *)node);
        iter += 4;
    }
    for (v15 = v10 & 3; v15; node += 1)
    {
        v15 -= 1;
        *((char *)iter) = *((char *)node);
        iter += 1;
    }
    return;
}
