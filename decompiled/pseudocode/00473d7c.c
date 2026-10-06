// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473d7c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_473d7c_0 {
    char padding_0[112];
    char field_70;
} st_473d7c_0;

extern int g_4ad4b0;
extern char g_4ad6d0;
extern char g_4ad7d8;
extern int g_4ad8d8;
extern char g_4ad9d4;
extern unsigned short g_4b40c4;
extern unsigned int g_4b40d0;
extern unsigned int g_4b40d4;
extern unsigned int g_4b40d8;

int sub_473d7c(void)
{
    unsigned int *idx;  // ebx
    st_473d7c_0 *v3;  // esi
    void* v4;  // ebp
    int index;  // eax
    int iter;  // eax
    int node;  // eax
    int v0;  // [bp+0x0]

    InterlockedIncrement(idx);
    if (v3->field_70 & 2)
        goto LABEL_0x473e79;
    if (g_4ad9d4 & 1)
        goto LABEL_0x473e79;
    sub_46ec9a(13);
    *((unsigned int *)((char *)v4 - 4)) = 0;
    g_4b40d0 = idx[1];
    g_4b40d4 = idx[2];
    g_4b40d8 = idx[3];
    index = 0;
    while (1)
    {
        *((int *)((char *)v4 - 28)) = index;
        if (index >= 5)
            break;
        (&g_4b40c4)[index] = *((short *)((char *)idx + 2 * index + 16));
        index += 1;
    }
    iter = 0;
    while (1)
    {
        *((int *)((char *)v4 - 28)) = iter;
        if (iter >= 0x101)
            break;
        (&g_4ad6d0)[iter] = *(iter + (char *)idx + 28);
        iter += 1;
    }
    node = 0;
    while (1)
    {
        *((int *)((char *)v4 - 28)) = node;
        if (node >= 0x100)
            break;
        (&g_4ad7d8)[node] = *(node + (char *)idx + 285);
        node += 1;
    }
    if (!InterlockedDecrement(g_4ad8d8) && g_4ad8d8 != &g_4ad4b0)
        sub_46c941(g_4ad8d8);
    g_4ad8d8 = idx;
    InterlockedIncrement(idx);
    *((unsigned int *)((char *)v4 - 4)) = 0xfffffffe;
    sub_473e49(v0);
}
