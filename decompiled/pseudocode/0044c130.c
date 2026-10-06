// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c130; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44c130_2 {
    char field_0;
    char field_1;
    char field_2;
} st_44c130_2;

typedef struct st_44c130_0 {
    unsigned int field_0;
    char field_4;
} st_44c130_0;

typedef struct st_44c130_1 {
    char padding_0[8];
    struct st_44c130_2 *field_8;
} st_44c130_1;

extern char g_4b4538;
extern st_44c130_1 g_4cf2b0;

int sub_44c130(char *a0)
{
    char *v5;  // esi
    char *v6;  // eax
    char *v15;  // edx
    char v16;  // 4101
    unsigned int v17;  // cc_dep1
    unsigned int v18;  // cc_dep2
    char v19;  // 4103
    unsigned int v20;  // ecx
    unsigned int v21;  // 4111
    unsigned int v22;  // 4120
    unsigned int v23;  // eax
    unsigned int v24;  // eax
    char *v7;  // eax
    int v8;  // edi
    char *node;  // eax
    char i;  // 4102
    st_44c130_0 *v11;  // eax
    int j;  // esi
    st_44c130_1 *iter;  // eax
    char *iter1;  // ecx
    int v0;  // [bp-0x114]
    int v1;  // [bp-0x110]
    int v2;  // [bp-0x10c]
    unsigned int v3;  // [bp-0x108]

    v5 = a0;
    v6 = sub_46d1a0(v5, 47, v0, v1, v2);
    if (v6)
        v7 = v6 + 1;
    else
        v7 = v5;
    v8 = v7 - v5 - 1;
    node = v5;
    do
    {
        i = *(node);
        *(&v3 - v5 + node) = *(node);
        node += 1;
    } while (i);
    if (v8 >= 0)
    {
        v11 = (char *)&v3 + v8;
        v11->field_0 = 1952539694;
        v11->field_4 = 0;
    }
    j = 0;
    iter = &g_4cf2b0.padding_0[0];
    if (*((int *)&g_4b4538) <= 0)
        return v24;
    do
    {
        iter1 = &iter->field_8->field_0;
        v15 = &v3;
        while (1)
        {
            v16 = *(iter1);
            v17 = v16;
            v18 = *(v15);
            if (v16 != *(v15))
                break;
            if (v16)
            {
                v19 = iter1[1];
                v17 = v19;
                v18 = v15[1];
                if (v19 != v15[1])
                    break;
                iter1 += 2;
                v15 += 2;
                if (!v19)
                    goto LABEL_44c1cc;
            }
            else
            {
LABEL_44c1cc:
                v20 = 0;
                goto LABEL_44c1d5;
            }
        }
        v21 = _ccall(4, v17, v18, 0);
        v22 = _ccall(12, 0, v21 & 1, v21 & 1);
        v20 = -(v21 & 1) + 1 - (v22 & 1);
LABEL_44c1d5:
        if (!v20)
            return v23;
        j += 1;
        iter = &iter[1].padding_0[4];
    } while (j < *((int *)&g_4b4538));
    return v24;
}
