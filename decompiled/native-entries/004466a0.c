// AUTOMATIC PSEUDOCODE; NOT ORIGINAL SOURCE
// replay_menu_tick 0x4466a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417

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
    unsigned int v13;  // eax
    unsigned int iter;  // eax
    char *node;  // edx
    char v16;  // 4103
    unsigned int *idx1;  // eax
    unsigned int v18;  // edx
    void* iter1;  // esi
    int v20;  // ebx
    int v21;  // ebx
    unsigned int v22;  // edi
    int v5;  // ebx
    void* index;  // esi
    void* v7;  // edi
    unsigned int v8;  // eax
    unsigned int v9;  // 4101
    unsigned int v10;  // ecx
    int v11;  // eax
    void* v12;  // esi
    unsigned int v0;  // [bp-0x34]
    unsigned int v1;  // [bp-0x30]
    unsigned int v2;  // [bp-0x2c]

    switch ((int)idx[36])
    {
    case 2:
        index = idx + 40;
        *((int *)&index[4]) = (int)idx[40];
        v7 = idx + 472;
        *((int *)&v7[4]) = (int)idx[472];
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if (g_4d48c4 & 64 || g_4d48c0 & 64)
            sub_464970(-0x1);
        if (g_4d48c4 & 128 || g_4d48c0 & 128)
            sub_464970(1);
        if ((int)v7[4] != *((int *)v7))
            sub_453d90();
        if ((int)index[4] != *((int *)index))
            sub_453d90();
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_43ef40();
            sub_453d90();
            *((unsigned int *)&idx[23576]) = (int)idx[23576] | 4;
            return 1;
        }
        else if (*((int *)&g_4d48c4) & 524289 && *((int *)((char *)idx + 100 * *((int *)v7) + 4 * *((int *)index) + 23168)))
        {
            sub_43ef40();
            *((unsigned int *)&idx[23160]) = *((int *)v7) * 25 + *((int *)index);
            sub_464900();
            sub_453d90();
            *((unsigned int *)&idx[48]) = 7;
            v8 = (int)index[8];
            if (v8)
            {
                v9 = _ccall(14, 15, v8, 0, 0);
                if (v9 & 1)
                    *((unsigned int *)index) = v8 - 1;
                else
                    *((unsigned int *)index) = 0;
            }
            else
            {
                *((unsigned int *)index) = 0;
            }
            v10 = 0;
            v11 = 0;
            do
            {
                if (!*((int *)(*((int *)((char *)idx + 4 * idx[23160] + 23168)) + v11 + 220)))
                {
                    *((unsigned int *)((char *)index + 4 * index[212] + 144)) = v10;
                    *((unsigned int *)&index[212]) = (int)index[212] + 1;
                }
            } while ((v11 = (int)(v11 + 36), v10 += 1, v11 < 252));
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
            v13 = (int)idx[23164] + 1;
            v0 = &(&g_4aebf0)[64 * v13];
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
            g_4b0cb0 = v13;
            g_4b0cb4 = v13;
            sub_430270();
            g_4cee40 = 13;
            iter = *((int *)((char *)idx + 4 * idx[23160] + 23168)) + 480;
            node = &g_4b43e8;
            do
            {
                v16 = *((char *)iter);
                *(node) = *((char *)iter);
                iter += 1;
                node += 1;
            } while (v16);
            idx1 = *((int *)(*((int *)((char *)idx + 4 * idx[23160] + 23168)) + 28));
            g_4b0c90 = idx1[23];
            g_4b0c94 = idx1[24];
            v18 = idx1[25];
            g_4ce8b0 = 2;
            g_4b0ca8 = v18;
            g_4ce8b4 = (int)idx[23160];
            return 1;
        }
        break;
    case 4:
        if ((int)idx[696] >= 15)
        {
            v12 = idx + 40;
            *((int *)&v12[4]) = (int)idx[40];
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0x1);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(1);
            if ((int)v12[4] != *((int *)v12))
                sub_453d90();
            if (*((int *)&g_4d48c4) & 258)
            {
                sub_464940();
                *((unsigned int *)&idx[48]) = 25;
                *((unsigned int *)&idx[252]) = 0;
                sub_43ef40();
                sub_453d90();
                return 1;
            }
            else if (*((int *)&g_4d48c4) & 524289)
            {
                *((int *)&idx[23164]) = *((int *)v12);
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
        v20 = 100;
        do
        {
            v21 = v20;
            v22 = *((int *)iter1);
            if (v22)
            {
                sub_43b450(v22);
                sub_46ca4f(v22);
            }
        } while ((iter1 += 4, v20 = (int)(v21 - 1), v21 != 1));
        sub_477420(idx + 23168, v20, 400);
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
