// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412450; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void g_4d4f37;
extern char g_4d4f38;

int sub_412450(unsigned int a0, unsigned int a1, void* a2)
{
    void* v5;  // eax
    unsigned int v6;  // ebx
    void* iter;  // edi
    unsigned int v16;  // ecx
    unsigned int v17;  // ecx
    unsigned int v18;  // 4131
    unsigned int v7;  // esi
    unsigned int v8;  // edi
    void* node;  // ecx
    void* v10;  // eax
    void* v11;  // eax
    unsigned int v12;  // eax
    void* v13;  // edi
    void* v14;  // edi
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v5 = a2;
    v4 = v6;
    v3 = v7;
    v2 = v8;
    g_4d4f38 = 0;
    node = v5;
    do
    {
        v10 = v5;
        v11 = v10 + 1;
        v5 = v11;
    } while (*((char *)v10));
    v12 = v11 - node;
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
    v18 = _ccall(13, 15, sub_4692e0(sub_463c10()), 0, 0);
    return (v18 & 1) - 1;
}
