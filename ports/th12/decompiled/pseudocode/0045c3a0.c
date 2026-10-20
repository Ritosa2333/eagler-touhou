// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45c3a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45c3a0_1 {
    unsigned int field_0;
} st_45c3a0_1;

typedef struct st_45c3a0_2 {
    char padding_0[959];
    char field_3bf;
    char padding_3c0[52];
    struct st_45c3a0_0 *field_3f4;
    char padding_3f8[132];
    unsigned int field_47c;
} st_45c3a0_2;

typedef struct st_45c3a0_0 {
    char padding_0[8];
    struct st_45c3a0_1 *field_8;
} st_45c3a0_0;

typedef struct st_45c3a0_3 {
    char padding_0[260];
    struct st_45c3a0_1 *field_104;
    char padding_108[4];
    struct st_45c3a0_1 *field_10c;
    char padding_110[60];
    struct st_45c3a0_1 *field_14c;
    char padding_150[20];
    struct st_45c3a0_1 *field_164;
} st_45c3a0_3;

typedef struct st_45c3a0_8 {
    struct st_45c3a0_3 *field_0;
} st_45c3a0_8;

extern char g_4b563c;
extern char g_4b5642;
extern char g_4b5647;
extern char g_4b56a0;
extern unsigned int g_4ce8cc;
extern st_45c3a0_8 *g_4ce8f0;

int sub_45c3a0(void)
{
    st_45c3a0_2 *v2;  // eax
    unsigned int idx;  // ecx
    unsigned int *v4;  // eax
    unsigned int v0;  // [bp+0x4]
    unsigned int v1;  // [bp+0x8]

    if (!((char)v2->field_47c & 1))
    {
        return;
    }
    else if (!((char)v2->field_47c & 2))
    {
        return;
    }
    else if (!v2->field_3bf)
    {
        return;
    }
    else
    {
        if (*((int *)&(&g_4b56a0)[idx]))
            sub_45a3c0();
        v4 = &v2->field_3f4->field_8->field_0;
        if (*((int *)&(&g_4b563c)[idx]) != v4)
        {
            *((unsigned int **)&(&g_4b563c)[idx]) = v4;
            g_4ce8f0->field_0->field_104(g_4ce8f0, 0, *(v4));
        }
        if ((&g_4b5642)[idx] != 3)
        {
            g_4ce8f0->field_0->field_164(g_4ce8f0, 324);
            g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 6, 0);
            g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 3, 0);
            (&g_4b5642)[idx] = 3;
        }
        sub_459cf0();
        if ((&g_4b5647)[g_4ce8cc] != 1)
        {
            g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 4);
            g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 4);
            (&g_4b5647)[g_4ce8cc] = 1;
        }
        g_4ce8f0->field_0->field_14c(g_4ce8f0, 5, v1 - 2, v0, 28);
        return;
    }
}
