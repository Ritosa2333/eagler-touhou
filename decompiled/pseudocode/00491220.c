// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491220; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ab208;
extern unsigned int g_4ae448;
extern unsigned int g_4d52dc;

void sub_491220(unsigned int a0)
{
    void* idx;  // ebp
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4ab208, 8);
    if (g_4d52dc)
    {
        if ((char)idx[8] & 64 && g_4ae448)
        {
            *((unsigned int *)((char *)idx - 4)) = 0;
            *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
        }
        else
        {
            *((unsigned int *)&idx[8]) = (int)idx[8] & 0xffffffbf;
        }
    }
    sub_46fd31(&g_4ab208, 8, v0, a0);
    return;
}
