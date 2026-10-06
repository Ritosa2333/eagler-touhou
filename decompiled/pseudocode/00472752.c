// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472752; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_472752_0 {
    char padding_0[4];
    unsigned int field_4;
} st_472752_0;

extern int g_4aab40;
extern st_472752_0 g_4b3d64;
extern st_472752_0 *g_4b3d68;

void sub_472752(unsigned int a0)
{
    void* v0;  // ebp
    st_472752_0 *idx;  // esi
    int v2;  // ecx
    st_472752_0 *v3;  // edx

    sub_46fcec(&g_4aab40, 12);
    sub_46ec9a(14);
    *((unsigned int *)((char *)v0 - 4)) = 0;
    idx = (int)v0[8];
    v2 = idx->field_4;
    if (v2)
    {
        v3 = &g_4b3d64.padding_0[0];
        while (1)
        {
            *((st_472752_0 **)((char *)v0 - 28)) = g_4b3d68;
            if (!g_4b3d68)
                break;
            if (g_4b3d68->padding_0 == v2)
            {
                v3->field_4 = g_4b3d68->field_4;
                sub_46c941(g_4b3d68);
                break;
            }
            else
            {
                v3 = g_4b3d68;
            }
        }
        sub_46c941(idx->field_4);
        idx->field_4 = 0;
    }
    *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
    sub_4727b9();
    sub_46fd31();
    return;
}
