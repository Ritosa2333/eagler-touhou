// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4600a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4a33b8;
extern unsigned int g_4a33f4;
extern int g_4a3478;
extern int g_4a34bc;

unsigned int sub_4600a0(unsigned int *a0, unsigned int a1, unsigned int a2, unsigned int a3, void* a4)
{
    unsigned int v24;  // esi
    unsigned int v25;  // edi
    void* v34;  // esi
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int *idx;  // edi
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int i;  // edx
    unsigned int v46;  // eax
    void* v47;  // ecx
    unsigned int v27;  // eax
    unsigned int *index;  // ecx
    unsigned int *idx1;  // ebx
    int v30;  // eax
    int v31;  // ebx
    unsigned int *idx2;  // esi
    int v33;  // eax
    int v0;  // [bp-0x8c]
    unsigned int v1;  // [bp-0x84]
    unsigned int v2;  // [bp-0x80]
    unsigned int v3;  // [bp-0x7c]
    char v4;  // [bp-0x70]
    void* v5;  // [bp-0x6c]
    int v6;  // [bp-0x64]
    int v7;  // [bp-0x60]
    unsigned int v8;  // [bp-0x5c]
    unsigned int v9;  // [bp-0x58]
    unsigned int v10;  // [bp-0x54]
    unsigned int v11;  // [bp-0x50]
    unsigned int v12;  // [bp-0x4c]
    unsigned int v13;  // [bp-0x48]
    unsigned int v14;  // [bp-0x44]
    unsigned int v15;  // [bp-0x40]
    unsigned int v16;  // [bp-0x3c]
    unsigned int v17;  // [bp-0x20]
    unsigned int v18;  // [bp-0x1c]
    unsigned int v19;  // [bp-0x10]
    unsigned int v20;  // [bp-0xc]
    unsigned int v21;  // [bp-0x8]
    unsigned int v22;  // [bp-0x4]
    unsigned int *v23;  // [bp+0x0]

    v3 = v24;
    v2 = v25;
    if (!a4)
    {
        v1 = &g_4a33b8;
    }
    else if (*((int *)a4) != 7)
    {
        v1 = &g_4a33f4;
    }
    else if (!(char)a4[32])
    {
        v5 = (int)a4[16] + a4;
        if (*((char *)v5) != 64)
        {
            idx1 = a0;
            a4 = a1 * 20;
            v30 = sub_45f880((short)a4[12], (short)a4[22]);
            if (v30 < NULL)
            {
                sub_464300(&g_4a3478, v31);
                return 0xffffffff;
            }
            idx1[75] = idx1[75] + v30;
            idx = idx1;
            goto LABEL_460237;
        }
        else if ((char)v5[1] == 82)
        {
            idx = a0;
            a2 = a1 * 20;
            a4 = a2;
            sub_45fbf0();
            goto LABEL_46023e;
        }
        else
        {
            a4 = a1 * 20;
            v27 = sub_45fba0((short)a4[10]);
            index = v23;
            index[75] = index[75] + v27;
            idx = index;
            goto LABEL_460237;
        }
    }
    else
    {
        idx2 = a0;
        a4 = a1 * 20;
        v33 = sub_45fa80((short)a4[10], (short)a4[12]);
        if (v33 < NULL)
        {
            v0 = &g_4a34bc;
        }
        else
        {
            idx2[75] = idx2[75] + v33;
            idx = idx2;
LABEL_460237:
LABEL_46023e:
            (*((int *)(*((int *)*((int *)(idx[72] + a2))) + 0x44)))(*((int *)(idx[72] + a2)), 0, &v4);
            v34 = a4 + 64;
            v19 = 0;
            if (0 < (short)a4[4])
            {
                while (1)
                {
                    v36 = v35 - 1;
                    if (/* unsupported instruction */)
                    {
                        v37 = v36 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v37 = v36 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    v10 = idx[72] + a2;
                    v8 = *(idx);
                    v9 = v20;
                    if (v6 < NULL)
                    {
                        if (/* unsupported instruction */)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                    }
                    if (/* unsupported instruction */)
                    {
                        v2 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v38 = v37 + 1;
                    }
                    else
                    {
                        v2 = nan;
                        /* unsupported instruction */
                        v38 = v37 + 1;
                    }
                    v39 = v38 - 1;
                    if (/* unsupported instruction */)
                    {
                        v40 = v39 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v40 = v39 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    v2 = (short)a4[10];
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v41 = v40 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if (v7 < NULL)
                    {
                        if (/* unsupported instruction */)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                    }
                    if (/* unsupported instruction */)
                    {
                        v2 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v42 = v41 + 1;
                    }
                    else
                    {
                        v2 = nan;
                        /* unsupported instruction */
                        v42 = v41 + 1;
                    }
                    v43 = v42 - 1;
                    if (/* unsupported instruction */)
                    {
                        v44 = v43 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v44 = v43 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    v2 = (short)a4[12];
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v35 = v44 + 2;
                    sub_460610();
                    v21 += 1;
                    v34 += 4;
                    v19 += 1;
                    if (v19 >= (short)a4[4])
                        break;
                    a2 = v23;
                }
            }
            i = 0;
            if (0 >= (short)a4[6])
                return 1;
            v46 = v22 * 4;
            v47 = v34 + 4;
            do
            {
                *((void* *)(v46 + idx[71])) = *((int *)v47) + a4;
                i += 1;
                v46 += 4;
                v47 += 8;
            } while (i < (short)a4[6]);
            return 1;
        }
    }
    sub_464300(v0);
    return 0xffffffff;
}
