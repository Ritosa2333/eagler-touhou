// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x469d40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_469d40_0 {
    char padding_0[1144];
    void* field_478;
} st_469d40_0;

unsigned int sub_469d40(st_469d40_0 *a0)
{
    unsigned int v18;  // ebx
    unsigned int v19;  // esi
    unsigned int v28;  // ftop
    unsigned int *idx;  // ecx
    unsigned int v30;  // edx
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int *v35;  // eax
    int j;  // ebx
    unsigned int v37;  // ftop
    unsigned int v20;  // edi
    unsigned int idx1;  // eax
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // 4157
    unsigned int v45;  // ftop
    unsigned int v47;  // 4200
    void* idx2;  // edi
    unsigned int node;  // ebp
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v52;  // ftop
    unsigned int v53;  // 4132
    unsigned int v54;  // ecx
    unsigned int v55;  // edx
    unsigned int v56;  // edx
    unsigned int *v57;  // ecx
    void* iter;  // esi
    unsigned short v59;  // ftop
    unsigned int *v61;  // eax
    int i;  // ecx
    unsigned int iter1;  // eax
    unsigned int index;  // ebx
    unsigned int v26;  // ecx
    unsigned int v27;  // ftop
    unsigned int v0;  // [bp-0x6c]
    unsigned int *v1;  // [bp-0x60]
    unsigned int v2;  // [bp-0x5c]
    unsigned int v3;  // [bp-0x54]
    unsigned int v4;  // [bp-0x50]
    unsigned int v5;  // [bp-0x4c]
    unsigned int v6;  // [bp-0x48]
    unsigned int v7;  // [bp-0x40]
    unsigned int v8;  // [bp-0x34]
    int v9;  // [bp-0x30], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x2c]
    unsigned int v11;  // [bp-0x28]
    char *v12;  // [bp-0x24], Other Possible Types: unsigned int
    unsigned int v13;  // [bp-0x20]
    unsigned int v14;  // [bp-0x1c]
    unsigned int v15;  // [bp-0x14]
    char v16;  // [bp-0x4]

    v13 = v18;
    v12 = &v16;
    v11 = v19;
    v10 = v20;
    idx2 = a0->field_478;
    if ((int)idx2[1016] < 30000)
    {
        iter = idx2;
        i = 11;
        iter1 = idx2 + 804;
        do
        {
            *((int *)iter1) = *((int *)(iter1 - 12));
            *((int *)(iter1 + 4)) = *((int *)(iter1 - 8));
            *((int *)(iter1 + 8)) = *((int *)(iter1 - 4));
            i -= 1;
            iter1 -= 12;
        } while (i > NULL);
        index = idx2 + 816;
        if (!((char)idx2[864] & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_465390((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)(index + 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = i;
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)(index + 32)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v8 = v26;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)(index + 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v28 = v27 + 2;
        }
        sub_464db0();
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx = *((int *)(index + 4));
        v30 = *((int *)(index + 8));
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((int *)&idx2[672]) = *((int *)index);
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int **)&idx2[676]) = idx;
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&idx2[680]) = v30;
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v32 = v28;
        if (!((((unsigned short)v32 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x42200000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_46a520((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = v26;
            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&idx2[844]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v32 += 2;
        }
        v33 = v32 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)idx2[808]) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            return 1;
        }
        v11 = -((int)idx2[1016]);
        /* unsupported instruction */
        /* unsupported instruction */
        v35 = (int)idx2[1044];
        j = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v37 = v33;
        if (v35)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v35[6] = (int)idx2[696];
            idx1 = v35 + 6;
            *((int *)(idx1 + 4)) = (int)idx2[700];
            *((int *)(idx1 + 8)) = (int)idx2[704];
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
            v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4937aa();
            v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((int)idx2[1044] + 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx = (int)idx2[1044];
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v40 = v37 - 0;
            v37 = v40 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v40 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx[7]) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                idx[28] = idx[28] & 0xfffffffe;
                *((unsigned int **)&idx2[1044]) = NULL;
            }
        }
        v41 = v37 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v43 = v41 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v44 = _ccall(10, 13, (char)((((unsigned short)v41 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (v44 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v45 = v43 + 1;
        }
        else
        {
            while (1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v43 -= 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v47 = _ccall(10, 13, (char)((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v47 & 1)
                    break;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v45 = v43 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        node = idx2 + 28;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v49 = v45 + 2;
        do
        {
            if (j > NULL)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4937aa();
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v0 = v26;
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            if (j < 11)
            {
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
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4937aa();
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = v26;
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v50 = v49;
                if (!j)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_465280((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v5 = v26;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v4 = v26;
                    v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_42e770((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    v2 = v26;
                    /* unsupported instruction */
                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    /* unsupported instruction */
                    v52 = v50 + 4;
                    v53 = _ccall(10, 13, (char)((((unsigned short)v52 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                    if (v53 & 1)
                        continue;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v50 = v52 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v50 = v49 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v1 = idx;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v52 = v50 + 2;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)iter) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v54 = (int)iter[4];
            /* unsupported instruction */
            /* unsupported instruction */
            v55 = (int)iter[8];
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)node) = *((int *)iter);
            *((unsigned int *)(node + 4)) = v54;
            /* unsupported instruction */
            *((unsigned int *)(node + 8)) = v55;
            sub_46a4a0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            j += 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            iter += 56;
            node += 56;
            *((int *)((char *)iter - 56)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 52)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 32)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 24)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            *((int *)((char *)iter - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v49 = v52;
        } while (j < 12);
        v56 = (int)idx2[1016];
        v57 = (int)idx2[0x400];
        *((unsigned int *)&idx2[1012]) = v56;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v59 = v49;
        if (!((char)(((v59 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)(((v59 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v57)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&idx2[1016]) = v56 + 1;
                *((int *)&idx2[0x3fc]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                return 0;
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx2[0x3fc]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&idx2[1016]) = sub_4931e0();
        return 0;
    }
    else
    {
        v61 = (int)idx2[1044];
        if (!v61)
            return 1;
        v61[28] = v61[28] & 0xfffffffe;
        *((unsigned int **)&idx2[1044]) = NULL;
        return 1;
    }
}
