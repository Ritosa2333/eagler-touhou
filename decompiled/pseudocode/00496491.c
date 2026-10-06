// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x496491; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_496491(void)
{
    unsigned int v10;  // eax
    unsigned long long *v11;  // ecx
    unsigned int v17;  // 4145
    unsigned int v18;  // fc3210
    unsigned short v19;  // ftop
    unsigned int v20;  // 4145
    unsigned int v21;  // esi
    unsigned int v22;  // edi
    unsigned int v12;  // fc3210
    unsigned long long *v23;  // edi
    unsigned int v24;  // fc3210
    unsigned int v25;  // 4149
    unsigned int v26;  // ecx
    int v27;  // ecx
    unsigned int v28;  // fc3210
    unsigned int v29;  // eax
    unsigned int i;  // eax
    unsigned int v13;  // 4156
    unsigned int v14;  // fc3210
    unsigned int v15;  // 4145
    unsigned int v16;  // fc3210
    unsigned long v0;  // [bp-0x30]
    unsigned int v1;  // [bp-0x2c]
    char *v2;  // [bp-0x28]
    unsigned int v3;  // [bp-0x24]
    int v4;  // [bp-0x18], Other Possible Types: unsigned long
    char v5;  // [bp-0xc], Other Possible Types: int
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp+0x4]
    unsigned long long *v8;  // [bp+0x8]
    unsigned int v9;  // [bp+0xc]

    v6 = v7 & 31;
    if ((char)v7 & 8 && (char)v9 & 1)
    {
        sub_4911aa(1);
    }
    else if ((char)v7 & 4 && (char)v9 & 4)
    {
        sub_4911aa(4);
    }
    else if ((char)v7 & 1 && (char)v9 & 8)
    {
        sub_4911aa(8);
        v10 = v9 & 0xc00;
        if (v9 & 0xc00)
        {
            if (v10 != 0x400)
            {
                if (v10 != 0x800)
                {
                    if (v10 != 0xc00)
                        goto LABEL_496575;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v11 = v8;
                    v12 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v11)) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v13 = _ccall(10, 13, (char)(((v19 & 7) * 0x800 | (unsigned short)v12 & 0x4700) >> 8) & 5, 0, 0);
                    if (v13 & 1)
                        goto LABEL_496571;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v11 = v8;
                    v14 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v11)) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    v15 = _ccall(10, 13, (char)(((v19 & 7) * 0x800 | (unsigned short)v14 & 0x4700) >> 8) & 5, 0, 0);
                    if (!(v15 & 1))
                        goto LABEL_496563;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    goto LABEL_496571;
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v11 = v8;
                v16 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v11)) * 0x100 & 0x4500;
                /* unsupported instruction */
                v17 = _ccall(10, 13, (char)(((v19 & 7) * 0x800 | (unsigned short)v16 & 0x4700) >> 8) & 5, 0, 0);
                if (!(v17 & 1))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
LABEL_49656b:
                    /* unsupported instruction */
                    /* unsupported instruction */
LABEL_496571:
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
            }
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v11 = v8;
            v18 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v11)) * 0x100 & 0x4500;
            /* unsupported instruction */
            v20 = _ccall(10, 13, (char)(((v19 & 7) * 0x800 | (unsigned short)v18 & 0x4700) >> 8) & 5, 0, 0);
            if (v20 & 1)
                goto LABEL_49656b;
LABEL_496563:
            /* unsupported instruction */
            /* unsupported instruction */
        }
        *(v11) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
LABEL_496575:
    }
    else if ((char)v7 & 2 && (char)v9 & 16)
    {
        v21 = 0;
        if ((char)v7 & 16)
            v21 = 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = v22;
        v23 = v8;
        v24 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v23)) * 0x100 & 0x4500;
        /* unsupported instruction */
        v25 = _ccall(10, 13, (char)(((v19 & 7) * 0x800 | (unsigned short)v24 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v25 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v2 = &v5;
            v1 = v26;
            v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_496afd((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&v4) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v27 = v5 - 0x600;
            if (v27 < -0x432)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v21 = 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v28 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((unsigned long long *)&v4)) * 0x100 & 0x4500;
                /* unsupported instruction */
                *((unsigned short *)&(&v4)[6]) = *((unsigned int *)(&v4 + 6)) & 15 | 16;
                if (v27 < -0x3fd)
                {
                    v29 = 0xfffffc03 - v27;
                    do
                    {
                        i = v29;
                        if (*((char *)&v4) & 1 && !v21)
                            v21 = 1;
                        *((unsigned int *)&v4) = *((unsigned int *)&v4) >> 1;
                        if (*((char *)(&v4 + 4)) & 1)
                            v4 |= 0x80000000;
                        *((unsigned int *)&(&v4)[4]) = *((unsigned int *)(&v4 + 4)) >> 1;
                        v29 = i - 1;
                    } while (i != 1);
                }
                if ((char)(((v19 + 1 & 7) * 0x800 | (unsigned short)v28 & 0x4700) >> 8) & 65)
                    goto LABEL_496634;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
LABEL_496634:
            /* unsupported instruction */
            /* unsupported instruction */
            *(v23) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            v21 = 1;
        }
        if (v21)
            sub_4911aa(16);
        v6 &= 0xfffffffd;
    }
    if (!((char)v7 & 16))
        return;
    if (!((char)v9 & 32))
        return;
    sub_4911aa(32);
    return;
}
