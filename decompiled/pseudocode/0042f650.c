// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f650; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42f650_4 {
    char padding_0[8];
    struct st_42f650_1 *field_8;
    char padding_c[20];
    struct st_42f650_1 *field_20;
} st_42f650_4;

typedef struct st_42f650_10 {
    char padding_0[12];
    struct st_42f650_11 *field_c;
    char padding_10[16];
    struct st_42f650_11 *field_20;
    struct st_42f650_11 *field_24;
} st_42f650_10;

typedef struct st_42f650_11 {
    struct st_42f650_0 *field_0;
} st_42f650_11;

typedef struct st_42f650_1 {
    unsigned int field_0;
} st_42f650_1;

typedef struct st_42f650_14 {
    char padding_0[1144];
    int field_478;
} st_42f650_14;

typedef struct st_42f650_0 {
    char padding_0[8];
    struct st_42f650_1 *field_8;
} st_42f650_0;

extern int g_4b43e0;
extern HGDIOBJ g_4b453c;
extern char g_4b564c;
extern HGDIOBJ g_4cc54c;
extern HGDIOBJ g_4ce550;
extern HGDIOBJ g_4ce554;
extern unsigned int g_4ce8cc;
extern st_42f650_14 *g_4ceaa4;
extern st_42f650_14 *g_4ceaa8;
extern st_42f650_14 *g_4ceaac;
extern int g_4cf28c;
extern unsigned int g_4d4770;

unsigned int sub_42f650(void)
{
    int v1;  // edi
    int v2;  // esi
    st_42f650_14 *v11;  // eax
    unsigned int v12;  // esi
    st_42f650_14 *v13;  // eax
    unsigned int v14;  // esi
    st_42f650_14 *v15;  // eax
    unsigned int v16;  // esi
    int v3;  // ebp
    st_42f650_0 **v4;  // eax
    st_42f650_10 *idx;  // ebx
    st_42f650_4 **v6;  // eax
    st_42f650_4 **v7;  // eax
    st_42f650_4 **v8;  // eax
    st_42f650_4 **v9;  // eax
    unsigned int v10;  // eax

    do
    { } while (sub_453fa0(v1, v2, v3));
    g_4d4770 = 2;
    sub_464c40();
    if (g_4cf28c)
    {
        sub_46c941(g_4cf28c);
        g_4cf28c = 0;
    }
    sub_42f830();
    if (g_4b43e0)
    {
        sub_41ca70();
        sub_46ca4f(g_4b43e0);
    }
    v4 = *((int *)&(&g_4b564c)[g_4ce8cc]);
    if (v4)
    {
        *(v4)->field_8(v4);
        *((unsigned int *)&(&g_4b564c)[g_4ce8cc]) = 0;
    }
    sub_454960(4, 0);
    sub_44d5b0(4, 0);
    DeleteObject(g_4ce554);
    DeleteObject(g_4cc54c);
    DeleteObject(g_4ce550);
    DeleteObject(g_4b453c);
    v6 = idx->field_20;
    if (v6)
    {
        *(v6)->field_20(v6);
        v7 = idx->field_20;
        if (v7)
        {
            *(v7)->field_8(v7);
            idx->field_20 = NULL;
        }
    }
    v8 = idx->field_24;
    if (v8)
    {
        *(v8)->field_20(v8);
        v9 = idx->field_24;
        if (v9)
        {
            *(v9)->field_8(v9);
            idx->field_24 = 0;
        }
    }
    v10 = idx->field_c;
    if (v10)
    {
        (*((int *)(*((int *)v10) + 8)))(v10);
        idx->field_c = 0;
    }
    sub_44b710();
    v11 = g_4ceaa4;
    if (g_4ceaa4)
    {
        v12 = &g_4ceaa4->field_478;
        if (*((int *)v12))
            sub_46c941(*((int *)v12));
        *((int *)v12) = 0;
        sub_46ca4f(v11);
    }
    v13 = g_4ceaa8;
    g_4ceaa4 = 0;
    if (g_4ceaa8)
    {
        v14 = &g_4ceaa8->field_478;
        if (*((int *)v14))
            sub_46c941(*((int *)v14));
        *((int *)v14) = 0;
        sub_46ca4f(v13);
    }
    v15 = g_4ceaac;
    g_4ceaa8 = 0;
    if (g_4ceaac)
    {
        v16 = &g_4ceaac->field_478;
        if (*((int *)v16))
            sub_46c941(*((int *)v16));
        *((int *)v16) = 0;
        sub_46ca4f(v15);
    }
    g_4ceaac = 0;
    return 0;
}
