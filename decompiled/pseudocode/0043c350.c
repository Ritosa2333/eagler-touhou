// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43c350; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43c350_1 {
    char padding_0[112];
    unsigned short field_70;
    char padding_72[2];
    unsigned int field_74;
    unsigned int field_78;
    char padding_7c[148];
    char field_110;
} st_43c350_1;

typedef struct st_43c350_0 {
    unsigned int field_0;
    unsigned short field_4;
    char padding_6[22];
    int field_1c;
    int field_20;
} st_43c350_0;

typedef struct st_43c350_2 {
    char padding_0[24];
    struct st_43c350_0 *field_18;
    struct st_43c350_1 *field_1c;
    char padding_20[424];
    struct st_43c350_1 *field_1c8;
} st_43c350_2;

extern unsigned int g_4ad138;
extern char g_4b0ce0;

int sub_43c350(st_43c350_2 *idx, char *a1)
{
    char *v6;  // eax
    unsigned int v7;  // esi
    st_43c350_1 *v16;  // eax
    unsigned int index;  // eax
    int v18;  // esi
    unsigned int v19;  // ebx
    int v20;  // ecx
    unsigned int v21;  // edx
    unsigned int v22;  // eax
    unsigned int v8;  // edi
    char *iter;  // ecx
    char i;  // 4102
    st_43c350_0 *v11;  // eax
    st_43c350_0 *v12;  // edi
    st_43c350_0 *v13;  // eax
    st_43c350_1 *v14;  // eax
    st_43c350_0 *v15;  // ecx
    unsigned int v0;  // [bp-0x154]
    unsigned int v1;  // [bp-0x114]
    unsigned int v2;  // [bp-0x110]
    char v3;  // [bp-0x108]
    char v4;  // [bp-0x104]
    unsigned int v5;  // [bp-0x4]

    v5 = g_4ad138 ^ &v3;
    v6 = a1;
    v2 = v7;
    v1 = v8;
    iter = v6;
    do
    {
        i = *(iter);
        *(&idx->padding_0[0] - v6 + iter + 480) = *(iter);
        iter += 1;
    } while (i);
    if (g_4b0ce0 & 32)
    {
        v13 = sub_463c10(&v3, 0);
        idx->field_18 = v13;
        v12 = v13 + 1;
        goto LABEL_43c40c;
    }
    sub_46cbbb(&v4, "replay/%s", v6);
    if (sub_463df0(&v4) && !sub_464070())
    {
        v11 = sub_464190();
        idx->field_18 = v11;
        if (v11->field_0 != 1915892084 || v11->field_4 != 4)
        {
            sub_4641e0();
            goto LABEL_43c3da;
        }
        else
        {
            v12 = sub_464190();
            sub_4641e0();
LABEL_43c40c:
            v14 = sub_46d04a(idx->field_18->field_20);
            v15 = idx->field_18;
            idx->field_1c8 = v14;
            sub_4639d0(v12, v15->field_1c, 225, 0x800, v15->field_1c);
            sub_4639d0(v12, idx->field_18->field_1c, 58, 64, idx->field_18->field_1c);
            sub_44c710(v12, idx->field_18->field_1c, idx->field_1c8);
            v16 = idx->field_1c8;
            idx->field_1c = v16;
            index = &v16->field_70;
            v18 = 0;
            v0 = v19;
            while (1)
            {
                v20 = *((int *)&idx->field_1c->padding_0[88]);
                if (*((int *)&idx->field_1c->padding_0[88]) >= 8)
                    v20 = 6;
                if (v18 >= v20)
                    break;
                *((unsigned int *)&idx->padding_20[152 + 36 * *((short *)index)]) = index;
                *((unsigned int *)&idx->padding_20[0x88 + 36 * *((short *)index)]) = index + 160;
                v21 = *((short *)index) * 9;
                *((unsigned int *)&idx->padding_20[144 + 4 * v21]) = *((int *)&idx->padding_20[0x88 + 4 * v21]) + *((int *)(index + 4)) * 6;
                index = index + *((int *)(index + 8)) + 160;
                v18 += 1;
            }
            if (!(g_4b0ce0 & 32) && v12)
                sub_46c941(v12);
        }
    }
    else
    {
LABEL_43c3da:
    }
    _security_check_cookie();
    return v22;
}
