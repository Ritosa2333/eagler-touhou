// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c710; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4cc550;

int sub_44c710(void)
{
    char v8;  // bl
    int v9;  // eax
    unsigned int i;  // esi
    char *v19;  // ecx
    unsigned int v20;  // 4139
    unsigned int v21;  // ebp
    unsigned int j;  // esi
    char *v23;  // ecx
    unsigned int v25;  // 4139
    unsigned int v26;  // ebp
    unsigned int k;  // esi
    char *v10;  // edx
    char *v28;  // ecx
    unsigned int v29;  // 4139
    unsigned int v30;  // esi
    unsigned int l;  // ecx
    char v32;  // dl
    char v33;  // bl
    unsigned int v34;  // 4121
    unsigned int v11;  // esi
    unsigned int v12;  // edi
    char *node;  // edi
    char v14;  // bl
    unsigned int v15;  // 4129
    char *v16;  // ecx
    unsigned int v17;  // eax
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0xc]
    char *iter;  // [bp-0x8]
    unsigned int idx;  // [bp-0x4]
    char *v5;  // [bp+0x4]
    char *v6;  // [bp+0x8]
    char *v7;  // [bp+0xc]

    v8 = 128;
    v2 = 0;
    if (!v7)
    {
        v7 = sub_46d04a(v9);
        if (!v7)
            return;
    }
    v10 = v5;
    v1 = v11;
    v0 = v12;
    node = v10;
    iter = v7;
    idx = 1;
    while (1)
    {
        if (v8 == 128)
        {
            v2 = *(node);
            if (node - v10 >= v6)
                v2 = 0;
            else
                node += 1;
        }
        v14 = v8 >> 1;
        v15 = _ccall(4, 25, v8 >> 1, v8, 0);
        if (v15 & 1)
            v14 = 128;
        v16 = node;
        if (v8 & v2)
        {
            v17 = 0;
            i = 128;
            v19 = v16 - v10;
            do
            {
                if (v14 == 128)
                {
                    v2 = *(node);
                    if (v19 >= v6)
                    {
                        v2 = 0;
                    }
                    else
                    {
                        node += 1;
                        v19 += 1;
                    }
                }
                if (v14 & (char)v2)
                    v17 |= i;
                i >>= 1;
                v8 = v14 >> 1;
                v20 = _ccall(4, 25, v14 >> 1, v14, 0);
                if (v20 & 1)
                    v8 = 128;
                v14 = v8;
            } while (i);
            *(iter) = v17;
            iter += 1;
            (&g_4cc550)[idx] = v17;
            idx = idx + 1 & 0x1fff;
        }
        else
        {
            v21 = 0;
            j = 0x1000;
            v23 = v16 - v10;
            do
            {
                if (v14 == 128)
                {
                    v2 = *(node);
                    if (v23 >= v6)
                    {
                        v2 = 0;
                    }
                    else
                    {
                        node += 1;
                        v23 += 1;
                    }
                }
                if (v14 & (char)v2)
                    v21 |= j;
                j >>= 1;
                v25 = _ccall(4, 25, v14 >> 1, v14, 0);
                v14 >>= 1;
                if (v25 & 1)
                    v14 = 128;
            } while (j);
            if (!v21)
                break;
            v26 = 0;
            k = 8;
            v28 = node - v10;
            do
            {
                if (v14 == 128)
                {
                    v2 = *(node);
                    if (v28 >= v6)
                    {
                        v2 = 0;
                    }
                    else
                    {
                        node += 1;
                        v28 += 1;
                    }
                }
                if (v14 & (char)v2)
                    v26 |= k;
                k >>= 1;
                v8 = v14 >> 1;
                v29 = _ccall(4, 25, v14 >> 1, v14, 0);
                if (v29 & 1)
                    v8 = 128;
                v14 = v8;
            } while (k);
            v30 = v26 + 2;
            l = 0;
            if (v30 >= NULL)
            {
                do
                {
                    v32 = (&g_4cc550)[l + v21 & 0x1fff];
                    *(iter) = v32;
                    iter += 1;
                    (&g_4cc550)[idx] = v32;
                    l += 1;
                    idx = idx + 1 & 0x1fff;
                } while (l <= v30);
            }
        }
        v10 = v5;
    }
    if (v14 != 128)
    {
        do
        {
            v33 = v14 >> 1;
            v34 = _ccall(4, 25, v14 >> 1, v14, 0);
        } while (!(v34 & 1) && (v14 = v33, v14 != 128));
    }
    return;
}
