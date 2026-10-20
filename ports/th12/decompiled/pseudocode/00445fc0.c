// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x445fc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_445fc0_0 {
    char padding_0[20];
    int field_14;
    char padding_18[12];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    int field_30;
    char padding_34[644];
    int field_2b8;
    char padding_2bc[484];
    unsigned int field_4a0;
    char padding_4a4[4];
    unsigned int field_4a8;
} st_445fc0_0;

extern char g_4aebf0;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern unsigned int g_4b0cec;
extern unsigned int g_4b451c;
extern unsigned int g_4b452c;
extern unsigned int g_4ce8b8;
extern unsigned int g_4cee40;
extern char g_4d48c0;
extern void g_4d48c4;
extern char g_4d4ca2;
extern char g_4d4ca3;
extern char g_4d4ca4;
extern char g_4d4ca5;
extern char g_4d4ca6;
extern char g_4d4ca7;
extern char g_4d4ca8;
extern char g_4d4ca9;
extern char g_4d4caa;
extern char g_4d4cd1;
extern char g_4d4cd2;
extern char g_4d4cd3;
extern char g_4d4cd4;
extern char g_4d4cd5;
extern char g_4d4cd6;
extern char g_4d4cd7;
extern char g_4d4cd8;
extern char g_4d4cd9;

int sub_445fc0(void)
{
    unsigned int v6;  // edi
    st_445fc0_0 *idx;  // eax
    int v9;  // eax
    int v10;  // edx
    st_445fc0_0 *v11;  // esi
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0x30]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v4;  // [bp-0x8]

    v4 = v6;
    switch (idx->field_24)
    {
    case 2:
        v11 = &idx->field_28;
        *((unsigned int *)&v11->padding_0[4]) = idx->field_28;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if (*((int *)&v11->padding_0[4]) != *((int *)&v11->padding_0[0]))
            sub_453d90();
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_43ef40();
            sub_453d90();
            g_4ce8b8 = *((int *)&v11->padding_0[0]);
            return;
        }
        else if (*((int *)&g_4d48c4) & 524289)
        {
            if (*((char *)((g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + (*((int *)&v11->padding_0[0]) + g_4b0ca8 * 6) * 8 + 1457)))
                sub_43ef40();
            sub_453d90();
            g_4ce8b8 = *((int *)&v11->padding_0[0]);
            g_4b0cec = 0;
            if (!sub_463910())
            {
                if (g_4d4cd1 & 128)
                {
                    g_4b0cec = 1;
                    return;
                }
                else if (g_4d4cd2 & 128)
                {
                    g_4b0cec = 2;
                    return;
                }
                else if (g_4d4cd3 & 128)
                {
                    g_4b0cec = 3;
                    return;
                }
                else if (g_4d4cd4 & 128)
                {
                    g_4b0cec = 4;
                    return;
                }
                else if (g_4d4cd5 & 128)
                {
                    g_4b0cec = 5;
                    return;
                }
                else if (g_4d4cd6 & 128)
                {
                    g_4b0cec = 6;
                    return;
                }
                else if (g_4d4cd7 & 128)
                {
                    g_4b0cec = 7;
                    return;
                }
                else if (g_4d4cd8 & 128)
                {
                    g_4b0cec = 8;
                    return;
                }
                else if (g_4d4cd9 & 128)
                {
                    g_4b0cec = 9;
                    return;
                }
            }
            else
            {
                if (g_4d4ca2 & 128)
                {
                    g_4b0cec = 1;
                    return;
                }
                else if (g_4d4ca3 & 128)
                {
                    g_4b0cec = 2;
                    return;
                }
                else if (g_4d4ca4 & 128)
                {
                    g_4b0cec = 3;
                    return;
                }
                else if (g_4d4ca5 & 128)
                {
                    g_4b0cec = 4;
                    return;
                }
                else if (g_4d4ca6 & 128)
                {
                    g_4b0cec = 5;
                    return;
                }
                else if (g_4d4ca7 & 128)
                {
                    g_4b0cec = 6;
                    return;
                }
                else if (g_4d4ca8 & 128)
                {
                    g_4b0cec = 7;
                    return;
                }
                else if (g_4d4ca9 & 128)
                {
                    g_4b0cec = 8;
                    return;
                }
                else if (g_4d4caa & 128)
                {
                    g_4b0cec = 9;
                    return;
                }
            }
        }
        break;
    case 3:
        if (idx->field_2b8 == 10)
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
            sub_4529a0(5, 32, 0, 0, 0, 59);
        }
        if (idx->field_2b8 >= 40)
        {
            sub_464900();
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
            v12 = idx->field_28 + 1;
            v0 = &(&g_4aebf0)[64 * v12];
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
            g_4b0cb0 = v12;
            g_4b0cb4 = v12;
            sub_430270();
            g_4cee40 = 7;
            return;
        }
        break;
    case 4:
        if (idx->field_2b8 < 6)
            return;
        sub_43efd0();
        sub_43efd0();
        sub_43eee0();
        sub_464940();
        return;
    case 0:
        idx->field_30 = 6;
        v9 = idx->field_30;
        v1 = 118;
        idx->field_28 = (!v9 ? g_4ce8b8 : (g_4ce8b8 < v9 ? (unsigned int)((g_4ce8b8 < 0) + 1) & g_4ce8b8 : v9 - 1));
        sub_4615a0(idx->field_14, &ecx, 118, 0);
        idx->field_4a0 = 118;
        v10 = idx->field_14;
        sub_4615a0(v10, &v1, 120, 0);
        idx->field_4a8 = 120;
        sub_43ef40(v10, &v1, 120, 0);
    case 1:
        if (idx->field_2b8 > 10)
        {
            sub_43ef40();
            return;
        }
        break;
    default:
        return;
    }
}
