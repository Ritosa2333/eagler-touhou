// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422368; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_422368_9 {
    char padding_0[12];
    unsigned int field_c;
} st_422368_9;

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

typedef struct st_422368_1 {
    char padding_0[4];
    unsigned int field_4;
} st_422368_1;

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

typedef struct st_422368_0 {
    char padding_0[12];
    struct st_422368_1 *field_c;
    char padding_10[452];
    struct st_422368_1 *field_1d4;
} st_422368_0;

typedef struct st_422368_3 {
    char padding_0[8];
    struct st_422368_1 *field_8;
    struct st_422368_1 *field_c;
} st_422368_3;

extern char g_4b0cb0;
extern unsigned int g_4b0cb4;
extern void g_4b0ce0;
extern unsigned int g_4b43bc;
extern char g_4b43c0;
extern char g_4b43c4;
extern st_422368_3 *g_4b43c8;
extern char g_4b43cc;
extern char g_4b43d4;
extern char g_4b43dc;
extern char g_4b43e4;
extern unsigned int g_4b44e8;
extern unsigned int g_4b44f0;
extern char g_4b44f4;
extern st_422368_3 *g_4b4510;
extern char g_4b4514;
extern st_422368_0 *g_4b4518;
extern char g_4b4524;
extern char g_4b4534;
extern unsigned int g_4ce55c;
extern char g_4ceae8;
extern unsigned int g_4cee40;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;
extern unsigned int g_4cf2a8;

void sub_422368(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    char v3;  // bl
    int v4;  // ebp
    int v5;  // esi
    unsigned int v6;  // 4108
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]
    st_422368_9 *v2;  // [bp+0x14]

    if (/* unsupported instruction */)
    {
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
        /* unsupported instruction */
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
    if (/* unsupported instruction */)
    {
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    sub_411b80(v0, v1);
    if (g_4b0cb4 == *((int *)&g_4b0cb0))
        *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) | 1;
    if (!(v3 & g_4b0ce0))
    {
        if (g_4cee40 != 15 && g_4cee40 != 16 && g_4b4518 != v4)
        {
            sub_43b450(g_4b4518);
            sub_46ca4f(g_4b4518);
        }
        if (*((int *)&g_4b43c0) != v4)
        {
            sub_402cc0(*((int *)&g_4b43c0));
            sub_46ca4f(*((int *)&g_4b43c0));
        }
        if (g_4b43bc != v4)
        {
            sub_402cc0(g_4b43bc);
            sub_46ca4f(g_4b43bc);
        }
        if (g_4b4510 != v4)
        {
            sub_431ed0();
            sub_46ca4f(g_4b4510);
        }
        if (*((int *)&g_4b43e4) != v4)
        {
            sub_41dc50(*((int *)&g_4b43e4));
            sub_46ca4f(*((int *)&g_4b43e4));
        }
        if (*((int *)&g_4b4514) != v4)
        {
            sub_436270(*((int *)&g_4b4514));
            sub_46ca4f(*((int *)&g_4b4514));
        }
        if (g_4b43c8 != v4)
        {
            sub_4096d0(g_4b43c8);
            sub_46ca4f(g_4b43c8);
        }
        if (g_4b44f0 != v4)
        {
            sub_425a00(g_4b44f0);
            sub_46ca4f(g_4b44f0);
        }
        if (*((int *)&g_4b44f4) != v4)
        {
            sub_4280d0();
            sub_46ca4f(*((int *)&g_4b44f4));
        }
        if (*((int *)&g_4b4524) != v4)
        {
            sub_43dc30(*((int *)&g_4b4524));
            sub_46ca4f(*((int *)&g_4b4524));
        }
        if (*((int *)&g_4b4534) != v4)
        {
            sub_44a120();
            sub_46ca4f(*((int *)&g_4b4534));
        }
    }
    else
    {
        sub_41dbc0();
        v5 = g_4b43bc;
        if (v5 != v4)
        {
            sub_402cc0(v5);
            sub_46ca4f(v5);
        }
        g_4b43bc = *((int *)&g_4b43c0);
        if (g_4b4510->field_8 != v4)
            g_4b4510->field_8->field_4 = g_4b4510->field_8->field_4 & 0xfffffffd;
        if (g_4b4510->field_c != v4)
            g_4b4510->field_c->field_4 = g_4b4510->field_c->field_4 & 0xfffffffd;
        if (g_4b43c8->field_8 != v4)
            g_4b43c8->field_8->field_4 = g_4b43c8->field_8->field_4 & 0xfffffffd;
        if (g_4b43c8->field_c != v4)
            g_4b43c8->field_c->field_4 = g_4b43c8->field_c->field_4 & 0xfffffffd;
        if (g_4b4518->field_1d4 != v4)
            g_4b4518->field_1d4->field_4 = g_4b4518->field_1d4->field_4 & 0xfffffffd;
        if (g_4b4518->field_c != v4)
            g_4b4518->field_c->field_4 = g_4b4518->field_c->field_4 & 0xfffffffd;
        sub_477420(g_4b44f0 + 20, v4, 6713280);
        *((int *)(g_4b44f0 + 6713304)) = v4;
        *((int *)(g_4b44f0 + 6713312)) = v4;
    }
    if (g_4b0ce0 & 9)
    {
        sub_40e940(*((int *)&g_4b43dc));
    }
    else if (*((int *)&g_4b43dc) != v4)
    {
        sub_412f10();
        sub_46ca4f(*((int *)&g_4b43dc));
    }
    if (*((int *)&g_4b43d4) != v4)
    {
        sub_40fa30();
        sub_46ca4f(*((int *)&g_4b43d4));
    }
    if (*((int *)&g_4b43c4) != v4)
    {
        sub_406930(*((int *)&g_4b43c4));
        sub_46ca4f(*((int *)&g_4b43c4));
    }
    if (*((int *)&g_4b43cc) != v4)
    {
        sub_40d9c0();
        sub_46ca4f(*((int *)&g_4b43cc));
    }
    if (*((int *)&v2->padding_0[8]) != v4)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (v2->field_c)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    v6 = g_4b0ce0 & 0x22;
    g_4b44e8 = 0;
    if (!(char)v6)
    {
        v1 = 0;
        v0 = (!(g_4ceae8 & 16) ? 3 : 4);
        sub_454960((!(g_4ceae8 & 16) ? 3 : 4));
    }
    sub_423020();
    g_4cf2a8 = (-(0 < (g_4b0ce0 & 1)) & 0x1000000) - 0x1000000;
    g_4ce55c = 1;
    return;
}
