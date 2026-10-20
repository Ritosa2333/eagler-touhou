// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4508b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct CRITICAL_SECTION_DEBUG {
    unsigned short Type;
    unsigned short CreatorBackTraceIndex;
    struct CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    unsigned int EntryCount;
    unsigned int ContentionCount;
    unsigned int Flags;
    unsigned short CreatorBackTraceIndexHigh;
    unsigned short Identifier;
} CRITICAL_SECTION_DEBUG;

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf170;
extern char g_4cf21d;
extern unsigned int g_4cf408;
extern unsigned int g_4cf40c;
extern char g_4cf410;
extern unsigned int g_4cf414;
extern unsigned long long g_4cf448;

int sub_4508b0(void)
{
    unsigned short v4;  // ftop
    unsigned short v5;  // ftop
    unsigned short v14;  // ftop
    unsigned short v15;  // ftop
    unsigned short v16;  // ftop
    unsigned int v18;  // eax
    unsigned short v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned int v9;  // fc3210
    unsigned int v10;  // 4156
    unsigned int v11;  // eax
    unsigned short v12;  // ftop
    unsigned short v13;  // ftop
    unsigned int t;  // [bp-0x14], Other Possible Types: unsigned long
    unsigned int v1;  // [bp-0x10]
    long long v2;  // [bp-0xc]

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf170.DebugInfo);
        g_4cf21d = g_4cf21d + 1;
    }
    if (g_4cf408 || g_4cf40c)
    {
        QueryPerformanceCounter(&v2);
        t = (unsigned int)v2 - *((int *)&g_4cf410);
        v1 = *((unsigned int *)((void*)&v2 + 4)) - g_4cf414 - ((unsigned int)v2 < *((int *)&g_4cf410));
        v5 = v4 - 1;
        if (/* unsupported instruction */)
        {
            v6 = v5 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v6 = v5 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v7 = v6 - 1;
        if (/* unsupported instruction */)
        {
            v8 = v7 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v8 = v7 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        t = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v9 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4cf448) * 0x100 & 0x4500;
        v10 = _ccall(10, 13, (char)(((v8 + 1 & 7) * 0x800 | (unsigned short)v9 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v10 & 1))
        {
            if (/* unsupported instruction */)
                g_4cf448 = /* unsupported instruction */;
            else
                g_4cf448 = nan;
        }
        if (g_4cee78 & 0x8000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v11 = (unsigned int)LeaveCriticalSection(&g_4cf170.DebugInfo);
            if (/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                g_4cf21d = g_4cf21d - 1;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                g_4cf21d = g_4cf21d - 1;
            }
        }
        if (!/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            return v11;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        return v11;
    }
    else
    {
        t = timeGetTime();
        v13 = v12 - 1;
        if (/* unsupported instruction */)
        {
            v14 = v13 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v14 = v13 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (t < 0)
        {
            if (/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        v15 = v14 - 1;
        if (/* unsupported instruction */)
        {
            v16 = v15 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v16 = v15 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (!((char)(((v16 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4cf448 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        t = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (g_4cee78 & 0x8000)
        {
            v18 = (unsigned int)LeaveCriticalSection(&g_4cf170.DebugInfo);
            g_4cf21d = g_4cf21d - 1;
        }
        if (!/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            return v18;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        return v18;
    }
}
