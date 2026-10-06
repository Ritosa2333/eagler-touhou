// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d90b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4d642c;
extern int g_4d6430;

int sub_46d90b(int a0)
{
    int v4;  // edi
    int v5;  // ecx
    unsigned int *v6;  // edi
    unsigned int *v7;  // esi
    unsigned int *v8;  // ebx
    unsigned int *v9;  // edi
    unsigned int *v10;  // eax
    unsigned int *v11;  // eax
    int v12;  // eax
    unsigned int *v13;  // eax
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    unsigned int *v2;  // [bp-0x8]
    char v3;  // [bp+0x0]

    v6 = sub_4751de(g_4d6430, v4, v0, v1, v5, &v3);
    v2 = v6;
    v7 = sub_4751de(g_4d642c);
    if (v7 < v6 || !(v8 = (unsigned int *)(v7 - v6), v8 + 4 >= 0x4))
    {
LABEL_46d9be:
        return 0;
    }
    v9 = sub_4778ff(v6);
    if (v9 < v8 + 1)
    {
        v10 = 0x800;
        if (v9 < 0x800)
            v10 = v9;
        v11 = v10 + v9;
        if ((v11 < v9 || (v12 = (int)sub_47785f(v6, v11), !v12)) && !(v13 = v9 + 16, v13 >= v9 && (v12 = (int)sub_47785f(v6, v13), v12)))
            goto LABEL_46d9be;
        v7 = v12 + (v8 >> 2) * 4;
        g_4d6430 = sub_475163(v12);
    }
    *(v7) = sub_475163(a0);
    g_4d642c = sub_475163(v7 + 1);
    return a0;
}
