// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44af00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44af00_2 {
    char padding_0[2504];
    unsigned int field_9c8;
    struct st_44af00_2 *field_9cc;
    unsigned int field_9d0;
    unsigned int field_9d4;
} st_44af00_2;

typedef struct st_44af00_0 {
    char padding_0[304];
    unsigned int field_130;
} st_44af00_0;

typedef struct st_44af00_3 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_44af00_3;

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

typedef struct st_44af00_1 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    char padding_38[24];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    unsigned int field_5c;
    unsigned int field_60;
    char padding_64[4];
    unsigned int field_68;
} st_44af00_1;

extern int g_4b0c48;
extern unsigned int g_4b0c54;
extern int g_4b0c78;
extern char g_4b0cd0;
extern unsigned int g_4b43c8;
extern st_44af00_1 *g_4b4534;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_44af00(void)
{
    int v18;  // edi
    int v19;  // esi
    st_44af00_2 *idx;  // edi
    st_44af00_2 *i;  // edi
    unsigned int v30;  // ftop
    unsigned int v31;  // eax
    unsigned int v32;  // ftop
    unsigned int v33;  // eax
    unsigned int v34;  // edx
    unsigned int v35;  // ebx
    unsigned int v36;  // eax
    unsigned int v37;  // fc3210
    int v20;  // ebx
    unsigned short v38;  // ftop
    unsigned int v39;  // 4144
    unsigned int v40;  // fc3210
    unsigned int v41;  // 4146
    unsigned int v42;  // edx
    unsigned int v43;  // eax
    unsigned int v44;  // edx
    unsigned int v45;  // eax
    unsigned int v46;  // eax
    unsigned int v47;  // eax
    unsigned int v21;  // ecx
    unsigned int v48;  // eax
    unsigned short v49;  // ftop
    unsigned int v50;  // fc3210
    unsigned short v51;  // ftop
    unsigned int v52;  // 4144
    unsigned int v53;  // fc3210
    unsigned int v54;  // 4146
    unsigned int v55;  // eax
    unsigned int v56;  // edx
    unsigned int v57;  // eax
    st_44af00_0 *idx1;  // ebp
    unsigned int v23;  // edi
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    st_44af00_3 *index;  // ebx
    unsigned int v27;  // ftop
    int v0;  // [bp-0xc0], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0xbc]
    unsigned int v2;  // [bp-0xb8]
    unsigned int v3;  // [bp-0xb4]
    unsigned int v4;  // [bp-0xb0]
    unsigned int v5;  // [bp-0x94]
    unsigned int v6;  // [bp-0x90]
    unsigned int v7;  // [bp-0x78]
    unsigned int v8;  // [bp-0x74]
    unsigned int v9;  // [bp-0x70]
    unsigned int v10;  // [bp-0x6c]
    char v11;  // [bp-0x54], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x50]
    unsigned int v13;  // [bp-0x48]
    unsigned int v14;  // [bp-0x44]
    unsigned int v15;  // [bp-0x40]
    char v16;  // [bp-0x4]

    sub_477420(&v11, 0, 80, v18, v19, &v16, v20);
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v15 = 3000;
    v14 = 0;
    v13 = 0;
    sub_412990("UFO_EtBreak2", &v11);
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = v21;
    /* unsupported instruction */
    sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    idx1 = *((int *)(g_4b43c8 + 5106652));
    v23 = g_4b4534->field_34 + 4212;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx1->field_130 = idx1->field_130 + 1;
    v25 = v24 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    index = sub_4621c0();
    index->field_480 = index->field_480 | 1;
    index->field_20 = 23;
    if (!v23)
    {
        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v27 = v25 + 1;
        index->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        index->field_434 = v8;
        index->field_438 = v9;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v27 = v25 + 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    v5 = 208;
    sub_454d10(index, 208);
    sub_461250(index, 208);
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx = 0xa;
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    do
    {
        i = idx;
        sub_40fbe0(&v5, 1, &v19);
        idx = (char *)i - 1;
    } while (i != 1);
    v30 = v27 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    if (*((int *)&g_4b4534->padding_38[20]))
    {
        if (*((int *)&g_4b4534->padding_38[16]) == idx)
            goto LABEL_44b0e6;
        /* unsupported instruction */
        /* unsupported instruction */
        v31 = g_4b4534->field_34 + 4212;
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v32 = v30 + 1;
        v1 = g_4b4534->field_34 + 4212;
        v0 = 18;
    }
    else if (*((int *)&g_4b4534->padding_38[16]))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v31 = g_4b4534->field_34 + 4212;
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v32 = v30 + 1;
        v1 = g_4b4534->field_34 + 4212;
        v0 = 16;
    }
    else
    {
LABEL_44b0e6:
        if (!*((int *)&g_4b4534->padding_38[20]))
            goto LABEL_44b12d;
        /* unsupported instruction */
        /* unsupported instruction */
        v31 = g_4b4534->field_34 + 4212;
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v32 = v30 + 1;
        v1 = g_4b4534->field_34 + 4212;
        v0 = 0x11;
    }
    v30 = v32 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    idx = sub_4273f0(v0, v31, v2, v3);
    if (idx)
    {
        idx->field_9cc = *((int *)&g_4b4534->padding_38[16]);
        idx->field_9d0 = *((int *)&g_4b4534->padding_38[20]);
        idx->field_9c8 = g_4b4534->field_30;
    }
LABEL_44b12d:
    v33 = g_4b4534->field_30 + 1;
    switch (v33)
    {
    case 0:
        v46 = g_4b0c54 - 1;
        if (g_4b0c54 != 1)
        {
            v47 = v46 - 1;
            if (v46 != 1)
            {
                v48 = v47 - 1;
                if (v47 == 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v49 = (unsigned short)v30 + 1;
                    v48 = sub_4273f0(15, g_4b4534->field_34 + 4212, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    break;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v49 = (unsigned short)v30 + 1;
                    break;
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v49 = (unsigned short)v30 + 1;
                v48 = sub_4273f0(14, g_4b4534->field_34 + 4212, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                break;
            }
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v49 = (unsigned short)v30 + 1;
            v48 = sub_4273f0(13, g_4b4534->field_34 + 4212, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            break;
        }
        if (!idx)
            return v48;
        /* unsupported instruction */
        /* unsupported instruction */
        v50 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4b4534->field_50) * 0x100 & 0x4500;
        /* unsupported instruction */
        v51 = v49;
        v52 = _ccall(10, 13, (char)(((v51 & 7) * 0x800 | (unsigned short)v50 & 0x4700) >> 8) & 65, 0, 0);
        if (!(v52 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v53 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4b4534->field_50) * 0x100 & 0x4500;
            /* unsupported instruction */
            v54 = _ccall(10, 13, (char)(((v51 & 7) * 0x800 | (unsigned short)v53 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v54 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        idx->field_9d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_9cc = *((int *)&g_4b4534->padding_38[16]);
        v55 = *((int *)&g_4b4534->padding_38[20]);
        idx->field_9d0 = v55;
        idx->field_9c8 = g_4b4534->field_30;
        if (!*((int *)&g_4b4534->padding_38[20]))
            return v55;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b4534->field_54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b4534->field_58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b4534->field_5c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&g_4b4534->padding_64[0]) = 120;
        g_4b4534->field_60 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v56 = 1374389535 * g_4b0c78 >> 37;
        v0 = *((int *)&g_4b4534->padding_38[20]) * ((v56 >> 31) + v56 - (int)(((v56 >> 31) + v56) % 10));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v57 = sub_4931e0();
        g_4b4534->field_68 = v57;
        return v57;
    case 2:
        if (idx)
        {
            if (g_4b0c48 >= *((int *)&g_4b0cd0))
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_9d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_5c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&g_4b4534->padding_64[0]) = 120;
            g_4b4534->field_60 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v34 = 1374389535 * g_4b0c78 >> 37;
            v35 = (v34 >> 31) + v34 - (int)(((v34 >> 31) + v34) % 10);
            v4 = v35 * *((int *)&g_4b4534->padding_38[16]);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v36 = sub_4931e0();
            g_4b4534->field_68 = v36 + v35 * *((int *)&g_4b4534->padding_38[20]);
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4273f0(4, g_4b4534->field_34 + 4212, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4273f0(13, g_4b4534->field_34 + 4212, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    case 3:
        if (idx)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v37 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4b4534->field_50) * 0x100 & 0x4500;
            /* unsupported instruction */
            v38 = v30;
            v39 = _ccall(10, 13, (char)(((v38 & 7) * 0x800 | (unsigned short)v37 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v39 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v40 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4b4534->field_50) * 0x100 & 0x4500;
                /* unsupported instruction */
                v41 = _ccall(10, 13, (char)(((v38 & 7) * 0x800 | (unsigned short)v40 & 0x4700) >> 8) & 65, 0, 0);
                if (!(v41 & 1))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
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
                }
            }
            idx->field_9d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_5c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&g_4b4534->padding_64[0]) = 120;
            g_4b4534->field_60 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v42 = 1374389535 * g_4b0c78 >> 37;
            v4 = *((int *)&g_4b4534->padding_38[20]) * ((v42 >> 31) + v42 - (int)(((v42 >> 31) + v42) % 10));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v43 = sub_4931e0();
            g_4b4534->field_68 = v43;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4273f0(14, g_4b4534->field_34 + 4212, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    case 4:
        if (idx)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_9d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b4534->field_5c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&g_4b4534->padding_64[0]) = 120;
            g_4b4534->field_60 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v44 = 1374389535 * g_4b0c78 >> 37;
            v4 = *((int *)&g_4b4534->padding_38[20]) * ((v44 >> 31) + v44 - (int)(((v44 >> 31) + v44) % 10));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v45 = sub_4931e0();
            g_4b4534->field_68 = v45;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4273f0(5, g_4b4534->field_34 + 4212, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4273f0(15, g_4b4534->field_34 + 4212, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    default:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return v33;
    }
}
