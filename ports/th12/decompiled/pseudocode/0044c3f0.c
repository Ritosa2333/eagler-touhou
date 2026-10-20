// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c3f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4540[4];
extern unsigned int g_4b4544[4];
extern unsigned int g_4b4548[4];
extern unsigned int g_4b454c;
extern unsigned int g_4b4550;
extern unsigned int g_4b4554;
extern unsigned int g_4cc548;
extern char g_4cc550;
extern char g_4cc551;

unsigned int sub_44c3f0(char *a0, unsigned int *a1, unsigned int *a2)
{
    unsigned int v12;  // ebx
    int v13;  // esi
    int v22;  // esi
    unsigned int v23;  // edx
    int v24;  // ecx
    char v25;  // al
    unsigned int v26;  // 4130
    unsigned int v27;  // eax
    unsigned int i;  // eax
    char v29;  // 4096
    unsigned int v30;  // 4124
    char v31;  // al
    int v14;  // ebx
    unsigned int v32;  // 4121
    unsigned int v33;  // ecx
    unsigned int j;  // ecx
    unsigned int v36;  // 4121
    unsigned int v37;  // ecx
    unsigned int k;  // ecx
    unsigned int v40;  // 4125
    unsigned int v41;  // edi
    char *v15;  // edi
    unsigned int idx;  // eax
    unsigned int idx1;  // eax
    unsigned int idx2;  // edx
    unsigned int index;  // ecx
    unsigned int v46;  // eax
    unsigned int *v47;  // eax
    unsigned int v48;  // eax
    char *v49;  // ecx
    unsigned int v50;  // eax
    char v51;  // 4096
    char *iter;  // ebp
    unsigned int v52;  // 4124
    unsigned int v53;  // eax
    unsigned int v54;  // eax
    char v55;  // 4096
    unsigned int v56;  // 4124
    int v17;  // edi
    int v18;  // ebp
    int node;  // eax
    unsigned int *v20;  // edx
    unsigned int v21;  // ecx
    unsigned int v0;  // [bp-0x28]
    char v1;  // [bp-0x25]
    char *v2;  // [bp-0x24]
    char *v3;  // [bp-0x20]
    int v4;  // [bp-0x1c]
    int v5;  // [bp-0x18]
    char *iter1;  // [bp-0x14], Other Possible Types: int
    char v7;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int l;  // [bp-0xc], Other Possible Types: int
    unsigned int v9;  // [bp-0x8]
    char *v10;  // [bp-0x4]
    unsigned int v11;  // [bp+0x0]

    *((char *)&v2 + 3) = 128;
    v12 = 0;
    v10 = sub_46d04a(a1 * 2, v13, v14);
    if (!v10)
        return 0;
    v15 = a0;
    iter = v10;
    *(a2) = 0;
    sub_44c920(v17, v18);
    node = 0;
    v4 = 1;
    v20 = NULL;
    do
    {
    } while (v20 < a1 && (v21 = (unsigned int)*(v15), v15 += 1, v20 += 1, v21 != 0xffffffff && ((&g_4cc551)[node] = (char)v21, node = (int)(node + 1), node < 18)));
    v22 = 0;
    v23 = 0;
    v24 = node;
    v3 = v15;
    iter1 = v24;
    g_4cc548 = 1;
    g_4b454c = 0x2000;
    g_4b4554 = 0;
    g_4b4550 = 0;
    v5 = 0;
    l = 0;
    if (node > NULL)
    {
        while (1)
        {
            if (v22 > v24)
            {
                v4 = v24;
                v22 = v24;
            }
            v25 = *((char *)((void*)&v0 + 3));
            if (v22 <= 2)
            {
                v12 |= v25;
                v22 = 1;
                v1 = v25 >> 1;
                v26 = _ccall(4, 25, v25 >> 1, v25, 0);
                if (v26 & 1)
                {
                    *(iter) = v12;
                    iter += 1;
                    v12 = 0;
                    v1 = 128;
                }
                v27 = 128;
                do
                {
                    i = v27;
                    if ((char)i & *((char *)(v3 + &g_4cc550)))
                        v12 |= v1;
                    v29 = v1;
                    v1 = v29 >> 1;
                    v30 = _ccall(4, 25, v29 >> 1, v29, 0);
                    if (v30 & 1)
                    {
                        *(iter) = v12;
                        iter += 1;
                        v12 = 0;
                        v1 = 128;
                    }
                    v27 = i >> 1;
                } while (i > 1);
            }
            else
            {
                v31 = v25 >> 1;
                v32 = _ccall(4, 25, v25 >> 1, v25, 0);
                if (v32 & 1)
                {
                    *(iter) = v12;
                    *((char *)&v0 + 3) = 128;
                    v31 = *((char *)((void*)&v0 + 3));
                    iter += 1;
                    v12 = 0;
                }
                v33 = 0x1000;
                do
                {
                    j = v33;
                    if (j & v23)
                        v12 |= v31;
                    v36 = _ccall(4, 25, v31 >> 1, v31, 0);
                    v31 >>= 1;
                    if (v36 & 1)
                    {
                        *(iter) = v12;
                        *((char *)&v0 + 3) = 128;
                        v31 = *((char *)((void*)&v0 + 3));
                        iter += 1;
                        v12 = 0;
                    }
                    v33 = j >> 1;
                } while (j > 1);
                v37 = 8;
                do
                {
                    k = v37;
                    if (v22 - 3 & k)
                        v12 |= v31;
                    *((char *)&v0 + 3) = v31 >> 1;
                    v40 = _ccall(4, 25, v31 >> 1, v31, 0);
                    v31 >>= 1;
                    if (v40 & 1)
                    {
                        *(iter) = v12;
                        *((char *)&v0 + 3) = 128;
                        v31 = *((char *)((void*)&v0 + 3));
                        iter += 1;
                        v12 = 0;
                    }
                    v37 = k >> 1;
                } while (k > 1);
                if (v22 <= 0)
                    goto LABEL_44c6bb;
            }
            l = v22;
            iter1 = &v2[-1 * v11];
            do
            {
                v41 = v3 + 18 & 0x1fff;
                idx = v41 * 6;
                idx1 = idx * 2;
                if (!*((int *)(2 * idx + (char *)&g_4b4540[0])))
                    continue;
                idx2 = *((int *)(idx1 + (char *)&g_4b4548[0]));
                if (!*((int *)(idx1 + (char *)&g_4b4548[0])))
                {
                    idx2 = *((int *)(idx1 + (char *)&g_4b4544[0]));
                    goto LABEL_44c5cf;
                }
                else if (!*((int *)(idx1 + (char *)&g_4b4544[0])))
                {
LABEL_44c5cf:
                    g_4b4540[3 * idx2] = *((int *)(2 * idx + (char *)&g_4b4540[0]));
                    index = *((int *)(idx1 + (char *)&g_4b4540[0])) * 12;
                    if (*((int *)(index + (char *)&g_4b4548[0])) == v41)
                    {
                        *((unsigned int *)(index + (char *)&g_4b4548[0])) = idx2;
                        *((unsigned int *)(idx1 + (char *)&g_4b4540[0])) = 0;
                    }
                    else
                    {
                        *((unsigned int *)(index + (char *)&g_4b4544[0])) = idx2;
                        *((unsigned int *)(idx1 + (char *)&g_4b4540[0])) = 0;
                    }
                }
                else
                {
                    v46 = *((int *)(idx1 + (char *)&g_4b4544[0])) * 3;
                    v47 = &g_4b4548[v46];
                    if (g_4b4548[v46])
                    {
                        do
                        {
                            v48 = *(v47) * 3;
                            v47 = &g_4b4548[v48];
                        } while (g_4b4548[v48]);
                    }
                    sub_44cb00();
                    sub_44cba0();
                }
                if (iter1 >= a0 || (v49 = v2, v50 = (unsigned int)*(v49), iter1 += 1, v2 = v49 + 1, v50 == 0xffffffff))
                    v5 -= 1;
                else
                    (&g_4cc550)[v41] = v50;
                v3 = v3 + 1 & 0x1fff;
                if (v5)
                    v3 = sub_44c960(&v7);
                l -= 1;
            } while (l != 1);
            v22 = v4;
LABEL_44c6bb:
            v24 = v5;
            if (v24 <= NULL)
                break;
            v23 = v7;
        }
    }
    v51 = *((char *)((void*)&v0 + 3));
    v1 = v51 >> 1;
    v52 = _ccall(4, 25, v51 >> 1, v51, 0);
    if (v52 & 1)
    {
        *(iter) = v12;
        iter += 1;
        v12 = 0;
        v1 = 128;
    }
    v53 = 0x1000;
    do
    {
        v54 = v53;
        v55 = v1;
        v1 = v55 >> 1;
        v56 = _ccall(4, 25, v55 >> 1, v55, 0);
        if (v56 & 1)
        {
            *(iter) = v12;
            iter += 1;
            v12 = 0;
            v1 = 128;
        }
    } while ((v53 = v54 >> 1, v54 > 1));
    *(a1) = &iter[-1 * v9];
    return v9;
}
