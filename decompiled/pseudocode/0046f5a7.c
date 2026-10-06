// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46f5a7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46f5a7_1 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    struct st_46f5a7_0 *field_10;
    char field_14;
} st_46f5a7_1;

typedef struct st_46f5a7_0 {
    char padding_0[67];
    char field_43;
} st_46f5a7_0;

extern HANDLE g_4b3c04;
extern st_46f5a7_1 *g_4b3d58;
extern int g_4d6440;
extern unsigned int g_4d6444;
extern unsigned int g_4d6454;

st_46f5a7_1 * sub_46f5a7(void)
{
    st_46f5a7_1 *v1;  // eax
    st_46f5a7_1 *v2;  // eax

    v1 = g_4b3d58;
    if (!v1)
        return v1;
    VirtualFree(g_4d6454 * 0x8000 + v1->field_c, 0x8000, MEM_DECOMMIT);
    g_4b3d58->field_8 = g_4b3d58->field_8 | 0x80000000 >> ((char)g_4d6454 & 31);
    *((unsigned int *)&g_4b3d58->field_10[2].padding_0[60 + 4 * g_4d6454]) = 0;
    g_4b3d58->field_10->field_43 = g_4b3d58->field_10->field_43 - 1;
    v2 = g_4b3d58;
    if (!g_4b3d58->field_10->field_43)
    {
        g_4b3d58->field_4 = g_4b3d58->field_4 & 0xfffffffe;
        v2 = g_4b3d58;
    }
    if (v2->field_8 == 0xffffffff && g_4d6440 > 1)
    {
        HeapFree(g_4b3c04, HEAP_NONE, v2->field_10);
        v2 = sub_47a6a0(g_4b3d58, &g_4b3d58->field_14, 20 * g_4d6440 - (char *)g_4b3d58 + g_4d6444 - 20);
        g_4d6440 = g_4d6440 - 1;
    }
    g_4b3d58 = 0;
    return v2;
}
