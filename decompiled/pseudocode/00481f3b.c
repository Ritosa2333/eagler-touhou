// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x481f3b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_481f3b_1 {
    struct st_481f3b_0 *field_0;
} st_481f3b_1;

typedef struct st_481f3b_0 {
    unsigned int field_0;
} st_481f3b_0;

typedef struct st_481f3b_4 {
    struct st_481f3b_1 *field_0;
    unsigned int field_4;
} st_481f3b_4;

typedef struct st_481f3b_10 {
    char field_0;
    char field_1;
} st_481f3b_10;

typedef struct st_481f3b_11 {
    char field_0;
} st_481f3b_11;

extern st_481f3b_0 *g_4b42f8;
extern st_481f3b_11 *g_4b4318;
extern st_481f3b_10 *g_4b431c;
extern unsigned int g_4b4320;
extern int g_4b4324;
extern unsigned int g_4b4328;

unsigned int sub_481f3b(void)
{
    st_481f3b_1 **v5;  // ecx
    unsigned int v6;  // eax
    st_481f3b_4 *v7;  // eax
    st_481f3b_4 *v8;  // eax
    unsigned int v9;  // eax
    unsigned int node;  // eax
    unsigned int iter;  // edx
    char v12;  // cl
    char v0;  // [bp-0x1c]
    char v1;  // [bp-0x14]
    st_481f3b_1 **v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]

    v3 &= 0xffff0000;
    v5 = NULL;
    v2 = NULL;
    v6 = v3 & 0xffff0000;
    if (g_4b431c)
    {
        if (g_4b431c->field_0 == 63)
        {
            if (g_4b431c->field_1 == 64)
            {
                g_4b4318 = g_4b4318 + 2;
                v7 = sub_47ec4a(&v0, "CV: ", sub_481298(&v1));
                goto LABEL_481fbd;
            }
            else if (g_4b431c->field_1 == 36)
            {
                v8 = sub_4800f3(&v0, 0);
                v5 = v8->field_0;
                v6 = v8->field_4;
                if ((char)v6 != 2)
                    goto LABEL_481fc2;
                g_4b4318 = g_4b431c;
            }
        }
        v7 = sub_481298(&v0);
LABEL_481fbd:
        v5 = v7->field_0;
        v6 = v7->field_4;
    }
LABEL_481fc2:
    if ((char)v6 == 3)
        return 0;
    if ((char)v6 != 2 && (g_4b4328 & 0x1000 || !g_4b4318->field_0))
    {
        v2 = v5;
        v3 = v6;
    }
    else
    {
        sub_47e896(g_4b431c);
    }
    v9 = g_4b4320;
    if (!g_4b4320)
    {
        if (v2)
            v9 = *(v2)->field_0();
        g_4b4324 = v9 + 1;
        v9 = g_4b42f8(g_4b4324 + 7 & 0xfffffff8);
        g_4b4320 = v9;
        if (!v9)
            return v9;
    }
    sub_47e213(v9, g_4b4324);
    node = g_4b4320;
    iter = g_4b4320;
    while (1)
    {
        v12 = *((char *)node);
        if (!*((char *)node))
            break;
        if (v12 == 32)
        {
            node += 1;
            *((char *)iter) = 32;
            for (iter += 1; *((char *)node) == 32; node += 1);
        }
        else
        {
            *((char *)iter) = v12;
            iter += 1;
            node += 1;
        }
    }
    *((char *)iter) = v12;
    return g_4b4320;
}
