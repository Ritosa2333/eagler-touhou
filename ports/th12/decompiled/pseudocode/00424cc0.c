// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x424cc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_424cc0_4 {
    char padding_0[124];
    struct st_424cc0_5 *field_7c;
} st_424cc0_4;

typedef struct st_424cc0_1 {
    char padding_0[28];
    char field_1c;
    char padding_1d[67];
    int field_60;
    struct st_424cc0_2 *field_64;
    struct st_424cc0_2 *field_68;
    int field_6c;
    int field_70;
    char padding_74[16];
    char field_84;
    char field_85;
    char field_86;
    char field_87;
} st_424cc0_1;

typedef struct st_424cc0_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_424cc0_2;

typedef struct st_424cc0_0 {
    char padding_0[4];
    int field_4;
    int field_8;
    int field_c;
    unsigned int field_10;
    unsigned int field_14;
} st_424cc0_0;

typedef struct st_424cc0_5 {
    struct st_424cc0_1 *field_0;
} st_424cc0_5;

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

extern int g_4a0280;
extern int g_4a02a8;
extern int g_4a02e0;
extern HANDLE g_4ae590;
extern unsigned int g_4aeef8[4];
extern unsigned int g_4aeefc[4];
extern unsigned int g_4aef10[4];
extern unsigned int g_4aef14[4];
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf128;
extern char g_4cf21a;

unsigned int sub_424cc0(st_424cc0_4 *a0, unsigned int a1)
{
    char *v25;  // edi
    char *v26;  // eax
    char *v35;  // eax
    char *v125;  // eax
    char *v126;  // eax
    char *v127;  // eax
    unsigned int count;  // esi
    char *v129;  // eax
    char *v130;  // eax
    char *v131;  // eax
    char *v132;  // eax
    unsigned int count1;  // esi
    char *v134;  // eax
    char *v36;  // eax
    char *v135;  // eax
    char *v136;  // eax
    char *v137;  // eax
    unsigned int count2[2];  // esi
    char *v38;  // eax
    char *v39;  // eax
    char *v41;  // eax
    char *v42;  // eax
    st_424cc0_0 *v44;  // eax
    char *v27;  // eax
    char *v45;  // eax
    char *v46;  // eax
    char *v48;  // eax
    char *v49;  // eax
    char *v51;  // eax
    char *v52;  // eax
    st_424cc0_1 **v54;  // eax
    st_424cc0_1 *v55;  // ebx
    st_424cc0_1 **i;  // ecx
    char *v57;  // eax
    char *v58;  // eax
    char *v59;  // eax
    char *v60;  // eax
    unsigned int v61;  // esi
    char *v62;  // eax
    char *v63;  // eax
    char *v64;  // eax
    char *v29;  // eax
    char *v65;  // eax
    unsigned int v66;  // esi
    char *v67;  // eax
    char *v68;  // eax
    char *v69;  // eax
    char *v70;  // eax
    unsigned int v71;  // esi
    char *v72;  // eax
    char *v73;  // eax
    char *v74;  // eax
    char *v30;  // eax
    char *v75;  // eax
    unsigned int v76;  // esi
    char *v77;  // eax
    char *v78;  // eax
    char *v79;  // eax
    char *v80;  // eax
    unsigned int v81;  // esi
    char *v83;  // eax
    char *v84;  // eax
    char *v85;  // eax
    char *v86;  // eax
    unsigned int v87;  // esi
    char *v88;  // eax
    char *v89;  // eax
    char *v90;  // eax
    char *v91;  // eax
    unsigned int v92;  // esi
    int idx;  // eax
    st_424cc0_1 *v94;  // ebx
    char *v32;  // eax
    unsigned int *v95;  // eax
    char *v96;  // eax
    char *v97;  // eax
    char *v98;  // eax
    char *v99;  // eax
    unsigned int v100;  // esi
    int index;  // eax
    st_424cc0_1 *v102;  // ebx
    unsigned int *v103;  // eax
    char *v104;  // eax
    char *v33;  // eax
    char *v105;  // eax
    char *v106;  // eax
    char *v107;  // eax
    unsigned int v108;  // esi
    char *v109;  // eax
    char *v110;  // eax
    char *v111;  // eax
    char *v112;  // eax
    unsigned int v113;  // esi
    char *v114;  // eax
    char *v115;  // eax
    char *v116;  // eax
    char *v117;  // eax
    unsigned int v118;  // esi
    char *v119;  // eax
    char *v120;  // eax
    char *v121;  // eax
    char *v122;  // eax
    unsigned int v123;  // esi
    char *v124;  // eax
    char *v0;  // [bp-0x94]
    unsigned int v1;  // [bp-0x90]
    double v2;  // [bp-0x8c], Other Possible Types: unsigned long
    int v3;  // [bp-0x84]
    int v4;  // [bp-0x80]
    int v5;  // [bp-0x7c]
    int v6;  // [bp-0x54]
    int v7;  // [bp-0x50]
    st_424cc0_4 *iter;  // [bp-0x4c]
    st_424cc0_1 **v9;  // [bp-0x48]
    unsigned int v10;  // [bp-0x44]
    unsigned int v11;  // [bp-0x40]
    unsigned int v12;  // [bp-0x3c]
    unsigned int v13;  // [bp-0x38]
    unsigned int v14;  // [bp-0x34]
    unsigned int v15;  // [bp-0x30]
    unsigned int v16;  // [bp-0x2c]
    unsigned int v17;  // [bp-0x28]
    unsigned int v18;  // [bp-0x24]
    unsigned int v19;  // [bp-0x20]
    unsigned int v20;  // [bp-0x1c]
    unsigned int v21;  // [bp-0x18]
    unsigned int v22;  // [bp-0x14]
    unsigned int v23;  // [bp-0x10]
    unsigned int v24[2];  // [bp-0xc]

    v25 = sub_46d04a(0x1000, v3, v4, v5);
    sub_46d585("hint");
    if (!sub_463fb0())
    {
        sub_46cbbb(v25, "# ========================================================= \r\n");
        v26 = v25;
        do
        {
            v27 = v26;
            v26 = v27 + 1;
        } while (*(v27));
        sub_464130();
        sub_46cbbb(v25, &g_4a0280);
        v29 = v25;
        do
        {
            v30 = v29;
            v29 = v30 + 1;
        } while (*(v30));
        sub_464130();
        sub_46cbbb(v25, "#\r\n");
        v32 = v25;
        do
        {
            v33 = v32;
            v32 = v33 + 1;
        } while (*(v33));
        sub_464130();
        sub_46cbbb(v25, &g_4a02a8);
        v35 = v25;
        do
        {
            v36 = v35;
            v35 = v36 + 1;
        } while (*(v36));
        sub_464130();
        sub_46cbbb(v25, &g_4a02e0);
        v38 = v25;
        do
        {
            v39 = v38;
            v38 = v39 + 1;
        } while (*(v39));
        sub_464130();
        sub_46cbbb(v25, "#\r\n");
        v41 = v25;
        do
        {
            v42 = v41;
            v41 = v42 + 1;
        } while (*(v42));
        sub_464130();
        sub_46d5b7(v24);
        v44 = sub_46d8e4(v24);
        sub_46cbbb(v25, "#                                Time-stamp: <%.4d/%.2d/%.2d %.2d:%.2d>\r\n", v44->field_14 + 1900, v44->field_10 + 1, v44->field_c, v44->field_8, v44->field_4);
        v45 = v25;
        do
        {
            v46 = v45;
            v45 = v46 + 1;
        } while (*(v46));
        sub_464130();
        sub_46cbbb(v25, "\r\n\r\n");
        v48 = v25;
        do
        {
            v49 = v48;
            v48 = v49 + 1;
        } while (*(v49));
        sub_464130();
        sub_46cbbb(v25, "Version = %s\r\n\r\n", "0.0");
        v51 = v25;
        do
        {
            v52 = v51;
            v51 = v52 + 1;
        } while (*(v52));
        sub_464130();
        v6 = 0;
        iter = &a0->field_7c;
        do
        {
            v54 = *((int *)&iter->padding_0[0]);
            v7 = 0;
            if (v54)
            {
                do
                {
                    v55 = *(v54);
                    i = v54[1];
                    v9 = i;
                    if (!v55->field_70)
                    {
                        v54 = i;
                    }
                    else
                    {
                        if (!v7)
                        {
                            sub_46cbbb(v25, "# ================================== \r\n");
                            v57 = v25;
                            v58 = v57;
                            do
                            {
                                v59 = v58;
                                v60 = v59 + 1;
                                v58 = v60;
                            } while (*(v59));
                            v61 = v60 - (v57 + 1);
                            if (g_4ae590 != 0xffffffff)
                            {
                                WriteFile(g_4ae590, v25, v61, &v10, NULL);
                                if (v61 != v10)
                                {
                                    CloseHandle(g_4ae590);
                                    if (g_4cee78 & 0x8000)
                                    {
                                        LeaveCriticalSection(&g_4cf128.DebugInfo);
                                        g_4cf21a = g_4cf21a - 1;
                                    }
                                }
                            }
                            sub_46cbbb(v25, "Stage : %d\r\n\r\n", v6);
                            v62 = v25;
                            v63 = v62;
                            do
                            {
                                v64 = v63;
                                v65 = v64 + 1;
                                v63 = v65;
                            } while (*(v64));
                            v66 = v65 - (v62 + 1);
                            if (g_4ae590 != 0xffffffff)
                            {
                                WriteFile(g_4ae590, v25, v66, &v11, NULL);
                                if (v66 != v11)
                                {
                                    CloseHandle(g_4ae590);
                                    if (g_4cee78 & 0x8000)
                                    {
                                        LeaveCriticalSection(&g_4cf128.DebugInfo);
                                        g_4cf21a = g_4cf21a - 1;
                                    }
                                }
                            }
                        }
                        sub_46cbbb(v25, "Tips\r\n", v6);
                        v67 = v25;
                        v68 = v67;
                        do
                        {
                            v69 = v68;
                            v70 = v69 + 1;
                            v68 = v70;
                        } while (*(v69));
                        v71 = v70 - (v67 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v71, &v12, NULL);
                            if (v71 != v12)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        if (v55->field_70 > 0)
                            sub_46cbbb(v25, "\tRemain\t: %d\r\n", v55->field_70);
                        v72 = v25;
                        v73 = v72;
                        do
                        {
                            v74 = v73;
                            v75 = v74 + 1;
                            v73 = v75;
                        } while (*(v74));
                        v76 = v75 - (v72 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v76, &v13, NULL);
                            if (v76 != v13)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        sub_46cbbb(v25, "\tText\t: \"%s\"\r\n", &v55->field_1c);
                        v77 = v25;
                        v78 = v77;
                        do
                        {
                            v79 = v78;
                            v80 = v79 + 1;
                            v78 = v80;
                        } while (*(v79));
                        v81 = v80 - (v77 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v81, &v14, NULL);
                            if (v81 != v14)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
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
                        sub_46cbbb(v25, "\tPos\t\t: %d, %d\r\n", sub_4931e0(sub_4931e0()));
                        v83 = v25;
                        v84 = v83;
                        do
                        {
                            v85 = v84;
                            v86 = v85 + 1;
                            v84 = v86;
                        } while (*(v85));
                        v87 = v86 - (v83 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v87, &v15, NULL);
                            if (v87 != v15)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        sub_46cbbb(v25, "\tCount\t: %d\r\n", v55->field_60);
                        v88 = v25;
                        v89 = v88;
                        do
                        {
                            v90 = v89;
                            v91 = v90 + 1;
                            v89 = v91;
                        } while (*(v90));
                        v92 = v91 - (v88 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v92, &v16, NULL);
                            if (v92 != v16)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        idx = 0;
                        v94 = v55;
                        do
                        {
                            if (g_4aef14[2 * idx] == v55->field_68)
                            {
                                v95 = g_4aef10[2 * idx];
                                goto LABEL_425225;
                            }
                        } while ((idx = (int)(idx + 1), idx < 61));
                        v95 = NULL;
LABEL_425225:
                        sub_46cbbb(v25, "\tBase\t: %s\r\n", v95);
                        v96 = v25;
                        v97 = v96;
                        do
                        {
                            v98 = v97;
                            v99 = v98 + 1;
                            v97 = v99;
                        } while (*(v98));
                        v100 = v99 - (v96 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v100, &v17, NULL);
                            if (v100 != v17)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        index = 0;
                        v102 = v94;
                        do
                        {
                            if (g_4aeefc[2 * index] == v94->field_64)
                            {
                                v103 = g_4aeef8[2 * index];
                                goto LABEL_4252b5;
                            }
                        } while ((index = (int)(index + 1), index < 3));
                        v103 = NULL;
LABEL_4252b5:
                        sub_46cbbb(v25, "\tAlign\t: %s\r\n", v103);
                        v104 = v25;
                        v105 = v104;
                        do
                        {
                            v106 = v105;
                            v107 = v106 + 1;
                            v105 = v107;
                        } while (*(v106));
                        v108 = v107 - (v104 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v108, &v18, NULL);
                            if (v108 != v18)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        sub_46cbbb(v25, "\tTime\t: %d\r\n", v102->field_6c);
                        v109 = v25;
                        v110 = v109;
                        do
                        {
                            v111 = v110;
                            v112 = v111 + 1;
                            v110 = v112;
                        } while (*(v111));
                        v113 = v112 - (v109 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v113, &v19, NULL);
                            if (v113 != v19)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        sub_46cbbb(v25, "\tAlpha\t: %d\r\n", v102->field_87);
                        v114 = v25;
                        v115 = v114;
                        do
                        {
                            v116 = v115;
                            v117 = v116 + 1;
                            v115 = v117;
                        } while (*(v116));
                        v118 = v117 - (v114 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v118, &v20, NULL);
                            if (v118 != v20)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        sub_46cbbb(v25, "\tColor\t: %d, %d, %d\r\n", v102->field_86, v102->field_85, v102->field_84);
                        v119 = v25;
                        v120 = v119;
                        do
                        {
                            v121 = v120;
                            v122 = v121 + 1;
                            v120 = v122;
                        } while (*(v121));
                        v123 = v122 - (v119 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, v123, &v21, NULL);
                            if (v123 != v21)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
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
                            v2 = /* unsupported instruction */;
                            /* unsupported instruction */
                        }
                        else
                        {
                            v2 = (double)nan;
                            /* unsupported instruction */
                        }
                        v1 = "\tScale\t: %.1f\r\n";
                        v0 = v25;
                        sub_46cbbb();
                        v124 = v25;
                        v125 = v124;
                        do
                        {
                            v126 = v125;
                            v127 = v126 + 1;
                            v125 = v127;
                        } while (*(v126));
                        count = v127 - (v124 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, count, &v22, NULL);
                            if (count != v22)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        sub_46cbbb(v25, "End\r\n\r\n", v6);
                        v129 = v25;
                        v130 = v129;
                        do
                        {
                            v131 = v130;
                            v132 = v131 + 1;
                            v130 = v132;
                        } while (*(v131));
                        count1 = v132 - (v129 + 1);
                        if (g_4ae590 != 0xffffffff)
                        {
                            WriteFile(g_4ae590, v25, count1, &v23, NULL);
                            if (count1 != v23)
                            {
                                CloseHandle(g_4ae590);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                                    g_4cf21a = g_4cf21a - 1;
                                }
                            }
                        }
                        v7 += 1;
                        if (v7 >= 0xff)
                            break;
                        i = v9;
                    }
                } while (i);
                if (v7)
                {
                    sub_46cbbb(v25, "StageEnd\r\n");
                    v134 = v25;
                    v135 = v134;
                    do
                    {
                        v136 = v135;
                        v137 = v136 + 1;
                        v135 = v137;
                    } while (*(v136));
                    count2 = (unsigned int (32 bits)[2])(v137 - (v134 + 1));
                    if (g_4ae590 != 0xffffffff)
                    {
                        WriteFile(g_4ae590, v25, count2, v24, NULL);
                        if (count2 != v24)
                        {
                            CloseHandle(g_4ae590);
                            if (g_4cee78 & 0x8000)
                            {
                                LeaveCriticalSection(&g_4cf128.DebugInfo);
                                g_4cf21a = g_4cf21a - 1;
                            }
                        }
                    }
                }
            }
        } while ((iter += 12, v6 = (int)(v6 + 1), v6 < 8));
    }
    if (g_4ae590 != 0xffffffff)
    {
        CloseHandle(g_4ae590);
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf128.DebugInfo);
            g_4cf21a = g_4cf21a - 1;
        }
    }
    sub_46c941(v25);
    return 0;
}
