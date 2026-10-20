// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44b7b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44b7b0_3 {
    char padding_0[8];
    int field_8;
    struct st_44b7b0_4 *field_c;
} st_44b7b0_3;

typedef struct st_44b7b0_5 {
    struct st_44b7b0_6 *field_0;
    unsigned int field_4;
    char padding_8[12];
    unsigned int field_14;
} st_44b7b0_5;

typedef struct st_44b7b0_0 {
    char padding_0[8];
    struct st_44b7b0_1 *field_8;
    char padding_c[12];
    struct st_44b7b0_1 *field_18;
} st_44b7b0_0;

typedef struct st_44b7b0_4 {
    struct st_44b7b0_0 *field_0;
} st_44b7b0_4;

typedef struct st_44b7b0_1 {
    unsigned int field_0;
} st_44b7b0_1;

typedef struct st_44b7b0_6 {
    char field_0;
} st_44b7b0_6;

extern char g_4ae531[4];
extern unsigned int g_4ae538[4];

unsigned int * sub_44b7b0(unsigned int a0, unsigned int a1, st_44b7b0_3 *a2, unsigned int *a3)
{
    unsigned int *v0;  // esi
    int v1;  // edi
    char *v9;  // eax
    unsigned int i;  // eax
    char *v11;  // edx
    unsigned int v12;  // eax
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int *v15;  // eax
    int v2;  // ebx
    st_44b7b0_5 *idx;  // ebx
    unsigned int *v4;  // edi
    char *v5;  // eax
    char *v6;  // edx
    unsigned int v7;  // ecx
    char *v8;  // eax

    v0 = NULL;
    if (!a2->field_c)
        return NULL;
    idx = sub_44b920(v1, v2);
    if (idx)
    {
        v4 = idx->field_14 - idx->field_4;
        a2 = *((int *)&idx->padding_8[0]);
        if (v4 == *((int *)&idx->padding_8[0]) && !(v0 = a3, !v0) || (v0 = (unsigned int *)sub_46d04a(v4), v0))
        {
            if ((char)a2->field_c->field_0->field_18(idx->field_4, 0) && a2->field_c->field_0->field_8(v0, v4))
            {
                v5 = &idx->field_0->field_0;
                v6 = &idx->field_0[1].field_0;
                do
                {
                    v8 = v5;
                    v7 = _INSERT(v7, 0, *(v8));
                    v9 = v8 + 1;
                    v5 = v9;
                } while (*(v8));
                i = v9 - v6;
                v11 = &idx->field_0->field_0;
                if (v9 != v6)
                {
                    do
                    {
                        v7 = _INSERT(v7, 0, (char)v7 + *(v11));
                        i -= 1;
                        v11 += 1;
                    } while (i);
                }
                v12 = (char)v7;
                v13 = v12 & 7;
                if ((v12 & 7) < NULL)
                    v13 = (v13 - 1 | 0xfffffff8) + 1;
                v14 = (char)v13 * 6;
                sub_4639d0(v0, v4, g_4ae531[2 * v14], *((int *)(2 * v14 + 4908340)), *((int *)(2 * v14 + (char *)&g_4ae538[0])));
                if (v4 != v0)
                    v15 = sub_44c710(v0, v4, v4);
                else
                    v15 = v0;
                if (v0 == *((int *)(2 * v14 + 4908340)))
                    return v15;
                if (!v0)
                    return v15;
                sub_46c941(v0);
                return v15;
            }
        }
    }
    sub_44bcd0("info : %s error\r\n", a2->field_8);
    if (!v0)
        return NULL;
    sub_46c941(v0);
    return NULL;
}
