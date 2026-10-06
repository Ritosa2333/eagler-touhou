// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ed7a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern HANDLE g_4b3c04;
extern unsigned int g_4b3d58;
extern unsigned int g_4d6440;
extern unsigned int g_4d6444;
extern unsigned int g_4d6448;
extern unsigned int g_4d644c;
extern unsigned int g_4d6450;

unsigned int sub_46ed7a(unsigned int a0)
{
    g_4d6444 = HeapAlloc(g_4b3c04, HEAP_NONE, 0x140);
    if (g_4d6444)
    {
        g_4b3d58 = 0;
        g_4d6440 = 0;
        g_4d644c = g_4d6444;
        g_4d6448 = a0;
        g_4d6450 = 16;
        return 1;
    }
    return 0;
}
