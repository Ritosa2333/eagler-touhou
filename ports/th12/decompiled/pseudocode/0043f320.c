// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f320; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f320_0 {
    char padding_0[1268];
    int field_4f4;
} st_43f320_0;

typedef struct st_43f320_1 {
    char padding_0[4];
    unsigned int field_4;
} st_43f320_1;

typedef struct st_43f320_2 {
    char padding_0[12];
    struct st_43f320_1 *field_c;
} st_43f320_2;

extern st_43f320_0 *g_4b44f8;
extern st_43f320_2 *g_4b4530;
extern char g_4b50c4;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee40;
extern unsigned int g_4cee78;
extern unsigned int g_4cf468;

unsigned int sub_43f320(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    unsigned int v5;  // esi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    if (sub_43f3d0())
    {
        g_4cee40 = ~(g_4cee78 >> 13) & 1 | 2;
        return 0;
    }
    if (g_4b44f8)
    {
        v1 = v3;
        v0 = v4;
        if (g_4b44f8->field_4f4 < 180)
        {
            do
            { } while (!(g_4cee78 & 384) && (Sleep(16), g_4b44f8->field_4f4 < 180));
        }
        v5 = &(&g_4b50c4)[g_4ce8cc];
        if (*((int *)&(&g_4b50c4)[g_4ce8cc]))
        {
            sub_4604e0();
            sub_46ca4f(*((int *)v5));
            *((unsigned int *)v5) = 0;
        }
    }
    g_4b4530->field_c->field_4 = g_4b4530->field_c->field_4 | 2;
    g_4cf468 = 1;
    return 0;
}
