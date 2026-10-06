// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ea8c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46ea8c_0 {
    char padding_0[12];
    void* field_c;
    void* field_10;
    char padding_14[16];
    char field_24;
} st_46ea8c_0;

extern HANDLE g_4b3c04;
extern char g_4d6440;
extern st_46ea8c_0 *g_4d6444;
extern unsigned int g_4d6458;

int sub_46ea8c(void)
{
    unsigned int v3;  // ebx
    int i;  // ebx
    unsigned int v5;  // edi
    unsigned int v6;  // esi
    int v7;  // eax
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    if (g_4d6458 == 3)
    {
        v1 = v3;
        i = 0;
        v0 = v5;
        if (*((int *)&g_4d6440) > 0)
        {
            v6 = &g_4d6444->field_10;
            do
            {
                VirtualFree(*((int *)(v6 - 4)), NULL, MEM_RELEASE);
                HeapFree(g_4b3c04, HEAP_NONE, *((int *)v6));
                v6 += 20;
                i += 1;
            } while (i < *((int *)&g_4d6440));
        }
        HeapFree(g_4b3c04, HEAP_NONE, g_4d6444);
    }
    v7 = HeapDestroy(g_4b3c04);
    g_4b3c04 = 0;
    return v7;
}
