// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4435b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4435b0_0 {
    char padding_0[20];
    int field_14;
    char padding_18[12];
    unsigned int field_24;
    int field_28;
    int field_2c;
    unsigned int field_30;
    char padding_34[644];
    int field_2b8;
    char padding_2bc[20];
    int field_2d0;
    char padding_2d4[22420];
    unsigned short field_5a68;
    unsigned short field_5a6a;
    unsigned short field_5a6c;
    unsigned short field_5a6e;
    unsigned short field_5a70;
} st_4435b0_0;

extern unsigned int g_4ceab4;
extern unsigned int g_4ceab8;
extern unsigned int g_4ceabc;
extern unsigned int g_4ceac0;
extern unsigned short g_4ceac4;
extern char g_4d48c0;
extern void g_4d48c4;
extern void g_4d49ec;
extern unsigned short g_4d49ee;
extern void g_4d49f0;
extern unsigned short g_4d49f2;
extern unsigned int g_4d49f4;
extern unsigned int g_4d49f8;
extern unsigned short g_4d49fc;

int sub_4435b0(void)
{
    unsigned int v4;  // esi
    unsigned int v5;  // edi
    unsigned short v14;  // ax
    st_4435b0_0 *idx;  // eax
    unsigned int v8;  // eax
    unsigned int v9;  // 4101
    int v10;  // ecx
    st_4435b0_0 *v11;  // esi
    unsigned int v12;  // eax
    int v13;  // ecx
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    v2 = v4;
    v1 = v5;
    switch (idx->field_24)
    {
    case 2:
        v11 = &idx->field_28;
        *((int *)&v11->padding_0[4]) = idx->field_28;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if (*((int *)&v11->padding_0[4]) != *((int *)&v11->padding_0[0]))
        {
            sub_453d90();
            sub_4619e0(idx->field_2d0);
            sub_461970(idx->field_2d0);
            sub_444380(idx);
        }
        v12 = sub_462db0();
        v13 = 0;
        do
        {
            if (*((char *)(v13 + v12)) & 128 && *((int *)&v11->padding_0[0]) <= 4)
            {
                sub_4442b0();
                break;
            }
        } while ((v13 = (int)(v13 + 1), v13 < 31));
        if (*((int *)&g_4d48c4) & 258 && *((int *)&v11->padding_0[0]) == 6)
        {
            idx->field_5a68 = *((short *)&g_4d49ec);
            idx->field_5a6a = g_4d49ee;
            idx->field_5a6c = *((short *)&g_4d49f0);
            idx->field_5a6e = g_4d49f2;
            idx->field_5a70 = g_4d49fc;
            sub_443920();
            goto LABEL_44384c;
        }
        else if (*((int *)&g_4d48c4) & 524289)
        {
            if (*((int *)&v11->padding_0[0]) == 5)
            {
                idx->field_5a68 = *((short *)&g_4d49ec);
                idx->field_5a6a = g_4d49ee;
                idx->field_5a6c = *((short *)&g_4d49f0);
                idx->field_5a6e = g_4d49f2;
                idx->field_5a70 = g_4d49fc;
                sub_443920();
                sub_453d90();
                return;
            }
            if (*((int *)&v11->padding_0[0]) == 6)
            {
                *((unsigned short *)&g_4d49ec) = idx->field_5a68;
                g_4d49ee = idx->field_5a6a;
                *((unsigned short *)&g_4d49f0) = idx->field_5a6c;
                g_4d49f2 = idx->field_5a6e;
                v14 = idx->field_5a70;
                g_4ceab4 = *((int *)&g_4d49ec);
                g_4ceab8 = *((int *)&g_4d49f0);
                g_4d49fc = v14;
                g_4ceabc = g_4d49f4;
                g_4ceac0 = g_4d49f8;
                g_4ceac4 = g_4d49fc;
LABEL_44384c:
                sub_453d90();
                sub_461970(idx->field_2d0);
                sub_43ef40(idx->field_2d0);
                return;
            }
        }
    case 4:
        if (idx->field_2b8 >= 10)
        {
            sub_43eee0();
            sub_464940();
        }
        goto LABEL_4438f7;
    case 0:
        idx->field_30 = 7;
        v8 = idx->field_30;
        if (v8)
        {
            v9 = _ccall(14, 15, v8, 0, 0);
            if (v9 & 1)
            {
                idx->field_28 = v8 - 1;
                break;
            }
            else
            {
                idx->field_28 = 0;
                break;
            }
        }
        else
        {
            idx->field_28 = 0;
            break;
        }
        v10 = idx->field_14;
        sub_4615a0(v10, &ecx, 2, 0);
        idx->field_2d0 = 0;
        sub_43ef40(v10, &ecx, 2, 0);
        idx->field_5a68 = *((short *)&g_4d49ec);
        idx->field_5a6a = g_4d49ee;
        idx->field_5a6c = *((short *)&g_4d49f0);
        idx->field_5a6e = g_4d49f2;
        idx->field_5a70 = g_4d49fc;
        sub_443920();
    case 1:
        if (idx->field_2b8 > 6)
        {
            sub_43ef40();
            sub_4619e0(idx->field_2d0);
            sub_461970(idx->field_2d0);
            sub_444380(idx);
            return;
        }
        goto LABEL_4438f7;
    case 3:
LABEL_4438f7:
        return;
    default:
        return;
    }
}
