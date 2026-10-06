// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42c770; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4514;

unsigned int sub_42c770(void* idx)
{
    unsigned int v12;  // ftop
    unsigned int v22;  // ftop
    unsigned int *index;  // edx
    unsigned int j;  // ecx
    void* node;  // eax
    unsigned int v26;  // ftop
    int v27;  // edi
    unsigned int v29;  // ftop
    unsigned int v30;  // 4409
    unsigned int v13;  // edi
    unsigned int v33;  // 4216
    unsigned int *v35;  // ecx
    unsigned int v37;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v14;  // esi
    unsigned int v41;  // ebp
    unsigned int v42;  // ebx
    unsigned int v44;  // 4324
    unsigned int v45;  // eax
    unsigned int v46;  // edx
    unsigned int *v47;  // ecx
    unsigned short v49;  // ftop
    unsigned int v15;  // ebp
    unsigned int v16;  // ebx
    unsigned int iter;  // edi
    unsigned int *v18;  // ecx
    unsigned int v20;  // ftop
    unsigned int v0;  // [bp-0x5c]
    unsigned int v1;  // [bp-0x58]
    unsigned int v2;  // [bp-0x54]
    unsigned int v3;  // [bp-0x50]
    unsigned int v4;  // [bp-0x4c]
    unsigned int v5;  // [bp-0x30]
    unsigned int v6;  // [bp-0x2c]
    unsigned int v7;  // [bp-0x28]
    unsigned int v8;  // [bp-0x24]
    unsigned int v9;  // [bp-0x20]
    unsigned int v10;  // [bp-0x1c]

    do
    {
        (*((int *)*((int *)idx)))(v13, v14, v15, v16);
        if (!(int)idx[1072])
            break;
        iter = 0;
        if ((char)(int)idx[1072] & 1)
            iter = (*((int *)(*((int *)idx) + 40)))();
        if ((char)idx[1072] & 4)
            iter += (*((int *)(*((int *)idx) + 44)))();
        if ((char)idx[1072] & 8)
            iter += (*((int *)(*((int *)idx) + 48)))();
        if ((char)idx[1072] & 16)
            iter += (*((int *)(*((int *)idx) + 52)))();
        if ((char)idx[1072] & 64)
            iter += (*((int *)(*((int *)idx) + 56)))();
        if ((char)idx[1072] & 32)
            iter += (*((int *)(*((int *)idx) + 60)))();
        if ((int)idx[1072] & 0x100)
            iter += (*((int *)(*((int *)idx) + 64)))();
        if ((int)idx[1072] & 0x20000)
            iter += (*((int *)(*((int *)idx) + 0x44)))();
        if ((int)idx[1072] & 0x40000)
            iter += (*((int *)(*((int *)idx) + 72)))();
        if ((int)idx[1072] & 0x800000)
            iter += (*((int *)(*((int *)idx) + 76)))();
        if ((int)idx[1072] & 0x400)
            iter += (*((int *)(*((int *)idx) + 80)))();
        if (!((int)idx[1072] & 0x1000))
            continue;
        if ((int)idx[396] <= 0)
        {
            *((unsigned int *)&idx[1072]) = (int)idx[1072] ^ 0x1000;
            iter += 1;
            continue;
        }
        v18 = (int)idx[404];
        *((int *)&idx[392]) = (int)idx[396];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v20 = v12;
        if (!((char)((((unsigned short)v20 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v20 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v18)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                v22 = v20 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_42c8f5;
            }
        }
        v22 = v20 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
LABEL_42c8f5:
        *((int *)&idx[400]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v12 = v22 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&idx[396]) = sub_4931e0();
        if ((int)idx[1100])
            *((unsigned int *)&idx[1100]) = (int)idx[1100] - 1;
    } while (iter);
    index = (int)idx[3996];
    j = (int)idx[1136] - 1;
    if (j > 0)
    {
        node = &index[5 * j];
        do
        {
            *((int *)node) = *((int *)((char *)node - 20));
            *((int *)&node[4]) = *((int *)((char *)node - 16));
            *((int *)&node[8]) = *((int *)((char *)node - 12));
            *((int *)&node[12]) = *((int *)((char *)node - 8));
            *((int *)&node[16]) = *((int *)((char *)node - 4));
            j -= 1;
            node -= 20;
        } while (j > 0);
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *(index) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index[1] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index[2] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&idx[80]) = *(index);
    *((unsigned int *)&idx[84]) = index[1];
    *((unsigned int *)&idx[88]) = index[2];
    index[3] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index[4] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v26 = v12;
    if ((int)idx[1084] <= 0 && !((int)idx[1072] & 0x400))
    {
        v27 = 0;
        if ((int)idx[1136] <= 0)
            return 1;
        while (1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            /* unsupported instruction */
            v29 = v26 - 1;
            v30 = _ccall(10, 13, (char)((((unsigned short)v29 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (v30 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = v29 + 1;
                if ((((unsigned short)v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                {
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
                    v29 = v26 - 1;
                    v33 = _ccall(10, 13, (char)((((unsigned short)v29 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                    if (!(v33 & 1))
                        goto LABEL_42ca70;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v29 + 1;
                    if ((((unsigned short)v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                        break;
                }
            }
            else
            {
LABEL_42ca70:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = v29 + 1;
            }
            v27 += 1;
            if (v27 >= (int)idx[1136])
                return 1;
        }
    }
    else
    {
        v35 = (int)idx[1092];
        *((int *)&idx[1080]) = (int)idx[1084];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v37 = v26;
        if (!((char)((((unsigned short)v37 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if ((char)((((unsigned short)v37 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v35)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                goto LABEL_42cacd;
            v39 = v37 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
LABEL_42cacd:
            v39 = v37 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        *((int *)&idx[1088]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v26 = v39 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&idx[1084]) = sub_4931e0();
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v40 = v26;
    v41 = 0;
    v42 = 0;
    if ((int)idx[1136] - 1 > 0)
    {
        do
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v44 = _ccall(10, 13, (char)((((unsigned short)v40 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v44 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v45 = sub_437a80((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                if (v45 == 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    (*((int *)(*((int *)idx) + 24)))(g_4b4514 + 2428, &v2, 0, 1);
                }
                else if (v45 == 2 && !v41 && !(int)((int)idx[44] % 3))
                {
                    sub_4391c0(g_4b4514 + 2428);
                    v41 = 1;
                }
            }
        } while ((v42 += 1, v42 < (int)idx[1136] - 1));
    }
    sub_455630(idx + 2792);
    v46 = (int)idx[44];
    v47 = (int)idx[52];
    *((unsigned int *)&idx[40]) = v46;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v49 = v40;
    if (!((char)(((v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v47)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&idx[44]) = v46 + 1;
            *((int *)&idx[48]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            return 0;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx[48]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((unsigned int *)&idx[44]) = sub_4931e0();
    return 0;
}
