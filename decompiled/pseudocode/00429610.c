// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x429610; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4514;

unsigned int sub_429610(void* idx)
{
    unsigned int v11;  // ftop
    unsigned int *v26;  // ecx
    unsigned int v28;  // ftop
    unsigned int v12;  // edi
    unsigned int v30;  // ftop
    unsigned int v33;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // 4250
    unsigned int v37;  // ftop
    unsigned int v38;  // fc3210
    unsigned int v13;  // esi
    unsigned int v39;  // 4501
    unsigned int v41;  // 4182
    unsigned int v43;  // fc3210
    unsigned int v44;  // ftop
    unsigned int v45;  // 4146
    unsigned int v46;  // fc3210
    unsigned int v47;  // 4146
    unsigned int iter;  // esi
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // eax
    void* index;  // eax
    unsigned int v54;  // 4144
    unsigned int v16;  // ecx
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x38]
    unsigned int v3;  // [bp-0x34]
    unsigned int v4;  // [bp-0x30]
    unsigned int v5;  // [bp-0x24]
    unsigned int v6;  // [bp-0x20]
    unsigned int v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0x18]
    unsigned int v9;  // [bp-0x14]

    do
    {
        (*((int *)*((int *)idx)))(v12, v13);
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
        if (!((int)idx[1072] & 0x1000))
            continue;
        if ((int)idx[396] <= 0)
        {
            *((unsigned int *)&idx[1072]) = (int)idx[1072] ^ 0x1000;
            iter += 1;
            continue;
        }
        v26 = (int)idx[404];
        *((int *)&idx[392]) = (int)idx[396];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v28 = v11;
        if (!((char)((((unsigned short)v28 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v28 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v26)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                v30 = v28 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_429783;
            }
        }
        v30 = v28 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
LABEL_429783:
        *((int *)&idx[400]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v11 = v30 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&idx[396]) = sub_4931e0();
        if ((int)idx[1100])
            *((unsigned int *)&idx[1100]) = (int)idx[1100] - 1;
    } while (iter);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v33 = v11 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)((((unsigned short)v11 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[108]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v35 = v33 + 1;
        v36 = _ccall(10, 13, (char)((((unsigned short)v35 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v36 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx[108]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[120]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[80]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[84]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v37 = v33;
        v38 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)idx[1132]) * 0x100 & 0x4500;
        v39 = _ccall(10, 13, (char)((((unsigned short)v37 & 7) * 0x800 | (unsigned short)v38 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v39 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v41 = _ccall(10, 13, (char)((((unsigned short)v37 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (v41 & 1)
                goto LABEL_4298a2;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx[108]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            *((int *)&idx[1124]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            v35 = v37 + 1;
            if (!((((unsigned short)v35 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)idx[108]) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                return 1;
        }
        else
        {
LABEL_4298a2:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v35 = v37 + 1;
        }
    }
    if ((int)idx[1084] <= 0)
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
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
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
        /* unsupported instruction */
        /* unsupported instruction */
        if (sub_42e820((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (sub_42e820((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)))
                return 1;
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = v16;
        /* unsupported instruction */
        sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v43 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)idx[108]) * 0x100 & 0x4500;
    /* unsupported instruction */
    v44 = v35;
    v45 = _ccall(10, 13, (char)((((unsigned short)v44 & 7) * 0x800 | (unsigned short)v43 & 0x4700) >> 8) & 5, 0, 0);
    if (!(v45 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v46 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)idx[112]) * 0x100 & 0x4500;
        /* unsupported instruction */
        v47 = _ccall(10, 13, (char)((((unsigned short)v44 & 7) * 0x800 | (unsigned short)v46 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v47 & 1))
        {
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
            /* unsupported instruction */
            /* unsupported instruction */
            sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
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
            v49 = v44;
            if (!((char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)idx[112]) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                v50 = v49 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v50 = v49 - 1;
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
            }
            /* unsupported instruction */
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
            v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v44 = v50 + 1;
            v51 = sub_437a80((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            if (v51 == 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                (*((int *)(*((int *)idx) + 24)))(g_4b4514 + 2428, &v1, 0, 1);
            }
            else if (v51 == 2)
            {
                if (!(int)((int)idx[44] % 3))
                    sub_4391c0(g_4b4514 + 2428);
                sub_464a80();
            }
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index = idx + 1596;
    *((unsigned int *)&index[1148]) = (int)index[1148] | 8;
    *((int *)&index[64]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&index[1148]) = (int)index[1148] | 8;
    *((int *)&index[0x44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    sub_455630(index);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v54 = _ccall(10, 13, (char)((((unsigned short)v44 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)idx[120]) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
    if (!(v54 & 1))
        sub_455630(idx + 2800);
    sub_464a80();
    return 0;
}
