// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42e9b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42e9b0_0 {
    char padding_0[4];
    unsigned int field_4;
} st_42e9b0_0;

typedef struct st_42e9b0_2 {
    char padding_0[8];
    struct st_42e9b0_0 *field_8;
    struct st_42e9b0_0 *field_c;
    char padding_10[1240];
    unsigned int field_4e8;
    unsigned int field_4ec;
    unsigned int field_4f0;
} st_42e9b0_2;

extern int g_4a0778;
extern int g_4a07c0;
extern st_42e9b0_2 *g_4b44f8;
extern char g_4ceae8;
extern unsigned int g_4cee40;
extern unsigned int g_4cee70;
extern unsigned int g_4d0e6c;
extern unsigned int g_4d4654;
extern unsigned int g_4d4658;
extern unsigned short g_4d465c;

unsigned int sub_42e9b0(int a0)
{
    int v1;  // edi
    int v2;  // esi
    int v3;  // ebx
    unsigned int v4;  // eax
    int v6;  // esi

    v4 = sub_45fe60(v1, v2, v3, a0);
    g_4b44f8->field_4e8 = v4;
    if (v4)
    {
        g_4b44f8->field_c->field_4 = g_4b44f8->field_c->field_4 | 2;
        g_4b44f8->field_4ec = 1;
        v6 = (!sub_46c9ea(102340) ? 0 : sub_401000());
        if (sub_401090())
        {
            if (v6)
            {
                sub_401220(v6);
                sub_46ca4f(v6);
            }
LABEL_42ea2a:
            sub_464220(&g_4a0778);
        }
        else
        {
            if (!v6)
                goto LABEL_42ea2a;
            g_4b44f8->field_4f0 = 1;
            g_4cee70 = sub_45fe60();
            if (g_4cee70)
            {
                g_4d0e6c = sub_463c10(0, 0);
                if (!g_4d0e6c)
                    sub_464220(&g_4a07c0);
                sub_453cd0();
                if (sub_463df0("thbgm.da"))
                {
                    if (!(g_4ceae8 & 16))
                    {
                        sub_4537b0();
                    }
                    else
                    {
                        g_4d4654 = 1734502516;
                        g_4d4658 = 1633955437;
                        g_4d465c = 116;
                    }
                }
                sub_42e8f0();
                g_4b44f8->field_8->field_4 = g_4b44f8->field_8->field_4 | 2;
                sub_43d050();
                return 0;
            }
        }
    }
    g_4cee40 = 3;
    g_4b44f8->field_8->field_4 = g_4b44f8->field_8->field_4 | 2;
    return 0;
}
