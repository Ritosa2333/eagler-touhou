// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b606; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ab018;
extern HANDLE g_4b3c04;
extern unsigned int g_4b40bc;
extern char g_4d6448;
extern unsigned int g_4d6458;

int sub_48b606(unsigned int a0, unsigned int a1)
{
    void* idx;  // ebp
    void* v1;  // ebx
    unsigned int **v10;  // eax
    unsigned int **v11;  // eax
    unsigned int ptr;  // eax
    unsigned int v2;  // esi
    int v3;  // eax
    void* v4;  // eax
    unsigned int v5;  // eax
    int v6;  // eax
    void* v7;  // eax
    unsigned int v8;  // eax
    void* ptr1;  // edi

    sub_46fcec(&g_4ab018, 16);
    v1 = (int)idx[8];
    if (!v1)
    {
        sub_46d04a((int)idx[12]);
    }
    else
    {
        v2 = (int)idx[12];
        if (!v2)
        {
            sub_46c941(v1);
            goto LABEL_48b7f1;
        }
        else if (g_4d6458 == 3)
        {
            while (1)
            {
                *((void* *)((char *)idx - 28)) = NULL;
                if (v2 > 0xffffffe0)
                    goto LABEL_48b7df;
                sub_46ec9a(4);
                *((unsigned int *)((char *)idx - 4)) = 0;
                v3 = sub_46edc8(v1);
                *((int *)((char *)idx - 32)) = v3;
                if (v3)
                {
                    if (v2 <= *((int *)&g_4d6448))
                    {
                        if (sub_46f2c6(v3, v1, v2))
                        {
                            *((void* *)((char *)idx - 28)) = v1;
                        }
                        else
                        {
                            v4 = sub_46fa07(v2);
                            *((void* *)((char *)idx - 28)) = v4;
                            if (v4)
                            {
                                v5 = *((int *)((char *)v1 - 4)) - 1;
                                if (v5 >= v2)
                                    v5 = v2;
                                sub_47a330(*((int *)((char *)idx - 28)), v1, v5);
                                v6 = sub_46edc8(v1);
                                *((int *)((char *)idx - 32)) = v6;
                                sub_46edf8(v6, v1);
                            }
                        }
                    }
                    if (!*((int *)((char *)idx - 28)))
                    {
                        if (!v2)
                        {
                            v2 = 1;
                            *((unsigned int *)&idx[12]) = 1;
                        }
                        v2 = v2 + 15 & 0xfffffff0;
                        *((unsigned int *)&idx[12]) = v2;
                        v7 = HeapAlloc(g_4b3c04, HEAP_NONE, v2);
                        *((void* *)((char *)idx - 28)) = v7;
                        if (v7)
                        {
                            v8 = *((int *)((char *)v1 - 4)) - 1;
                            if (v8 >= v2)
                                v8 = v2;
                            sub_47a330(*((int *)((char *)idx - 28)), v1, v8);
                            sub_46edf8(*((int *)((char *)idx - 32)), v1);
                        }
                    }
                }
                *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                sub_48b74a();
                if (!*((int *)((char *)idx - 32)))
                {
                    if (!v2)
                        v2 = 1;
                    v2 = v2 + 15 & 0xfffffff0;
                    *((unsigned int *)&idx[12]) = v2;
                    ptr1 = HeapReAlloc(g_4b3c04, HEAP_NONE, v1, v2);
                }
                else
                {
                    ptr1 = *((int *)((char *)idx - 28));
                }
                if (ptr1)
                    break;
                if (!g_4b40bc)
                {
                    if (!1)
                        break;
                    v11 = sub_46e96f();
                    if (*((int *)((char *)idx - 32)))
                    {
                        *(v11) = 12;
                        break;
                    }
                    goto LABEL_48b80c;
                }
                if (!sub_46ff67(v2))
                {
                    v10 = sub_46e96f();
                    if (*((int *)((char *)idx - 32)))
                        goto LABEL_48b7eb;
                    goto LABEL_48b77f;
                }
            }
        }
        while (1)
        {
            if (v2 <= 0xffffffe0)
            {
                if (!v2)
                    v2 = 1;
                ptr = HeapReAlloc(g_4b3c04, HEAP_NONE, v1, v2);
                if (ptr)
                    goto LABEL_48b81d;
                if (g_4b40bc == ptr)
                {
                    if (!1)
                        goto LABEL_48b81d;
                    v11 = sub_46e96f();
LABEL_48b80c:
                    *(v11) = sub_46e92d(GetLastError());
LABEL_48b81d:
                    break;
                }
                if (!sub_46ff67(v2))
                {
                    v10 = sub_46e96f();
LABEL_48b77f:
                    *(v10) = sub_46e92d(GetLastError());
LABEL_48b7f1:
                    break;
                }
            }
            else
            {
LABEL_48b7df:
                sub_46ff67(v2);
                v10 = sub_46e96f();
LABEL_48b7eb:
                *(v10) = 12;
                goto LABEL_48b7f1;
            }
        }
    }
    return sub_46fd31();
}
