// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45d5d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45d5d0_2 {
    unsigned int field_0;
} st_45d5d0_2;

typedef struct st_45d5d0_1 {
    char padding_0[268];
    struct st_45d5d0_2 *field_10c;
    char padding_110[60];
    struct st_45d5d0_2 *field_14c;
    char padding_150[20];
    struct st_45d5d0_2 *field_164;
} st_45d5d0_1;

typedef struct st_45d5d0_5 {
    struct st_45d5d0_1 *field_0;
} st_45d5d0_5;

typedef struct st_45d5d0_0 {
    char padding_0[172];
    unsigned int field_ac;
} st_45d5d0_0;

extern char g_4b5642;
extern char g_4b5647;
extern st_45d5d0_0 *g_4ce8cc;
extern st_45d5d0_5 *g_4ce8f0;

unsigned int sub_45d5d0(unsigned int a0, void* a1, int a2)
{
    void* iter;  // eax
    unsigned int v2;  // esi
    unsigned int v3;  // ebp
    unsigned int node;  // [bp-0x4], Other Possible Types: int

    node = a0;
    iter = *((int *)&g_4ce8cc[50767].padding_0[96]);
    v2 = &g_4ce8cc[50767].padding_0[96];
    v3 = a2 * 20;
    if (*((int *)&g_4ce8cc[50767].padding_0[96]) + v3 + 20 >= v2)
        return 0;
    if (a2 > 0)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        node = a2;
        /* unsupported instruction */
        /* unsupported instruction */
        do
        {
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)iter) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            a1 += 4;
            /* unsupported instruction */
            /* unsupported instruction */
            iter += 20;
            node -= 1;
            *((int *)((char *)iter - 16)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 12)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            *((int *)((char *)iter - 4)) = *((int *)((char *)a1 - 4));
        } while (node != 1);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (*((char *)(g_4ce8cc + &g_4b5647)))
    {
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 3);
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 3);
        *((char *)(g_4ce8cc + &g_4b5647)) = 0;
    }
    if (*((char *)(g_4ce8cc + &g_4b5642)) != 1)
    {
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 6, 0);
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 3, 0);
        *((char *)(g_4ce8cc + &g_4b5642)) = 1;
    }
    g_4ce8f0->field_0->field_164(g_4ce8f0, 0x44);
    g_4ce8f0->field_0->field_14c(g_4ce8f0, 3, a2 - 1, *((int *)v2), 20);
    *((unsigned int *)v2) = *((int *)v2) + v3;
    g_4ce8cc->field_ac = g_4ce8cc->field_ac + 1;
    return 0;
}
