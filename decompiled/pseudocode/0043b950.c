// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43b950; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43b950_2 {
    struct st_43b950_3 *field_0;
    char padding_4[168];
    struct st_43b950_0 *field_ac;
    char padding_b0[8];
    struct st_43b950_1 *field_b8;
    char padding_bc[272];
    char field_1cc;
    char padding_1cd[3];
    int field_1d0;
    char padding_1d4[4];
    int field_1d8;
} st_43b950_2;

typedef struct st_43b950_0 {
    unsigned short field_0;
    unsigned short field_2;
    unsigned short field_4;
} st_43b950_0;

typedef struct st_43b950_1 {
    char padding_0[4];
    int field_4;
} st_43b950_1;

typedef struct st_43b950_3 {
    char field_0;
} st_43b950_3;

extern unsigned int g_4b44e8;
extern unsigned int g_4d49d0;
extern unsigned int g_4d49d4;
extern unsigned int g_4d49dc;
extern unsigned int g_4d49e0;

unsigned int sub_43b950(st_43b950_2 *idx)
{
    unsigned int v2;  // edx
    st_43b950_2 *v3;  // eax
    st_43b950_0 *v4;  // eax
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    st_43b950_2 *v0;  // [bp-0x4]

    v0 = idx;
    if (!g_4b44e8)
    {
        return 1;
    }
    else if (*((int *)&idx->padding_bc[36 * idx->field_1d8]) < 0)
    {
        g_4d49d0 = 0;
        g_4d49dc = 0;
        g_4d49e0 = 0;
        return 1;
    }
    else if (idx->field_1d8 >= 0)
    {
        g_4d49d4 = g_4d49d0;
        v2 = idx->field_1d8 * 9;
        v3 = &(&idx->field_0)[v2];
        if (*((int *)&idx->padding_bc[4 * v2]) < v3->field_b8->field_4)
        {
            v4 = v3->field_ac;
            if (v4->field_0 == 0xffff && v4->field_2 == 0xffff && v4->field_4 == 0xffff)
            {
                g_4d49d0 = 0;
                g_4d49dc = 0;
                g_4d49e0 = 0;
                sub_432850(idx);
                return 1;
            }
            g_4d49d0 = v4->field_0;
            g_4d49dc = (&idx->field_ac)[9 * idx->field_1d8]->field_2;
            g_4d49e0 = (&idx->field_ac)[9 * idx->field_1d8]->field_4;
            v5 = idx->field_1d8 * 9;
            idx->field_1cc = *((char *)*((int *)&(&idx->field_0)[9 * idx->field_1d8 + 45]));
            (&idx->field_ac)[v5] = (&idx->field_ac)[v5] + 1;
            if (!(int)(idx->field_1d0 % 30))
                *((unsigned int *)&(&idx->field_0)[9 * idx->field_1d8 + 45]) = *((int *)&(&idx->field_0)[9 * idx->field_1d8 + 45]) + 1;
        }
        else
        {
            g_4d49d0 = 0;
            g_4d49dc = 0;
            g_4d49e0 = 0;
        }
        v6 = idx->field_1d8 * 9;
        *((unsigned int *)&idx->padding_bc[4 * v6]) = *((int *)&idx->padding_bc[4 * v6]) + 1;
        idx->field_1d0 = idx->field_1d0 + 1;
        return 1;
    }
    else
    {
        g_4d49d0 = 0;
        g_4d49dc = 0;
        g_4d49e0 = 0;
        idx->field_1d0 = idx->field_1d0 + 1;
        return 1;
    }
}
