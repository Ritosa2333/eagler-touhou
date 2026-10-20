// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4466a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4466a0_0 {
    char padding_0[102324];
    int field_18fb4;
} st_4466a0_0;

extern char g_4aebf0;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern st_4466a0_0 *g_4b43b8;
extern char g_4b43e8;
extern unsigned int g_4b452c;
extern unsigned int g_4ce8b0;
extern unsigned int g_4ce8b4;
extern unsigned int g_4cee40;
extern char g_4d48c0;
extern void g_4d48c4;

unsigned int sub_4466a0(void* idx)
{
    int v3;  // edi
    int v4;  // esi
    unsigned int v13;  // ecx
    int v14;  // eax
    void* v15;  // esi
    unsigned int v16;  // ecx
    unsigned int v17;  // eax
    unsigned int iter;  // eax
    char *node;  // edx
    char v20;  // 4103
    unsigned int *idx1;  // eax
    unsigned int v22;  // edx
    int v5;  // ebx
    void* iter1;  // esi
    int v24;  // ebx
    int v25;  // ebx
    unsigned int v26;  // edi
    void* index;  // esi
    void* idx2;  // edi
    unsigned int v8;  // ecx
    unsigned int v9;  // ecx
    unsigned int v10;  // ecx
    unsigned int v11;  // eax
    unsigned int v12;  // 4101
    unsigned int v0;  // [bp-0x34]
    unsigned int v1;  // [bp-0x30]
    unsigned int v2;  // [bp-0x2c]

    switch ((int)idx[36])
    {
    case 2:
        index = idx + 40;
        *((int *)&index[4]) = (int)idx[40];
        idx2 = idx + 472;
        *((int *)&idx2[4]) = (int)idx[472];
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if (g_4d48c4 & 64 || g_4d48c0 & 64)
            sub_464970(-0x1);
        if (g_4d48c4 & 128 || g_4d48c0 & 128)
            sub_464970(1);
        v8 = (int)idx2[4];
        if (v8 != *((int *)idx2))
            sub_453d90(v8, 10);
        if ((int)index[4] != *((int *)index))
            sub_453d90(v8, 10);
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_43ef40();
            sub_453d90(v9, 9);
            *((unsigned int *)&idx[23576]) = (int)idx[23576] | 4;
            return 1;
        }
        else if (*((int *)&g_4d48c4) & 524289 && *((int *)((char *)idx + 100 * *((int *)idx2) + 4 * *((int *)index) + 23168)))
        {
            sub_43ef40();
            *((unsigned int *)&idx[23160]) = *((int *)idx2) * 25 + *((int *)index);
            sub_464900();
            sub_453d90(v10, 7);
            *((unsigned int *)&idx[48]) = 7;
            v11 = (int)index[8];
            if (v11)
            {
                v12 = _ccall(14, 15, v11, 0, 0);
                if (v12 & 1)
                    *((unsigned int *)index) = v11 - 1;
                else
                    *((unsigned int *)index) = 0;
            }
            else
            {
                *((unsigned int *)index) = 0;
            }
            v13 = 0;
            v14 = 0;
            do
            {
                if (!*((int *)(*((int *)((char *)idx + 4 * idx[23160] + 23168)) + v14 + 220)))
                {
                    *((unsigned int *)((char *)index + 4 * index[212] + 144)) = v13;
                    *((unsigned int *)&index[212]) = (int)index[212] + 1;
                }
            } while ((v14 = (int)(v14 + 36), v13 += 1, v14 < 252));
            sub_464970(-0x1);
            sub_464970(1);
            return 1;
        }
        break;
    case 3:
        if ((int)idx[696] == 2)
        {
            sub_4529a0(5, 32, 0, 0, 0, 59);
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
                v2 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v2 = nan;
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
            if (/* unsupported instruction */)
            {
                v1 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v1 = nan;
                /* unsupported instruction */
            }
            sub_411b80();
        }
        if ((int)idx[696] >= 32 && (char)idx[23576] & 8)
        {
            sub_43eee0();
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
            v17 = (int)idx[23164] + 1;
            v0 = &(&g_4aebf0)[64 * v17];
            if (/* unsupported instruction */)
            {
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v0 = nan;
                /* unsupported instruction */
            }
            g_4b452c = v0;
            g_4b0cb0 = v17;
            g_4b0cb4 = v17;
            sub_430270();
            g_4cee40 = 13;
            iter = *((int *)((char *)idx + 4 * idx[23160] + 23168)) + 480;
            node = &g_4b43e8;
            do
            {
                v20 = *((char *)iter);
                *(node) = *((char *)iter);
                iter += 1;
                node += 1;
            } while (v20);
            idx1 = *((int *)(*((int *)((char *)idx + 4 * idx[23160] + 23168)) + 28));
            g_4b0c90 = idx1[23];
            g_4b0c94 = idx1[24];
            v22 = idx1[25];
            g_4ce8b0 = 2;
            g_4b0ca8 = v22;
            g_4ce8b4 = (int)idx[23160];
            return 1;
        }
        break;
    case 4:
        if ((int)idx[696] >= 15)
        {
            v15 = idx + 40;
            *((int *)&v15[4]) = (int)idx[40];
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0x1);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(1);
            if ((int)v15[4] != *((int *)v15))
                sub_453d90((int)v15[4], 10);
            if (*((int *)&g_4d48c4) & 258)
            {
                sub_464940();
                *((unsigned int *)&idx[48]) = 25;
                *((unsigned int *)&idx[252]) = 0;
                sub_43ef40();
                sub_453d90(v16, 9);
                return 1;
            }
            else if (*((int *)&g_4d48c4) & 524289)
            {
                *((int *)&idx[23164]) = *((int *)v15);
                sub_43ef40();
                *((unsigned int *)&idx[23576]) = (int)idx[23576] | 4;
                return 1;
            }
        }
        break;
    case 5:
        if ((int)idx[696] < 6 || !((char)idx[23576] & 8))
            return 1;
        iter1 = idx + 23168;
        v24 = 100;
        do
        {
            v25 = v24;
            v26 = *((int *)iter1);
            if (v26)
            {
                sub_43b450(v26);
                sub_46ca4f(v26);
            }
        } while ((iter1 += 4, v24 = (int)(v25 - 1), v25 != 1));
        sub_477420(idx + 23168, v24, 400);
        sub_461970((int)idx[1164]);
        *((int *)&idx[1164]) = 0;
        sub_461970((int)idx[1644]);
        *((int *)&idx[1644]) = 0;
        sub_43eee0((int)idx[1644]);
        sub_464940();
        return 1;
    case 0:
        *((unsigned int *)&idx[48]) = 25;
        sub_40f790(v3, v4, v5);
        *((unsigned int *)&idx[480]) = 3;
        sub_40f790();
        *((unsigned int *)&idx[680]) = 1;
        g_4ce8b4 = 0;
        if (!(int)idx[1644])
        {
            sub_4615a0(g_4b43b8->field_18fb4, &idx, 20, 0);
            *((int *)&idx[1644]) = v4;
        }
        sub_43efa0();
        sub_43ef40();
        sub_477420(idx + 23168, 0, 400);
        *((unsigned int *)&idx[23576]) = (int)idx[23576] & 0xfffffff3;
        *((unsigned int *)&idx[23156]) = 0;
        sub_464cb0(idx);
        if (!sub_461920((int)idx[1108]))
        {
            sub_43efa0();
            sub_4619e0((int)idx[1108]);
        }
    case 1:
        if ((int)idx[696] > 6)
        {
            sub_43ef40();
            return 1;
        }
        break;
    default:
        return 1;
    }
}
