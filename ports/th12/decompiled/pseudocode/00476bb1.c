// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x476bb1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adae0;
extern unsigned int g_4adae4;
extern unsigned int g_4adae8;
extern unsigned int g_4adaec;
extern unsigned int g_4adaf0;
extern int g_4adaf4;
extern unsigned int g_4ae298[4];
extern char g_4ae29c;
extern unsigned int g_4ae2cc[4];
extern char g_4ae2d0;

int sub_476bb1(void)
{
    unsigned int v14;  // edi
    unsigned int v15;  // edi
    unsigned int v23;  // esi
    unsigned int index;  // eax
    unsigned int v25;  // esi
    unsigned int v26;  // edi
    int v27;  // edx
    unsigned int v28;  // eax
    int v29;  // esi
    int v30;  // eax
    unsigned int v31;  // 4101
    unsigned int v16;  // eax
    unsigned int *v32;  // esi
    unsigned int v33;  // ecx
    unsigned int v34;  // eax
    int v35;  // 4102
    unsigned int v36;  // 4112
    unsigned int v17;  // cc_op
    unsigned int v18;  // cc_dep1
    unsigned int v19;  // eax
    unsigned int v20;  // eax
    unsigned int v21;  // 4105
    unsigned int idx;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp+0x4]
    unsigned int idx1;  // [bp+0x8]
    unsigned int v7;  // [bp+0xc]
    unsigned int v8;  // [bp+0x10]
    int v9;  // [bp+0x14]
    unsigned int v10;  // [bp+0x18]
    unsigned int v11;  // [bp+0x1c]
    unsigned int v12;  // [bp+0x20]
    unsigned int v13;  // [bp+0x24]

    v4 = 0;
    v1 = v14;
    v15 = v7;
    v16 = v15;
    if (idx1 == 1)
    {
        v17 = 15;
        v18 = v16 & 0x80000003;
        v19 = v16 & 0x80000003;
        if ((v16 & 0x80000003) < 0)
        {
            v20 = v19 - 1 | 0xfffffffc;
            v17 = 18;
            v18 = v20 + 1;
            v19 = v20 + 1;
        }
        v2 = v19;
        v21 = _ccall(4, v17, v18, 0, 0);
        if ((!(v21 & 1) || (v0 = 100, !(v15 % 100))) && (int)((v15 + 1900) % 400))
        {
            idx = index * 4;
            v23 = *((int *)(idx + (char *)&g_4ae2cc[0]));
        }
        else
        {
            idx = index * 4;
            v23 = *((int *)(idx + (char *)&g_4ae298[0]));
        }
        idx1 = idx;
        v0 = 100;
        v25 = v23 + 1;
        v0 = 7;
        v3 = (v15 + 299) / 400;
        v26 = v7;
        v27 = (v26 * 365 + ((int)(v26 - 1 + ((int)(v26 - 1) >> 31 & 3)) >> 2) + v25 + v3 - (v15 - 1) / 100 - 25563) % 7;
        v28 = v8 * 7 - v27 + v9;
        v29 = (v27 <= v9 ? v25 + v28 - 7 : v25 + v28);
        v15 = v26;
        if (v8 == 5)
        {
            if ((v2 || (v0 = 100, !(v26 % 100))) && (int)((v26 + 1900) % 400))
                v30 = *((int *)&(&g_4ae2d0)[idx1]);
            else
                v30 = *((int *)&(&g_4ae29c)[idx1]);
            v15 = v26;
            if (v29 > v30)
            {
                v29 -= 7;
                v15 = v26;
            }
        }
    }
    else
    {
        if ((v16 & 0x80000003) < 0)
        {
            v31 = _ccall(4, 18, ((v16 & 0x80000003) - 1 | 0xfffffffc) + 1, 0, 0);
            if (v31 & 1)
                goto LABEL_476cd8;
        }
        else if (!(v16 & 0x80000003))
        {
LABEL_476cd8:
            v0 = 100;
            if (v15 % 100)
                goto LABEL_476cff;
        }
        if ((int)((v15 + 1900) % 400))
        {
            v32 = g_4ae2cc[index];
        }
        else
        {
LABEL_476cff:
            v32 = g_4ae298[index];
        }
        v29 = (char *)v32 + v10;
    }
    if (v5 == 1)
    {
        g_4adae4 = v29;
        g_4adae8 = ((v33 * 60 + v11) * 60 + v12) * 1000 + v13;
        g_4adae0 = v15;
        return;
    }
    g_4adaf0 = v29;
    g_4adaf4 = g_4adae8;
    if (sub_4772eb(&v4))
        sub_470dc6(0, 0, 0, 0, 0);
    v34 = v4 * 1000;
    v35 = g_4adaf4;
    g_4adaf4 = g_4adaf4 + v34;
    v36 = _ccall(8, 3, v35, v34, 0);
    if (v36 & 1)
    {
        g_4adaf4 = g_4adaf4 + 86400000;
        g_4adaf0 = g_4adaf0 - 1;
    }
    else if (g_4adaf4 >= 86400000)
    {
        g_4adaf4 = g_4adaf4 - 86400000;
        g_4adaf0 = g_4adaf0 + 1;
    }
    g_4adaec = v15;
    return;
}
