// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427230; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427230_0 {
    char padding_0[1080];
    unsigned int field_438;
    unsigned int field_43c;
    unsigned int field_440;
    char padding_444[76];
    char field_490;
    char padding_491[1014];
    char field_887;
    char padding_888[100];
    unsigned int field_8ec;
    unsigned int field_8f0;
    unsigned int field_8f4;
    char padding_8f8[204];
    unsigned int field_9c4;
    char padding_9c8[4];
    unsigned int field_9cc;
} st_427230_0;

extern int g_4ce8cc;

int sub_427230(void)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v13;  // ebp
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ecx
    unsigned int v20;  // fc3210
    unsigned int v21;  // edx
    unsigned int v22;  // ftop
    unsigned int v7;  // edi
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    st_427230_0 *v8;  // eax
    unsigned int v33;  // ftop
    unsigned int v34;  // 4103
    unsigned long long v35;  // 4111
    st_427230_0 *iter;  // esi
    unsigned int v10;  // ebp
    unsigned int v11;  // ftop
    unsigned int v12;  // fpround
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned short v2;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x8]

    v2 = v5;
    v1 = v6;
    v0 = v7;
    iter = &v8->field_43c;
    v10 = 2648;
    do
    {
        v13 = v10;
        if (*((int *)&iter->padding_491[247]) && iter->padding_0[84] & 1)
        {
            v14 = v11 - 1;
            if (/* unsupported instruction */)
            {
                v15 = v14 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v15 = v14 - 1;
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
                *((int *)(&iter->padding_0[0] - 4)) = /* unsupported instruction */;
                /* unsupported instruction */
                v16 = v15 + 1;
            }
            else
            {
                *((unsigned int *)(&iter->padding_0[0] - 4)) = nan;
                /* unsupported instruction */
                v16 = v15 + 1;
            }
            v17 = v16 - 1;
            if (/* unsupported instruction */)
            {
                v18 = v17 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v18 = v17 - 1;
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
            *((int *)&iter->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter->padding_0[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = *((int *)&iter->padding_0[0]);
            v20 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_0[0])) * 0x100 & 0x4500;
            /* unsupported instruction */
            v11 = v18 + 1;
            v21 = *((int *)&iter->padding_0[4]);
            *((int *)&iter->padding_491[31]) = *((int *)(&iter->padding_0[0] - 4));
            *((unsigned int *)&iter->padding_491[35]) = v19;
            *((unsigned int *)&iter->padding_491[39]) = v21;
            if (!((char)((((unsigned short)v11 & 7) * 0x800 | (unsigned short)v20 & 0x4700) >> 8) & 65))
            {
                v22 = v11 - 1;
                if (/* unsupported instruction */)
                {
                    v23 = v22 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v23 = v22 - 1;
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
                if (/* unsupported instruction */)
                {
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v24 = v23 + 1;
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                    v24 = v23 + 1;
                }
                v25 = v24 - 1;
                if (/* unsupported instruction */)
                {
                    v26 = v25 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v26 = v25 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                if (/* unsupported instruction */)
                {
                    *((int *)&iter->padding_491[35]) = /* unsupported instruction */;
                    /* unsupported instruction */
                    v27 = v26 + 1;
                }
                else
                {
                    *((unsigned int *)&iter->padding_491[35]) = nan;
                    /* unsupported instruction */
                    v27 = v26 + 1;
                }
                v28 = v27 - 1;
                if (/* unsupported instruction */)
                {
                    v29 = v28 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v29 = v28 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v30 = v29 - 1;
                if (/* unsupported instruction */)
                {
                    v31 = v30 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v31 = v30 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v33 = v31 + 1;
                if (!((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v11 = v33 + 1;
                    iter->padding_444[7] = 0xff;
                }
                else
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
                    v34 = _ccall(v12);
                    v2 = v34;
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
                    v3 = v2 | 0xc00;
                    if (/* unsupported instruction */)
                    {
                        v3 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v11 = v33 + 1;
                    }
                    else
                    {
                        v3 = nan;
                        /* unsupported instruction */
                        v11 = v33 + 1;
                    }
                    iter->padding_444[7] = v3;
                    v35 = _ccall(v2);
                    v12 = v35;
                }
                sub_45c900(g_4ce8cc);
                *((unsigned int *)&iter->padding_491[0xff]) = 1;
            }
            else
            {
                sub_45c900(g_4ce8cc);
                *((unsigned int *)&iter->padding_491[0xff]) = 0;
            }
        }
    } while ((iter += 2520, v10 = v13 - 1, v13 != 1));
    return;
}
