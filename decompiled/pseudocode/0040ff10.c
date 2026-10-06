// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40ff10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4cea94;

int sub_40ff10(void)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // edi
    unsigned int *v13;  // eax
    unsigned int v14;  // ebx
    unsigned int v15;  // eax
    void* v5;  // esi
    unsigned int v6;  // eax
    int v7;  // ebp
    unsigned int *idx;  // esi
    unsigned int v9;  // edi
    unsigned int v10;  // eax
    unsigned int v11;  // edx
    unsigned int v12;  // ebp
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0x4]
    unsigned int v2;  // [bp+0x4]

    v1 = v3;
    v0 = v4;
    if (!g_4cea94)
    {
        memset(v5, 0, 24);
        return;
    }
    v7 = v6 * 4 - 1;
    *(idx) = v6;
    idx[1] = v2;
    idx[2] = sub_46d04a(v7);
    v9 = v2 * v6;
    idx[3] = sub_46d04a(v7);
    idx[4] = sub_46d04a(v9 * 28);
    v10 = sub_46d04a(v9 * 12);
    v11 = *(idx) - 1;
    v12 = 0;
    idx[5] = v10;
    if (v11 <= 0)
        return;
    while (1)
    {
        v13 = sub_4314d0();
        v14 = v12 * 4;
        *((unsigned int *)(v14 + idx[2])) = *(v13);
        v15 = sub_461920(*((int *)(idx[2] + v14)));
        if (!v15)
            *((unsigned int *)(idx[2] + v14)) = 0;
        *((unsigned int *)(v14 + idx[3])) = v15;
        *((void* *)(*((int *)(v14 + idx[3])) + 1164)) = sub_40ff00;
        *((unsigned int **)(*((int *)(v14 + idx[3])) + 1176)) = idx;
        *((unsigned int *)(*((int *)(v14 + idx[3])) + 1148)) = *((int *)(*((int *)(v14 + idx[3])) + 1148)) & 0xffffff1f;
        v12 += 1;
        if (v12 >= *(idx) - 1)
            break;
    }
    return;
}
