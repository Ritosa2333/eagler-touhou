// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46f10e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46f10e_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    void* field_c;
    struct st_46f10e_1 *field_10;
} st_46f10e_0;

typedef struct st_46f10e_1 {
    unsigned int field_0;
} st_46f10e_1;

extern HANDLE g_4b3c04;
extern unsigned int g_4d6440;
extern void* g_4d6444;
extern unsigned int g_4d6450;

st_46f10e_0 * sub_46f10e(void)
{
    unsigned int v1;  // esi
    void* ptr;  // eax
    st_46f10e_0 *idx;  // esi
    unsigned int *v4;  // eax
    void* v5;  // eax

    v1 = g_4d6440;
    if (g_4d6440 == g_4d6450)
    {
        ptr = HeapReAlloc(g_4b3c04, HEAP_NONE, g_4d6444, (g_4d6450 + 16) * 20);
        if (!ptr)
            return NULL;
        g_4d6450 = g_4d6450 + 16;
        v1 = g_4d6440;
        g_4d6444 = ptr;
    }
    idx = v1 * 20 + g_4d6444;
    v4 = HeapAlloc(g_4b3c04, HEAP_ZERO_MEMORY, 0x41c4);
    idx->field_10 = v4;
    if (!v4)
        return NULL;
    v5 = VirtualAlloc(NULL, 0x100000, MEM_RESERVE, PAGE_READWRITE);
    idx->field_c = v5;
    if (v5)
    {
        idx->field_8 = 0xffffffff;
        idx->field_0 = 0;
        idx->field_4 = 0;
        g_4d6440 = g_4d6440 + 1;
        idx->field_10->field_0 = 0xffffffff;
        return idx;
    }
    HeapFree(g_4b3c04, HEAP_NONE, idx->field_10);
    return NULL;
}
