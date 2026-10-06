// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4778ff; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46edc8_0 {
    char padding_0[12];
    unsigned int field_c;
} st_46edc8_0;

extern unsigned int g_4aadf8;
extern HANDLE g_4b3c04;
extern unsigned int g_4d6458;

int sub_4778ff(unsigned int a0)
{
    void* v0;  // ebp
    void* v1;  // ebx
    st_46edc8_0 *v2;  // eax

    sub_46fcec(&g_4aadf8, 16);
    v1 = (int)v0[8];
    if (!v1)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        if (g_4d6458 == 3)
        {
            sub_46ec9a(4);
            *((unsigned int *)((char *)v0 - 4)) = 0;
            v2 = sub_46edc8(v1);
            *((st_46edc8_0 **)((char *)v0 - 32)) = v2;
            if (v2)
                *((unsigned int *)((char *)v0 - 28)) = *((int *)((char *)v1 - 4)) - 9;
            *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
            sub_477999();
            if (*((int *)((char *)v0 - 32)))
                goto LABEL_477989;
        }
        HeapSize(g_4b3c04, HEAP_NONE, v1);
LABEL_477989:
    }
    return sub_46fd31();
}
