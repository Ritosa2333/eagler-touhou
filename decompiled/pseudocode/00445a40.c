// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x445a40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_445a40_1 {
    char padding_0[16];
    struct st_445a40_0 *field_10;
    struct st_445a40_1 *field_14;
} st_445a40_1;

typedef struct st_445a40_2 {
    char padding_0[712];
    int field_2c8;
} st_445a40_2;

typedef struct st_445a40_0 {
    unsigned int field_0;
    char padding_4[998];
    unsigned short field_3ea;
} st_445a40_0;

extern char g_4aec30;
extern char g_4aedb0;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern char g_4b0ce0;
extern unsigned int g_4b451c;
extern unsigned int g_4b452c;
extern unsigned int g_4ce8c0;
extern unsigned int g_4cee40;
extern char g_4d48c0;
extern void g_4d48c4;

unsigned int sub_445a40(unsigned int a0, unsigned int a1, void* iter)
{
    int v6;  // eax
    unsigned int v14;  // eax
    st_445a40_1 *v15;  // eax
    st_445a40_1 *node;  // eax
    st_445a40_2 *v17;  // esi
    st_445a40_1 *v18;  // eax
    st_445a40_1 *iter1;  // eax
    void* v20;  // esi
    unsigned int v21;  // ebx
    unsigned int v22;  // ecx
    int v7;  // eax
    unsigned int v8;  // 4101
    unsigned int v9;  // ecx
    unsigned int v10;  // edi
    unsigned int v11;  // esi
    unsigned int v12;  // esi
    unsigned int v13;  // eax
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x4]

    v4 = a0;
    iter = (-(0 < g_4b0ca8 - 4) & 0xfffffffd) + 178;
    switch ((int)iter[36])
    {
    case 2:
        v20 = iter + 40;
        *((int *)&v20[4]) = (int)iter[40];
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
        {
            v0 = 1;
            sub_464970(1);
        }
        if ((int)v20[4] != *((int *)v20))
        {
            sub_453d90();
            sub_4619e0(*((int *)((char *)iter + 4 * g_4b0c90 + 4 * iter + 712)));
            sub_461970(*((int *)((char *)iter + 4 * g_4b0c90 + 4 * v21 + 712)), *((int *)((char *)iter + 4 * g_4b0c90 + 4 * iter + 712)));
        }
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_43ef40();
            sub_453d90();
            return 1;
        }
        else if (*((int *)&g_4d48c4) & 524289)
        {
            sub_43ef40();
            sub_453d90();
            sub_462060();
            sub_461970(v4, v0);
            if (!(g_4b0ce0 & 16))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v0 = v22;
                /* unsupported instruction */
                sub_430270((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                return 1;
            }
        }
        goto LABEL_445f90;
    case 3:
        if ((int)iter[696] == 10)
        {
            if (!(g_4b0ce0 & 16))
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
                    v1 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v1 = nan;
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
                    v0 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v0 = nan;
                    /* unsupported instruction */
                }
                sub_411b80();
                sub_4529a0(5, 32, 0, 0, 0, 64);
            }
            else
            {
                g_4b0c94 = (int)iter[40];
                g_4ce8c0 = g_4b0c94;
                sub_464900();
                sub_43efd0();
                sub_43eee0();
            }
        }
        if ((int)iter[696] >= 40)
        {
            g_4b0c94 = (int)iter[40];
            g_4ce8c0 = g_4b0c94;
            sub_464900();
            if (g_4b0ce0 & 16)
            {
                sub_43efd0();
                sub_43eee0();
                return 1;
            }
            sub_43eee0();
            if (g_4b0ca8 < 4)
            {
                g_4cee40 = 7;
                g_4b452c = &g_4aec30;
                g_4b0cb0 = 1;
                g_4b0cb4 = 1;
            }
            else
            {
                g_4b0cb0 = 7;
                g_4b0cb4 = 7;
                g_4cee40 = 7;
                g_4b452c = &g_4aedb0;
            }
            return 1;
        }
        goto LABEL_445f90;
    case 4:
        if ((int)iter[696] >= 6)
        {
            iter += 40;
            g_4b0c94 = *((int *)iter);
            g_4ce8c0 = g_4b0c94;
            sub_43efd0();
            sub_43efd0();
            sub_43eee0();
            sub_464940();
        }
LABEL_445f90:
        return 1;
    case 0:
        *((int *)&iter[48]) = 2;
        if (g_4b0ca8 == 4)
        {
            if (!(*((char *)(g_4b451c + g_4b0c90 * 2 + 125392)) & 16))
            {
                if (!(int)iter[40])
                {
                    v6 = (int)iter[48];
                    if (!v6)
                    {
                        *((unsigned int *)&iter[40]) = 1;
                        break;
                    }
                    else if (v6 <= 1)
                    {
                        *((unsigned int *)&iter[40]) = v6 - 1;
                        break;
                    }
                    else
                    {
                        *((unsigned int *)&iter[40]) = 1;
                        break;
                    }
                }
                *((unsigned int *)((char *)iter + 4 * iter[252] + 184)) = 0;
                *((unsigned int *)&iter[252]) = (int)iter[252] + 1;
            }
            if (!(*((char *)(g_4b451c + g_4b0c90 * 2 + 125393)) & 16))
            {
                if ((int)iter[40] == 1)
                {
                    v7 = (int)iter[48];
                    if (v7)
                    {
                        v8 = _ccall(14, 15, v7, 0, 0);
                        if (v8 & 1)
                            *((unsigned int *)&iter[40]) = v7 - 1;
                        else
                            *((unsigned int *)&iter[40]) = 0;
                    }
                    else
                    {
                        *((unsigned int *)&iter[40]) = 0;
                    }
                }
                *((unsigned int *)((char *)iter + 4 * iter[252] + 184)) = 1;
                *((unsigned int *)&iter[252]) = (int)iter[252] + 1;
            }
        }
        v9 = (int)iter[20];
        sub_4615a0(v9, &v4, 112, 0);
        *((unsigned int *)&iter[1160]) = v10;
        v11 = g_4b0c90 + iter;
        sub_461970(*((int *)((char *)iter + 4 * v11 + 712)), v9);
        *((unsigned int *)((char *)iter + 4 * v11 + 712)) = 0;
        sub_43efa0(&v4);
        sub_4619e0(*((int *)((char *)iter + 4 * g_4b0c90 + 4 * v21 + 712)));
        v12 = v3;
        sub_461970(*((int *)((char *)iter + 4 * g_4b0c90 + 4 * v12 + 712)), *((int *)((char *)iter + 4 * g_4b0c90 + 4 * v21 + 712)));
        sub_43ef40(&v4);
        v13 = g_4b0c90;
        if (!*((int *)(g_4b451c + (g_4b0c90 * 8954 + g_4b0ca8) * 4 + 1432)))
        {
            v14 = g_4b0c90 + v12;
            v15 = sub_461920(*((int *)((char *)iter + 4 * v14 + 712)));
            if (!v15)
                *((unsigned int *)((char *)iter + 4 * v14 + 712)) = 0;
            node = &v15->field_10;
            if (v15 != 0xfffffff0)
            {
                do
                {
                    if (*((short *)(*((int *)&node->padding_0[0]) + 1002)) == 168)
                    {
                        v3 = *((int *)*((int *)&node->padding_0[0]));
                        goto LABEL_445c43;
                    }
                } while ((node = (st_445a40_1 *)*((int *)&node->padding_0[4]), node));
            }
            v3 = 0;
LABEL_445c43:
            sub_461d80();
            v13 = g_4b0c90;
        }
        if (!*((int *)(g_4b451c + (v13 * 8954 + g_4b0ca8) * 4 + 19340)))
        {
            v17 = iter + (v13 + v12) * 4 + 712;
            v18 = sub_461920(*((int *)&v17->padding_0[0]));
            if (!v18)
                *((int *)&v17->padding_0[0]) = 0;
            iter1 = &v18->field_10;
            if (v18 != 0xfffffff0)
            {
                do
                {
                    if (*((short *)(*((int *)&iter1->padding_0[0]) + 1002)) == 169)
                    {
                        v2 = *((int *)*((int *)&iter1->padding_0[0]));
                        goto LABEL_445caf;
                    }
                } while ((iter1 = (st_445a40_1 *)*((int *)&iter1->padding_0[4]), iter1));
            }
            v2 = 0;
LABEL_445caf:
            sub_461d80();
        }
    case 1:
        if ((int)iter[696] <= 6)
            return 1;
        sub_43ef40();
        return 1;
    default:
        return 1;
    }
}
