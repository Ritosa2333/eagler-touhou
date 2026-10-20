// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d04a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern HANDLE g_4b3c04;
extern char g_4b40bc;
extern unsigned int g_4d6458;

void* sub_46d04a(unsigned int a0)
{
    unsigned int v2;  // esi
    unsigned int v3;  // ebx
    unsigned int v4;  // edi
    void* v5;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    v2 = a0;
    if (v2 > 0xffffffe0)
    {
        sub_46ff67(v2);
        *((unsigned int *)sub_46e96f()) = 12;
        return NULL;
    }
    v1 = v3;
    v0 = v4;
    while (1)
    {
        if (!g_4b3c04)
        {
            sub_47315e();
            sub_472f8d(30);
            sub_472c61(0xff); /* do not return */
        }
        if (g_4d6458 == 1)
        {
            if (!v2)
                v2 = 1;
        }
        else
        {
            if (g_4d6458 == 3 && !(v5 = (void*)sub_46cf81(v2), !v5))
                goto LABEL_46d0c3;
            if (!v2)
                v2 = 1;
            v2 = v2 + 15 & 0xfffffff0;
            v2 = v2 + 15 & 0xfffffff0;
        }
        v5 = HeapAlloc(g_4b3c04, HEAP_NONE, v2);
LABEL_46d0c3:
        if (v5)
            break;
        v2 = 12;
        if (*((int *)&g_4b40bc) != v5)
        {
            if (!sub_46ff67(a0))
                goto LABEL_46d0f0;
            v2 = a0;
        }
        else
        {
            *((unsigned int *)sub_46e96f()) = 12;
LABEL_46d0f0:
            *((unsigned int *)sub_46e96f()) = 12;
            break;
        }
    }
    return v5;
}
