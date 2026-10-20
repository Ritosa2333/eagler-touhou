// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9f6b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_186bc225;
extern unsigned int g_78263ba2;

int sub_4d9f6b(void)
{
    unsigned int v2;  // eax
    unsigned int index;  // edi
    unsigned int v4;  // ecx
    unsigned int v5;  // edx
    unsigned int v6;  // ebx
    unsigned int *v7;  // eax
    unsigned int v8;  // ebp
    unsigned int v9;  // 4158
    unsigned int idx;  // esi
    unsigned int v0;  // [bp-0x4]

    g_78263ba2 = v2;
    v0 = index;
    if (!(v4 != 1 & v5 == v6))
    {
        v7 = (short)(v2 - 1);
        *((unsigned int *)(index + 679348397)) = *((int *)(index + 679348397)) + v8;
        if (v6 & *(v7))
            goto LABEL_0x4d9f46;
        v9 = _ccall(15, v6 & *(v7), 0, 0);
        *((unsigned int *)(idx + 1445935904)) = *((int *)(idx + 1445935904)) + v6 + (v9 & 1);
        return;
    }
    g_186bc225 = v2;
    __unsupported_jumpkind_Ijk_Privileged()
}
