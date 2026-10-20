// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x436ba0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_436ba0_0 {
    char padding_0[27928];
    char field_6d18;
    char padding_6d19[23];
    unsigned int field_6d30;
} st_436ba0_0;

typedef struct st_436ba0_2 {
    char padding_0[60];
    unsigned int field_3c;
} st_436ba0_2;

typedef struct st_436ba0_3 {
    char padding_0[16];
    unsigned int field_10;
} st_436ba0_3;

typedef struct st_436ba0_1 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[4];
    unsigned int field_18;
    char padding_1c[84];
    unsigned int field_70;
} st_436ba0_1;

extern int g_4b0c98;
extern int g_4b0ca0;
extern int g_4b0ccc;
extern int g_4b0cd4;
extern unsigned int g_4b2ed0;
extern st_436ba0_2 *g_4b43c4;
extern st_436ba0_1 *g_4b43dc;
extern st_436ba0_0 *g_4b43e4;
extern st_436ba0_3 *g_4b4518;
extern char g_4d49d0;

unsigned int sub_436ba0(unsigned int a0, unsigned int a1, void* a2)
{
    unsigned int v28;  // ebx
    unsigned int v29;  // esi
    int idx;  // esi
    void* idx1;  // esi
    unsigned int v39;  // edx
    unsigned int v40;  // eax
    unsigned int v41;  // ecx
    unsigned int v42;  // ftop
    void* iter;  // esi
    unsigned int v44;  // ftop
    unsigned int *v45;  // ecx
    unsigned int v30;  // edi
    unsigned int v47;  // ftop
    unsigned int v49;  // ftop
    int v50;  // eax
    unsigned int *v51;  // ecx
    unsigned int v53;  // ftop
    unsigned int v55;  // ftop
    unsigned int v56;  // edx
    void* idx2;  // edi
    unsigned int *v57;  // ecx
    unsigned int v58;  // ftop
    unsigned int v60;  // ftop
    unsigned int v61;  // 4767
    unsigned int v63;  // edx
    unsigned int *v64;  // ecx
    unsigned int v65;  // fc3210
    unsigned short v66;  // ftop
    unsigned int v32;  // ecx
    unsigned int v67;  // fc3210
    unsigned int v68;  // eax
    int v33;  // edx
    unsigned int v34;  // eax
    unsigned int v35;  // ecx
    unsigned int node;  // ftop
    unsigned int v0;  // [bp-0x94]
    unsigned int v1;  // [bp-0x90]
    unsigned int v2;  // [bp-0x84]
    void* iter1;  // [bp-0x7c]
    unsigned int i;  // [bp-0x78]
    void* v5;  // [bp-0x74], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x70]
    unsigned int v7;  // [bp-0x6c]
    unsigned int v8;  // [bp-0x68]
    int v9;  // [bp-0x64], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x60]
    unsigned int v11;  // [bp-0x54]
    unsigned int v12;  // [bp-0x50]
    unsigned int v13;  // [bp-0x4c]
    unsigned int v14;  // [bp-0x48]
    unsigned int v15;  // [bp-0x44]
    unsigned int v16;  // [bp-0x40]
    char v17;  // [bp-0x34]
    unsigned int v18;  // [bp-0x30]
    unsigned int v19;  // [bp-0x2c]
    unsigned int v20;  // [bp-0x28]
    unsigned int v21;  // [bp-0x24]
    unsigned int v22;  // [bp-0x20]
    unsigned int v23;  // [bp-0x1c]
    unsigned int v24;  // [bp-0x18]
    unsigned int v25;  // [bp-0x14]
    unsigned int v26;  // [bp-0x10]
    unsigned int v27;  // [bp-0xc]

    v13 = v28;
    v12 = v29;
    v11 = v30;
    idx2 = a2;
    switch ((int)idx2[2600])
    {
    case 2:
        goto LABEL_436dd9;
    case 3:
        if ((int)idx2[2612] != 4 && (int)idx2[2612] == 15)
        {
            sub_40d230();
            sub_428750();
LABEL_436f8d:
            idx2 = a2;
            break;
        }
        break;
    case 4:
        if ((int)idx2[2612] >= 8)
        {
            sub_4381e0();
            g_4b43dc->field_10 = g_4b43dc->field_10 + 1;
            g_4b43dc->field_18 = 0;
LABEL_436dd9:
            if ((int)idx2[2612] == 3)
            {
                sub_439440(g_4b0cd4);
                /* unsupported instruction */
                /* unsupported instruction */
                v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_437730(g_4b0cd4);
                idx = 0;
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                node += 1;
                v21 = 1;
                v22 = 1;
                v23 = 1;
                v24 = 1;
                v25 = 1;
                v26 = 1;
                v27 = 1;
                v15 = 0;
                do
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
                    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    i = *((int *)&(&v17)[4 * idx]);
                    sub_4273f0(*((int *)&(&v17)[4 * idx]), idx2 + 2428, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    idx += 1;
                    v9 = idx;
                } while (idx < 7);
                sub_4385b0(idx2);
                iter1 = idx2;
            }
            if ((int)idx2[2612] >= 30)
            {
                if (g_4b0c98 < 0)
                {
                    if (g_4b4518->field_10 != 1)
                    {
                        sub_433710();
                        break;
                    }
                    else
                    {
                        sub_432850();
                        break;
                    }
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v2 = 30;
                    *((unsigned int *)&idx2[2600]) = 0;
                    g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx1 = idx2 + 2428;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4390f0(idx1, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), 30, 150);
                    if (g_4b0ca0 < 2)
                        sub_422f60();
                    v39 = *((int *)idx1);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v40 = (int)idx1[4];
                    v41 = (int)idx1[8];
                    *((int *)idx1) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&idx2[2568]) = v39;
                    *((unsigned int *)&idx2[2572]) = v40;
                    *((int *)&idx2[2432]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    *((unsigned int *)&idx2[2576]) = v41;
                    *((unsigned int *)&idx2[2440]) = 0;
                    *((unsigned int *)&idx2[2444]) = 0xf000;
                    sub_4067e0(280);
                    sub_4067e0(0);
                    break;
                }
            }
        }
        else if (g_4b43c4 && !g_4b43c4->field_3c && g_4b0ca0 && g_4d49d0 & 2)
        {
            sub_4067e0(60);
            sub_406bf0(60);
            sub_422f20();
            sub_4385b0(idx2);
            *((unsigned int *)&idx2[2600]) = 1;
            break;
        }
    case 0:
        v32 = (int)idx2[2612] * 0x2800;
        v33 = (int)((unsigned int)(2290649225 * v32 >> 32) + v32) >> 5;
        v16 = 0xf000 - (((unsigned int)(v33) >> 31) + v33);
        *((unsigned int *)&idx2[2444]) = v16;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx2[2432]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_412610();
        v34 = (int)idx2[2612];
        *((unsigned int *)&idx2[2592]) = 0;
        *((unsigned int *)&idx2[2596]) = 0;
        v16 = v34;
        if (v16 >= 30)
        {
            sub_40d230();
            sub_428750();
            idx2 = a2;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v10 = 0;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_40caa0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0, 1);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = v35;
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_40caa0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0, 0);
            /* unsupported instruction */
            /* unsupported instruction */
            i = v35;
            i = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_4286f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0, 1);
            /* unsupported instruction */
            /* unsupported instruction */
            iter1 = NULL;
            v2 = v35;
            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_4286f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0, 0);
        }
        if ((int)idx2[2612] < 60)
            break;
        *((unsigned int *)&idx2[2600]) = 1;
        sub_4067e0(0);
    case 1:
        if (g_4b43e4 && !g_4b43e4->field_6d30 && g_4b43dc && g_4b43dc->field_70 && g_4b43c4 && !g_4b43c4->field_3c && g_4b0ca0 && g_4d49d0 & 2)
        {
            sub_406bf0();
            sub_422f20();
            sub_4385b0(idx2);
        }
        if ((int)idx2[2612] < 30)
        {
            sub_40d230();
            sub_428750();
        }
        sub_4364f0();
        goto LABEL_436f8d;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx2[33372]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v5 = idx2 + 35208;
    v42 = node - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    iter = idx2 + 35244;
    v6 = 128;
    do
    {
        if (!(1 & (char)iter[76]))
            continue;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v44 = v42 + 1;
        if (!((char)iter[36] & 1))
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
            *((int *)&iter[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = a0;
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter[20]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v0 = v35;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&iter[16]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v44 += 2;
        }
        sub_464db0();
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)v2) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)iter - 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v45 = (int)iter[52];
        *((int *)&iter[40]) = (int)iter[44];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v47 = v44;
        if (!((char)((((unsigned short)v47 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v47 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v45)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v49 = v47 - 2;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_43706f;
            }
        }
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
        iter1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v49 = v47 - 2;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
LABEL_43706f:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&iter[48]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v42 = v49 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        v50 = sub_4931e0();
        *((int *)&iter[44]) = v50;
        if (v50 <= 0)
            *((unsigned int *)&iter[76]) = (int)iter[76] & 0xfffffffe;
        iter1 += 116;
        iter += 116;
        i -= 1;
    } while (i != 1);
    if ((int)idx2[50180] > 0)
    {
        v51 = (int)idx2[50188];
        *((int *)&idx2[0xc400]) = (int)idx2[50180];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v53 = v42;
        if (!((char)((((unsigned short)v53 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if ((char)((((unsigned short)v53 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v51)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                goto LABEL_4370e3;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
LABEL_4370e3:
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
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        *((int *)&idx2[50184]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v55 = v53 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&idx2[50180]) = sub_4931e0();
        if ((int)idx2[2612] == (int)idx2[2608] || (int)((int)idx2[2612] % 3))
            goto LABEL_437140;
        *((unsigned int *)&idx2[1168]) = (int)idx2[1168] | 0x10000;
        *((unsigned int *)&idx2[980]) = 0xff0000ff;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v55 = v42 + 1;
LABEL_437140:
        *((unsigned int *)&idx2[1168]) = (int)idx2[1168] & 0xfffeffff;
    }
    sub_455630(idx2 + 20);
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
    *((int *)&idx2[2508]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((int *)&idx2[2512]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx2[2516]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    *((int *)&idx2[2520]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((int *)&idx2[2524]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx2[2528]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    *((int *)&idx2[50244]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx2[50248]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx2[50252]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
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
    *((int *)&idx2[50256]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx2[50260]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx2[50264]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx2[50268]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((int *)&idx2[50272]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx2[50276]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx2[50280]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((int *)&idx2[50284]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx2[50288]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx2[50292]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((int *)&idx2[50296]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx2[50300]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx2[50304]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((int *)&idx2[50308]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx2[50312]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v56 = (int)idx2[2612];
    v57 = (int)idx2[2620];
    *((unsigned int *)&idx2[2608]) = v56;
    /* unsupported instruction */
    /* unsupported instruction */
    v58 = v55 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v60 = v58 + 1;
    v61 = _ccall(10, 13, (char)((((unsigned short)v58 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
    if (!(v61 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)v60 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v57)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
            goto LABEL_437454;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&idx2[2612]) = v56 + 1;
        *((int *)&idx2[2616]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else
    {
LABEL_437454:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        i = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v60 -= 1;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx2[2616]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&idx2[2612]) = sub_4931e0();
    }
    v63 = (int)idx2[2632];
    v64 = (int)idx2[2640];
    *((unsigned int *)&idx2[2628]) = v63;
    /* unsupported instruction */
    /* unsupported instruction */
    v65 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
    /* unsupported instruction */
    /* unsupported instruction */
    v66 = (unsigned short)v60 + 1;
    if (!((char)(((v66 & 7) * 0x800 | (unsigned short)v65 & 0x4700) >> 8) & 65) && !(/* unsupported instruction */
, /* unsupported instruction */
, v67 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (unsigned long long)*(v64)) * 0x100 & 0x4500, /* unsupported instruction */
, (char)(((v66 & 7) * 0x800 | (unsigned short)v67 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&idx2[2632]) = v63 + 1;
        *((int *)&idx2[2636]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        i = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx2[2636]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&idx2[2632]) = sub_4931e0();
    }
    if (!g_4b43e4->field_6d30 && g_4b43dc && g_4b43dc->field_70 && !(int)((int)idx2[2612] % 60))
    {
        g_4b0ccc = g_4b0ccc + 1;
        if (g_4b0ccc > 0x400)
        {
            g_4b0ccc = 0x400;
        }
        else if (g_4b0ccc < -0x400)
        {
            g_4b0ccc = 0xfffffc00;
        }
    }
    if (!g_4b43e4->field_6d30 && g_4b43dc && g_4b43dc->field_70 && !(g_4b43e4->field_6d18 & 16))
    {
        sub_439a40(idx2);
        sub_439b10(idx2);
        return 1;
    }
    v68 = (int)idx2[50224];
    if (!((char)(int)idx2[50224] & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx2[50216]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&idx2[50212]) = 0;
        *((unsigned int *)&idx2[50208]) = 0xfff0bdc1;
        *((unsigned int **)&idx2[50220]) = &g_4b2ed0;
        *((unsigned int *)&idx2[50224]) = v68 | 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&idx2[50212]) = 0xffffffff;
    *((int *)&idx2[50216]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)&idx2[50208]) = 0xfffffffe;
    *((unsigned int *)&idx2[35200]) = 0;
    *((char *)&idx2[35204]) = 0;
    sub_439b10(idx2);
    return 1;
}
