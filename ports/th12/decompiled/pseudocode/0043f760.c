// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f760; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f760_1 {
    char padding_0[28];
    struct st_43f760_2 *field_1c;
    char padding_20[152];
    unsigned int field_b8;
} st_43f760_1;

typedef struct st_43f760_2 {
    char padding_0[92];
    unsigned int field_5c;
    unsigned int field_60;
    unsigned int field_64;
} st_43f760_2;

typedef struct st_43f760_3 {
    char padding_0[125402];
    char field_1e9da;
} st_43f760_3;

extern char g_4aebf0;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cac;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern void g_4b0ce0;
extern int g_4b0ce4;
extern unsigned int g_4b0ce8;
extern void g_4b33a4;
extern int g_4b43e8;
extern st_43f760_3 *g_4b451c;
extern char g_4b452c;
extern unsigned int g_4ce8b0;
extern char g_4ceae8;
extern unsigned int g_4cee40;
extern unsigned int g_4cee78;
extern unsigned short g_4d48b8;

unsigned int sub_43f760(void* idx)
{
    unsigned int v2;  // esi
    void* v3;  // eax
    int v12;  // edi
    unsigned int v13;  // eax
    void* v14;  // eax
    int v15;  // ecx
    unsigned int v16;  // 4101
    int v17;  // eax
    void* v18;  // eax
    int v19;  // ecx
    unsigned int v20;  // edx
    unsigned int *v21;  // ecx
    void* iter;  // eax
    unsigned short v23;  // ftop
    unsigned short v24;  // ftop
    unsigned short v25;  // ftop
    char i;  // 4104
    st_43f760_1 *v6;  // esi
    unsigned int v7;  // ecx
    int v8;  // edx
    int v9;  // eax
    st_43f760_1 *v10;  // ecx
    unsigned int *index;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    if ((int)idx[28] == 1)
    {
        g_4b0ce4 = g_4b0ce4 + 1;
        if (!g_4d48b8)
        {
            if (g_4b0ce4 < 1800)
                goto LABEL_43f882;
            v3 = *((int *)&(&g_4b33a4)[4 * g_4b0ce8]);
            *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) & 0xffffffbf | 32;
            iter = v3;
            do
            {
                i = *((char *)iter);
                *(&g_4b43e8 - v3 + (char *)iter) = *((char *)iter);
                iter += 1;
            } while (i);
            v6 = sub_43b6f0(&g_4b43e8);
            v7 = g_4b0ce8 + 1;
            v8 = (int)((unsigned int)(0x55555555 * v7 >> 32) - v7) >> 1;
            g_4b0ce8 = v7 + ((unsigned int)(v8) >> 31) + v8 + (((unsigned int)(v8) >> 31) + v8) * 2;
            v9 = 0;
            v10 = &v6->field_b8;
            do
            {
            } while (!*((int *)&v10->padding_0[0]) && (v9 = (int)(v9 + 1), v10 += 36, v9 < 8));
            g_4b0cb0 = v9;
            g_4b0cb4 = v9;
            g_4cee40 = 13;
            index = v6->field_1c;
            g_4b0c90 = index[23];
            *((char **)&g_4b452c) = &(&g_4aebf0)[64 * v9];
            g_4b0c94 = index[24];
            g_4b0cac = g_4b0ca8;
            g_4b0ca8 = index[25];
            sub_43b450(v6);
            sub_46ca4f(v6);
            g_4ce8b0 = 1;
        }
        g_4b0ce4 = 0;
    }
LABEL_43f882:
    if ((char)idx[23576] & 1)
    {
        *((unsigned int *)&idx[23572]) = (int)idx[23572] + 1;
        if ((int)idx[23572] >= 5)
        {
            sub_4300d0(0, "bgm/th12_01.wav", v12);
            if (g_4ceae8 & 16)
                sub_454960(4, 0);
            sub_454960(2, 0);
            g_4b451c->field_1e9da = 1;
            *((unsigned int *)&idx[23576]) = (int)idx[23576] & 0xfffffffe;
            *((int *)&idx[23572]) = 0;
        }
    }
    switch ((int)idx[28])
    {
    case 0:
        sub_461be0();
        sub_461be0();
        sub_461be0();
        sub_461be0();
        sub_411b00();
        v13 = g_4ce8b0;
        if (v13 == 3)
        {
            v14 = idx + 40;
            *((int *)&idx[48]) = 10;
            v15 = (int)v14[8];
            if (v15)
            {
                v16 = _ccall(14, 15, v15, 0, 0);
                if (v16 & 1)
                    *((unsigned int *)v14) = v15 - 1;
                else
                    *((unsigned int *)v14) = 0;
            }
            else
            {
                *((unsigned int *)v14) = 0;
            }
            sub_464900();
            sub_43eee0();
            *((unsigned int *)((int)idx[16] + 4)) = *((int *)((int)idx[16] + 4)) | 2;
            g_4ce8b0 = 1;
            goto LABEL_43f98f;
        }
        if (!(g_4b0ce0 & 32))
        {
            *((unsigned int *)&idx[23576]) = (int)idx[23576] | 1;
            *((int *)&idx[23572]) = 0;
        }
        else
        {
            *((unsigned int *)&idx[23576]) = (int)idx[23576] & 0xfffffffe;
            g_4b0ca8 = g_4b0cac;
        }
        *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) & 0xffffffdf;
        if (!v13)
        {
            *((unsigned int *)&idx[23576]) = (int)idx[23576] | 2;
            sub_43eee0();
            g_4ce8b0 = 1;
            goto LABEL_43f9ef;
        }
        *((unsigned int *)&idx[23576]) = (int)idx[23576] & 0xfffffffd;
        if (v13 == 1)
        {
            if (g_4b0ca8 == 4)
            {
                v17 = (int)idx[48];
                if (v17)
                {
                    if (v17 <= 1)
                    {
                        *((unsigned int *)&idx[40]) = v17 - 1;
                        sub_43eee0();
                        *((unsigned int *)((int)idx[16] + 4)) = *((int *)((int)idx[16] + 4)) | 2;
                        sub_43fde0(idx);
                        break;
                    }
                    else
                    {
                        *((unsigned int *)&idx[40]) = 1;
                        sub_43eee0();
                        *((unsigned int *)((int)idx[16] + 4)) = *((int *)((int)idx[16] + 4)) | 2;
                        sub_43fde0(idx);
                        break;
                    }
                }
                else
                {
                    *((unsigned int *)&idx[40]) = 1;
                }
            }
            sub_43eee0();
            *((unsigned int *)((int)idx[16] + 4)) = *((int *)((int)idx[16] + 4)) | 2;
            sub_43fde0(idx);
            break;
        }
        else if (v13 == 2)
        {
            v18 = idx + 40;
            *((int *)&idx[48]) = 10;
            v19 = (int)v18[8];
            if (!v19)
            {
                *((unsigned int *)v18) = 3;
            }
            else if (v19 <= 3)
            {
                *((unsigned int *)v18) = v19 - 1;
            }
            else
            {
                *((unsigned int *)v18) = 3;
            }
            sub_464900();
            sub_43eee0();
            *((unsigned int *)((int)idx[16] + 4)) = *((int *)((int)idx[16] + 4)) | 2;
            g_4ce8b0 = 1;
            goto LABEL_43fad2;
        }
    case 1:
LABEL_43f9ef:
        *((unsigned int *)((int)idx[16] + 4)) = *((int *)((int)idx[16] + 4)) | 2;
        sub_43fde0(idx);
        break;
    case 3:
        sub_4411d0();
        break;
    case 4:
        sub_4435b0();
        break;
    case 5:
        sub_444c80(idx);
        break;
    case 6:
        sub_4452d0(idx);
        break;
    case 7:
        sub_445a40(idx);
        break;
    case 8:
        sub_445fc0();
        break;
    case 10:
        sub_4470c0(idx);
        break;
    case 11:
LABEL_43fad2:
        sub_4466a0(idx);
        break;
    case 13:
        sub_449360(idx);
        break;
    case 14:
LABEL_43f98f:
        sub_4480c0();
        break;
    case 15:
        sub_4489e0();
    case 2:
        g_4cee40 = ~(g_4cee78 >> 13) & 1 | 2;
    case 9: case 12:
        sub_430240();
        break;
    }
    v20 = (int)idx[696];
    v21 = (int)idx[704];
    *((unsigned int *)&idx[692]) = v20;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)(((v23 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        v24 = v23 - 1;
        if (/* unsupported instruction */)
        {
            v25 = v24 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v25 = v24 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        if (!((char)(((v25 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v21)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
            *((unsigned int *)&idx[696]) = v20 + 1;
            if (!/* unsupported instruction */)
            {
                *((unsigned int *)&idx[700]) = nan;
                /* unsupported instruction */
                return 1;
            }
            *((int *)&idx[700]) = /* unsupported instruction */;
            /* unsupported instruction */
            return 1;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx[700]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((unsigned int *)&idx[696]) = sub_4931e0();
    return 1;
}
