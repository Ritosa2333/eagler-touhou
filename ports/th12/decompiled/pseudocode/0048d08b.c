// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d08b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48d08b_0 {
    char padding_0[12];
    char field_c;
} st_48d08b_0;

extern int g_4ab100;
extern unsigned int g_4d52e0;
extern char g_4d6300;

void sub_48d08b(void)
{
    void* v2;  // ebp
    int v3;  // edi
    unsigned int v4;  // esi
    st_48d08b_0 **v5;  // eax
    unsigned int v0;  // [bp-0xc]

    sub_46fcec(&g_4ab100, 16);
    *((unsigned int *)((char *)v2 - 28)) = 0;
    sub_46ec9a(1);
    *((unsigned int *)((char *)v2 - 4)) = 0;
    v0 = 3;
    v3 = 3;
    while (1)
    {
        *((int *)((char *)v2 - 32)) = v3;
        if (v3 >= *((int *)&g_4d6300))
            break;
        v4 = v3 * 4;
        v5 = g_4d52e0 + v4;
        if (*(v5))
        {
            if (*(v5)->field_c & 131 && sub_48ebf2(*(v5)) != 0xffffffff)
                *((unsigned int *)((char *)v2 - 28)) = *((int *)((char *)v2 - 28)) + 1;
            if (v3 >= 20)
            {
                DeleteCriticalSection(*((int *)(v4 + g_4d52e0)) + 32);
                sub_46c941(*((int *)(v4 + g_4d52e0)));
                *((unsigned int *)(v4 + g_4d52e0)) = 0;
            }
        }
        v3 += 1;
    }
    *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
    sub_48d121();
    sub_46fd31();
    return;
}
