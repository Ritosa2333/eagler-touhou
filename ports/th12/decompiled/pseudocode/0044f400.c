// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f400; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44f400_1 {
    char padding_0[12];
    unsigned int field_c;
    unsigned int field_10;
} st_44f400_1;

typedef struct st_44f400_0 {
    char padding_0[268];
    int field_10c;
    char padding_110[16];
    unsigned int field_120;
} st_44f400_0;

typedef struct st_44f400_3 {
    unsigned int field_0;
} st_44f400_3;

typedef struct st_44f400_2 {
    char padding_0[92];
    struct st_44f400_3 *field_5c;
} st_44f400_2;

typedef struct st_44f400_4 {
    struct st_44f400_2 *field_0;
} st_44f400_4;

extern char g_4b50c0;
extern unsigned int g_4ce8cc;
extern st_44f400_4 *g_4ce8f0;
extern unsigned int g_4ce9dc;
extern unsigned int g_4ce9e0;
extern char g_4ce9e4;

void sub_44f400(void)
{
    unsigned int iter;  // ebx
    st_44f400_0 *v4;  // eax
    unsigned int v5;  // edi
    unsigned int v6;  // ebp
    st_44f400_1 *idx;  // esi
    unsigned int v0;  // [bp-0x4]
    unsigned int v1;  // [bp-0x4]

    iter = &(&g_4b50c0)[g_4ce8cc];
    v0 = 32;
    do
    {
        v1 = v0;
        v4 = *((int *)iter);
        if (v4)
        {
            v5 = 0;
            if (v4->field_10c > NULL)
            {
                v6 = 0;
                do
                {
                    idx = v4->field_120 + v6;
                    if (*((char *)(v4->field_120 + v6 + 16)) & 1)
                    {
                        idx->field_10 = idx->field_10 | 1;
                        g_4ce8f0->field_0->field_5c(g_4ce8f0, g_4ce9dc, g_4ce9e0, 1, 1, *((int *)&g_4ce9e4), 0, idx, 0);
                        idx->field_c = (*((int *)&g_4ce9e4) == 22) * 2 + 2;
                    }
                } while ((v4 = (st_44f400_0 *)*((int *)iter), v5 += 1, v6 += 20, v5 < *((int *)(*((int *)iter) + 268))));
            }
        }
    } while ((iter += 4, v0 = v1 - 1, v1 != 1));
    return;
}
