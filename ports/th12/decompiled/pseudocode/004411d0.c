// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4411d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4411d0_0 {
    char padding_0[20];
    int field_14;
    char padding_18[12];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    int field_30;
    char padding_34[644];
    int field_2b8;
    char padding_2bc[16];
    unsigned int field_2cc;
} st_4411d0_0;

extern char g_4cead0;
extern char g_4cead1;
extern char g_4cead2;
extern char g_4d48c0;
extern void g_4d48c4;

unsigned int sub_4411d0(unsigned int a0)
{
    unsigned int v6;  // ebx
    st_4411d0_0 *idx;  // esi
    char v16;  // 4099
    unsigned int v17;  // edi
    unsigned int v9;  // eax
    unsigned int v10;  // 4101
    unsigned int v11;  // ecx
    unsigned int v12;  // edx
    st_4411d0_0 *index;  // edi
    unsigned int v14;  // eax
    char v15;  // 4099
    unsigned int v0;  // [bp-0x20]
    st_4411d0_0 *v1;  // [bp-0x18]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v4 = a0;
    v3 = v6;
    switch (idx->field_24)
    {
    case 2:
        v12 = idx->field_28;
        index = &idx->field_28;
        *((unsigned int *)&index->padding_0[4]) = v12;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if (*((int *)&index->padding_0[4]) != *((int *)&index->padding_0[0]))
        {
            sub_453d90();
            sub_4619e0(idx->field_2cc);
            sub_461970(idx->field_2cc, idx->field_2cc);
            sub_441530(idx);
            v1 = idx;
        }
        if (*((int *)&g_4d48c4) & 258)
        {
            if (*((int *)&index->padding_0[0]) != 4)
            {
                sub_453d90();
                v14 = *((int *)&index->padding_0[8]);
                if (!v14)
                {
                    *((unsigned int *)&index->padding_0[0]) = 4;
                }
                else if (v14 <= 4)
                {
                    *((unsigned int *)&index->padding_0[0]) = v14 - 1;
                }
                else
                {
                    *((unsigned int *)&index->padding_0[0]) = 4;
                }
                sub_4619e0(idx->field_2cc);
                sub_461970(idx->field_2cc, v0);
                sub_441530(idx);
                return 1;
            }
LABEL_441455:
            sub_461970(idx->field_2cc, v1);
            sub_453d90();
            sub_43ef40();
            return 1;
        }
        else
        {
            if (*((int *)&index->padding_0[0]) == 1 && sub_407700(&idx->padding_34[640], v12, 60))
                sub_453d90();
            if (g_4d48c4 & 64 || g_4d48c0 & 64)
            {
                if (*((int *)&index->padding_0[0]))
                {
                    if (*((int *)&index->padding_0[0]) != 1)
                        goto LABEL_4413db;
                    if (g_4cead1 < 5)
                        g_4cead1 = 0;
                    else
                        g_4cead1 = g_4cead1 - 5;
                }
                else if (g_4cead0 < 5)
                {
                    g_4cead0 = 0;
                }
                else
                {
                    g_4cead0 = g_4cead0 - 5;
                }
                sub_4426b0();
            }
LABEL_4413db:
            if (g_4d48c4 & 128 || g_4d48c0 & 128)
            {
                if (*((int *)&index->padding_0[0]))
                {
                    if (*((int *)&index->padding_0[0]) != 1)
                        goto LABEL_441430;
                    v15 = g_4cead1 + 5;
                    g_4cead1 = g_4cead1 + 5;
                    if (v15 > 100)
                        g_4cead1 = 100;
                }
                else
                {
                    v16 = g_4cead0 + 5;
                    g_4cead0 = g_4cead0 + 5;
                    if (v16 > 100)
                        g_4cead0 = 100;
                }
                sub_4426b0();
            }
LABEL_441430:
            if (*((int *)&g_4d48c4) & 524289)
            {
                v17 = *((int *)&index->padding_0[0]) - 2;
                if (*((int *)&index->padding_0[0]) == 2)
                {
                    sub_461970(idx->field_2cc, v1);
                    sub_453d90();
                    sub_43ef40();
                    return 1;
                }
                if (v17 == 1)
                {
                    g_4cead0 = 100;
                    g_4cead1 = 80;
                    g_4cead2 = 0;
                    sub_4426b0();
                    sub_453d90();
                    return 1;
                }
                if (v17 == 2)
                    goto LABEL_441455;
                break;
            }
        }
        break;
    case 4:
        if (idx->field_2b8 >= 10)
        {
            if (idx->field_28 == 2)
            {
                sub_43eee0();
                sub_464900();
            }
            else if (idx->field_28 == 4)
            {
                sub_43eee0();
                sub_464940();
                return 1;
            }
        }
        break;
    case 0:
        idx->field_30 = 5;
        v9 = idx->field_30;
        if (v9)
        {
            v10 = _ccall(14, 15, v9, 0, 0);
            if (v10 & 1)
                idx->field_28 = v9 - 1;
            else
                idx->field_28 = 0;
        }
        else
        {
            idx->field_28 = 0;
        }
        v11 = idx->field_14;
        sub_4615a0(v11, &v4, 1, 0);
        idx->field_2cc = 1;
        sub_4426b0(v11, &v4, 1, 0);
        sub_43ef40();
    case 1:
        if (idx->field_2b8 > 6)
        {
            sub_43ef40();
            v0 = idx->field_2cc;
            sub_4619e0(idx->field_2cc);
            sub_461970(idx->field_2cc, v0);
            sub_441530(idx);
            return 1;
        }
        break;
    case 3:
        return 1;
    default:
        return 1;
    }
}
