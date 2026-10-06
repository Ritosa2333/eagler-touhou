// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4538e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4538e0_0 {
    char padding_0[20];
    unsigned int field_14;
} st_4538e0_0;

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

extern char g_4ceae8;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf128;
extern char g_4cf21a;
extern int g_4cf4e8;
extern unsigned int g_4cf4f8;
extern unsigned int g_4d0d68;
extern unsigned int g_4d0da8[4];
extern unsigned int g_4d0de8;
extern unsigned int g_4d0e28;
extern unsigned int g_4d0e6c;
extern char g_4d3654;
extern char g_4d4654;

int sub_4538e0(void)
{
    unsigned int v5;  // ecx
    unsigned int v6;  // ebx
    char v15;  // 4103
    unsigned int v16;  // eax
    unsigned int v17;  // 4111
    unsigned int v18;  // 4120
    char *node;  // eax
    char i;  // 4102
    int v21;  // edi
    HANDLE v22;  // esi
    int v23;  // ebp
    unsigned int v24;  // ebx
    unsigned int v7;  // esi
    unsigned int v25;  // ebp
    st_4538e0_0 *v26;  // ebx
    char *v8;  // eax
    char *v9;  // ebx
    char *v10;  // eax
    char *iter;  // ecx
    char v12;  // 4101
    unsigned int v13;  // cc_dep1
    unsigned int v14;  // cc_dep2
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]
    unsigned int idx;  // [bp+0x0]
    int index;  // [bp+0x4]

    v2 = v5;
    v1 = v6;
    v0 = v7;
    v9 = v8;
    if (g_4d0da8[index])
    {
        v10 = &(&g_4d3654)[0x100 * index];
        iter = v9;
        while (1)
        {
            v12 = *(iter);
            v13 = v12;
            v14 = *(v10);
            if (v12 != *(v10))
                break;
            if (v12)
            {
                v15 = iter[1];
                v13 = v15;
                v14 = v10[1];
                if (v15 != v10[1])
                    break;
                iter += 2;
                v10 += 2;
                if (!v15)
                    goto LABEL_45391c;
            }
            else
            {
LABEL_45391c:
                v16 = 0;
                goto LABEL_453925;
            }
        }
        v17 = _ccall(4, v13, v14, 0);
        v18 = _ccall(12, 0, v17 & 1, v17 & 1);
        v16 = -(v17 & 1) + 1 - (v18 & 1);
LABEL_453925:
        if (!v16)
            return;
    }
    node = v9;
    do
    {
        i = *(node);
        *(&(&g_4d3654)[0x100 * index] - v9 + node) = *(node);
        node += 1;
    } while (i);
    if (!(g_4ceae8 & 16))
    {
        return;
    }
    else if (!g_4cf4f8)
    {
        return;
    }
    else
    {
        if (g_4d0da8[index])
        {
            sub_46c941(g_4d0da8[index]);
            g_4d0da8[index] = 0;
        }
        sub_454b00("Streming BGM PreLoad %d\r\n", index, v21);
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf128.DebugInfo);
            g_4cf21a = g_4cf21a + 1;
        }
        v22 = CreateFileA(&g_4d4654, 0x80000000, FILE_SHARE_READ, NULL, OPEN_EXISTING, 134217856, NULL);
        if (v22 == 0xffffffff)
        {
            sub_454b00("error : bgmfile is not find %s\r\n", &g_4d4654);
            if (!(g_4cee78 & 0x8000))
                return;
            LeaveCriticalSection(&g_4cf128.DebugInfo);
            g_4cf21a = g_4cf21a - 1;
            return;
        }
        else
        {
            v24 = sub_453670(&g_4cf4e8, v23) * 52;
            SetFilePointer(v22, *((int *)(v24 + g_4d0e6c + 16)), NULL, FILE_BEGIN);
            v25 = sub_46d04a(*((int *)(v24 + g_4d0e6c + 20)));
            if (!v25)
            {
                CloseHandle(v22);
                sub_454b00("error : bgmfile is not find %s\r\n", &g_4d4654);
                sub_431800();
            }
            else
            {
                ReadFile(v22, v25, *((int *)(v24 + g_4d0e6c + 20)), &v1, NULL);
                CloseHandle(v22);
                sub_431800();
                v26 = v24 + g_4d0e6c;
                g_4d0da8[idx] = v25;
                (&g_4d0de8)[idx] = v25;
                (&g_4d0d68)[idx] = v26;
                (&g_4d0e28)[idx] = v26->field_14;
            }
            return;
        }
    }
}
