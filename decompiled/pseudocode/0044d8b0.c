// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d8b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44d8b0_0 {
    char field_0;
    char field_1;
    char field_2;
    char field_3;
} st_44d8b0_0;

extern unsigned int g_4b0e48;
extern unsigned int g_4b0e4c;
extern unsigned int g_4b0e50;
extern int g_4b0e58;
extern void* g_4b0e68;

char sub_44d8b0(unsigned int a0)
{
    void* iter;  // ebp
    unsigned int v6;  // esi
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    unsigned int v18;  // eax
    void* iter1;  // ebp
    unsigned int iter2;  // ebx
    unsigned int i;  // edi
    unsigned int v22;  // eax
    unsigned int j;  // edx
    void* node;  // ecx
    unsigned int v7;  // edx
    unsigned int v25;  // esi
    st_44d8b0_0 *v26;  // eax
    int v27;  // eax
    char *v28;  // eax
    unsigned int v8;  // edi
    unsigned int v9;  // ebx
    unsigned int v10;  // ecx
    unsigned int v11;  // eax
    unsigned int v12;  // eax
    unsigned int v13;  // eax
    unsigned int v14;  // esi
    unsigned int v0;  // [bp-0x18]
    void* v1;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    if (g_4b0e48 == 21)
    {
        iter1 = g_4b0e68;
        iter2 = 0;
        i = 0;
        v1 = g_4b0e68;
        v0 = 0;
        if (a0 <= 0)
            return 1;
        v22 = g_4b0e4c;
        do
        {
            j = 0;
            v2 = 0;
            if (v22 > 0)
            {
                node = iter1 - 2;
                do
                {
                    if (!(char)node[5])
                    {
                        v25 = 0;
                        v4 = 0;
                        v3 = 0;
                        if (j && (char)node[1])
                        {
                            iter2 = *((char *)node);
                            v3 = *((char *)node - 1);
                            v4 = *((char *)node - 2);
                            v25 = 1;
                        }
                        if (j < v22 - 1 && (char)node[9])
                        {
                            v3 += (char)node[7];
                            iter2 += (char)node[8];
                            v4 += (char)node[6];
                            v25 += 1;
                        }
                        if (i)
                        {
                            v26 = iter1 - ((int)(g_4b0e58 + (g_4b0e58 >> 31 & 3)) >> 2) * 4;
                            if (v26->field_3)
                            {
                                iter1 = v1;
                                iter2 += v26->field_2;
                                v3 += v26->field_1;
                                v4 += v26->field_0;
                                v25 += 1;
                            }
                        }
                        if (v0 < g_4b0e50 - 1)
                        {
                            v27 = (int)(g_4b0e58 + (g_4b0e58 >> 31 & 3)) >> 2;
                            v28 = iter1 + v27 * 4;
                            if (*((char *)iter1 + 4 * v27 + 3))
                            {
                                iter2 += v28[2];
                                v3 += v28[1];
                                v4 += *(v28);
                                v25 += 1;
                            }
                        }
                        if (v25 > 1)
                        {
                            iter2 /= v25;
                            v3 /= v25;
                            v4 /= v25;
                        }
                        i = v0;
                        *((char *)&node[4]) = iter2;
                        *((char *)&node[3]) = v3;
                        j = v2;
                        *((char *)iter1) = v4;
                        v22 = g_4b0e4c;
                        iter2 = 0;
                    }
                    j += 1;
                    iter1 += 4;
                    node += 4;
                    v1 = iter1;
                    v2 = j;
                } while (j < v22);
            }
            i += 1;
            v0 = i;
        } while (i < a0);
        return 1;
    }
    else if (g_4b0e48 == 26)
    {
        iter = g_4b0e68;
        v0 = 0;
        if (a0 <= 0)
            return 1;
        v6 = g_4b0e4c;
        do
        {
            v7 = 0;
            v1 = 0;
            if (v6)
            {
                do
                {
                    if (!(*((short *)iter) & 0xf000))
                    {
                        v8 = 0;
                        v9 = 0;
                        v10 = 0;
                        v4 = 0;
                        if (v7)
                        {
                            v11 = *((short *)((char *)iter - 2));
                            if (v11 & 0xf000)
                            {
                                v12 = (unsigned short)v11;
                                v10 = v12 >> 8 & 15;
                                v9 = v12 >> 4 & 15;
                                v4 = v12 & 15;
                                v8 = 1;
                            }
                        }
                        if (v7 < v6 - 1)
                        {
                            v13 = (short)iter[2];
                            if (v13 & 0xf000)
                            {
                                v14 = (unsigned short)v13;
                                v4 += v14 & 15;
                                v10 += v14 >> 8 & 15;
                                v9 += v14 >> 4 & 15;
                                v8 += 1;
                            }
                        }
                        if (v0 > 0)
                        {
                            v15 = *((short *)((char *)iter + -0x2 * &g_4b0e58));
                            if (v15 & 0xf000)
                            {
                                v16 = (unsigned short)v15;
                                v10 += v16 >> 8 & 15;
                                v4 += v16 & 15;
                                v9 += v16 >> 4 & 15;
                                v8 += 1;
                            }
                        }
                        if (v0 < g_4b0e50 - 1)
                        {
                            v17 = *((short *)((char *)iter + 0x2 * &g_4b0e58));
                            if (v17 & 0xf000)
                            {
                                v18 = (unsigned short)v17;
                                v10 += v18 >> 8 & 15;
                                v4 += v18 & 15;
                                v9 += v18 >> 4 & 15;
                                v8 += 1;
                            }
                        }
                        if (v8 > 1)
                        {
                            v10 /= v8;
                            v9 /= v8;
                            v4 /= v8;
                        }
                        v7 = v1;
                        *((unsigned short *)iter) = ((((char)(v10 >> 1) & 15) * 16 | (char)(v9 >> 1) & 15) * 16 | *((short *)iter) & 61455) & 0xfff0 | (char)(v4 >> 1) & 15;
                        v6 = g_4b0e4c;
                    }
                } while ((v7 += 1, iter += 2, v1 = v7, v7 < v6));
            }
        } while ((v0 += 1, v0 < a0));
        return 1;
    }
    else
    {
        return 1;
    }
}
