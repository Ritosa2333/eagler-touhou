// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4359a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4b4514;

unsigned int * sub_4359a0(unsigned int *idx)
{
    unsigned long v2;  // ldt
    unsigned long v3;  // gdt
    int v12;  // ecx
    unsigned int *iter;  // eax
    int v14;  // ecx
    int v16;  // ebp
    unsigned int *node;  // esi
    int v18;  // ebp
    unsigned int *iter1;  // eax
    int v20;  // ecx
    int v21;  // ecx
    unsigned short v4;  // fs
    unsigned long long v23;  // 4128
    unsigned long long v5;  // 4136
    unsigned int v6;  // eax
    unsigned long long v7;  // 4158
    int v8;  // edi
    int v9;  // esi
    int v10;  // ebp
    int v11;  // ebx
    unsigned int v0;  // [bp-0xc]
    char v1;  // [bp-0x4]

    v5 = _ccall(v2, v3, v4, 0);
    v6 = *((int *)(unsigned int)v5);
    v0 = *((int *)(unsigned int)v5);
    v7 = _ccall(v2, v3, v4, 0);
    *((unsigned int **)(unsigned int)v7) = &v0;
    sub_4027e0(g_4ad138 ^ &v8, v8, v9, v10, v11, v6, sub_4970f9, -0x1);
    sub_4027e0(g_4ad138 ^ &v8, v8, v9, v10, v11, v6, sub_4970f9, 0);
    v1 = 1;
    idx[656] = idx[656] & 0xfffffffe;
    idx[661] = idx[661] & 0xfffffffe;
    v12 = 0xff;
    iter = idx + 666;
    do
    {
        v14 = v12;
        *(iter) = *(iter) & 0xfffffffe;
        iter += 30;
        v12 = v14 - 1;
    } while (v14 - 1 >= 0);
    v16 = 7;
    node = idx + 8394;
    do
    {
        v18 = v16;
        sub_402790(8);
        sub_402790(8);
        *(node) = *(node) & 0xfffffffe;
        node += 57;
        v16 = v18 - 1;
    } while (v18 - 1 >= 0);
    iter1 = idx + 8825;
    v20 = 128;
    do
    {
        v21 = v20;
        *(iter1) = *(iter1) & 0xfffffffe;
        iter1 += 29;
        v20 = v21 - 1;
    } while (v21 - 1 >= 0);
    idx[12548] = idx[12548] & 0xfffffffe;
    idx[12556] = idx[12556] & 0xfffffffe;
    sub_477420(idx, 0, 50588);
    g_4b4514 = idx;
    v23 = _ccall(v2, v3, v4, 0);
    *((int *)(unsigned int)v23) = v8;
    return idx;
}
