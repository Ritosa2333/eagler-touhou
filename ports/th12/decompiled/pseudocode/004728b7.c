// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4728b7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4728b7_0 {
    char padding_0[4];
    unsigned int field_4;
} st_4728b7_0;

extern char g_4aab80;
extern st_4728b7_0 g_4b3d64;
extern st_4728b7_0 *g_4b3d68;

void sub_4728b7(unsigned int a0)
{
    void* v1;  // ebp
    st_4728b7_0 *idx;  // esi
    unsigned int v3;  // ecx
    st_4728b7_0 *v4;  // edx
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aab80, 12);
    sub_46ec9a(14);
    *((unsigned int *)((char *)v1 - 4)) = 0;
    idx = (int)v1[8];
    v3 = idx->field_4;
    if (v3)
    {
        v4 = &g_4b3d64.padding_0[0];
        while (1)
        {
            *((st_4728b7_0 **)((char *)v1 - 28)) = g_4b3d68;
            if (!g_4b3d68)
                break;
            if (g_4b3d68->padding_0 == v3)
            {
                v4->field_4 = g_4b3d68->field_4;
                sub_46c941(g_4b3d68, &g_4aab80);
                break;
            }
            else
            {
                v4 = g_4b3d68;
            }
        }
        sub_46c941(idx->field_4, &g_4aab80);
        idx->field_4 = 0;
    }
    *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
    sub_47291e();
    sub_46fd31(&g_4aab80, 12, v0, a0);
    return;
}
