// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43e8d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43e8d0_0 {
    char padding_0[140];
    unsigned int field_8c;
} st_43e8d0_0;

typedef struct st_43e8d0_1 {
    char padding_0[20];
    struct st_43e8d0_2 *field_14;
} st_43e8d0_1;

typedef struct WIN32_FIND_DATAA {
    unsigned int dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    unsigned int nFileSizeHigh;
    unsigned int nFileSizeLow;
    unsigned int dwReserved0;
    unsigned int dwReserved1;
    CHAR cFileName[260];
    CHAR cAlternateFileName[14];
} WIN32_FIND_DATAA;

typedef struct st_43e8d0_2 {
    unsigned int field_0;
} st_43e8d0_2;

typedef struct st_43e8d0_5 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    int field_38;
    unsigned int field_3c;
    unsigned int field_40;
    int field_44;
    char padding_48[196];
    unsigned int field_10c;
    char padding_110[4];
    int field_114;
    unsigned int field_118;
    int field_11c;
    char padding_120[196];
    unsigned int field_1e4;
    char padding_1e8[4];
    unsigned int field_1ec;
    unsigned int field_1f0;
    unsigned int field_1f4;
    char padding_1f8[196];
    unsigned int field_2bc;
    char padding_2c0[4];
    unsigned int field_2c4;
    unsigned int field_2c8;
    unsigned int field_2cc;
    unsigned int field_2d0;
    unsigned int field_2d4;
    struct st_43e8d0_0 *field_2d8;
    struct st_43e8d0_4 *field_2dc;
} st_43e8d0_5;

typedef struct st_43e8d0_3 {
    char padding_0[4];
    unsigned int field_4;
} st_43e8d0_3;

typedef struct st_43e8d0_4 {
    struct st_43e8d0_1 *field_0;
    struct st_43e8d0_3 *field_4;
} st_43e8d0_4;

typedef struct FILETIME {
    unsigned int dwLowDateTime;
    unsigned int dwHighDateTime;
} FILETIME;

extern unsigned int g_4ad138;
extern char g_4d48c0;
extern void g_4d48c4;
extern unsigned int g_4d48c8;

void sub_43e8d0(st_43e8d0_5 *idx)
{
    int v5;  // ebx
    unsigned int v6;  // eax
    int index;  // edi
    char *v16;  // eax
    char *v17;  // eax
    char *v18;  // eax
    unsigned int v19;  // eax
    char *iter;  // edx
    char *node;  // ecx
    char i;  // 4103
    unsigned int v23;  // eax
    int v24;  // eax
    st_43e8d0_4 *v7;  // ecx
    st_43e8d0_5 *v8;  // edi
    unsigned int v9;  // edi
    st_43e8d0_5 *v10;  // edi
    int v11;  // edi
    int v12;  // esi
    int v13;  // ebx
    HANDLE v14;  // edi
    st_43e8d0_5 *v0;  // [bp-0x160]
    int <0x43e8d0[is_1]|Stack bp-0x150, 1 B>;  // [bp-0x150]
    HANDLE v1;  // [bp-0x150]
    WIN32_FIND_DATAA v2;  // [bp-0x14c]
    unsigned int v3;  // [bp-0x8]

    v3 = g_4ad138 ^ &<0x43e8d0[is_1]|Stack bp-0x150, 1 B>;
    v5 = 0;
    v6 = idx->field_30 - 0;
    if (!idx->field_30)
    {
        sub_43e380(v11, v12, v13);
        v14 = FindFirstFileA("../../data/*.ecl", &v2);
        if (v14 != 0xffffffff)
        {
            do
            {
                v5 += 1;
            } while (FindNextFileA(v14, &v2));
        }
        FindClose(v14);
        index = 0;
        idx->field_38 = v5;
        if (v5)
        {
            idx->field_34 = sub_46d04a(v5 * 4);
            v1 = FindFirstFileA("../../data/*.ecl", &v2);
            if (v5 > 0)
            {
                do
                {
                    v16 = &v2.cFileName[0];
                    do
                    {
                        v17 = v16;
                        v18 = v17 + 1;
                        v16 = v18;
                    } while (*(v17));
                    v19 = sub_46d04a(-((struct WIN32_FIND_DATAA *)v2.cFileName) + v18);
                    *((unsigned int *)(idx->field_34 + index * 4)) = v19;
                    iter = *((int *)(idx->field_34 + index * 4));
                    node = &v2.cFileName[0];
                    do
                    {
                        i = *(node);
                        *(iter) = *(node);
                        node += 1;
                        iter += 1;
                    } while (i);
                } while (FindNextFileA(v1, &v2) && (index = (int)(index + 1), v2 = v2, index < v5));
            }
            FindClose(v1);
        }
        idx->field_44 = 3;
        idx->field_10c = 1;
        v23 = idx->field_44;
        if (v23)
        {
            if (v23 <= 0)
                idx->field_3c = v23 - 1;
            else
                idx->field_3c = 0;
        }
        else
        {
            idx->field_3c = 0;
        }
        v24 = v5;
        idx->field_11c = v5;
        idx->field_1e4 = 1;
        if (v24)
        {
            if (v24 <= 0)
                idx->field_114 = v24 - 1;
            else
                idx->field_114 = 0;
        }
        else
        {
            idx->field_114 = 0;
        }
        idx->field_1f4 = 1;
        idx->field_2bc = 1;
        idx->field_1ec = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_30 = 1;
        idx->field_2c4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        idx->field_2d0 = 1000;
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_2c8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_2cc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else if (v6 == 1)
    {
        v8 = &idx->field_3c;
        *((unsigned int *)&v8->padding_0[4]) = idx->field_3c;
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        v9 = *((int *)&v8->padding_0[0]) - 0;
        if (*((int *)&v8->padding_0[0]))
        {
            if (v9 != 1)
            {
                if (v9 == 2 && *((int *)&g_4d48c4) & 524289)
                    sub_40f720();
            }
            else
            {
                if (idx->field_2d8)
                {
                    v10 = &idx->field_1ec;
                    *((unsigned int *)&v10->padding_0[4]) = idx->field_1ec;
                    if (g_4d48c4 & 128 || g_4d48c0 & 128)
                        sub_464970(1);
                    if (g_4d48c4 & 64 || g_4d48c0 & 64)
                        sub_464970(-0x1);
                    if (g_4d48c8 & 524289)
                    {
                        if (idx->field_2dc)
                        {
                            idx->field_2dc->field_0->field_14(1);
                            idx->field_2dc = NULL;
                        }
                        idx->field_2dc = sub_469570(*((int *)(idx->field_2d8->field_8c + *((int *)&v10->padding_0[0]) * 8)));
                        idx->field_30 = 4;
                    }
                }
            }
        }
        else
        {
            idx->field_118 = idx->field_114;
            if (g_4d48c4 & 128 || g_4d48c0 & 128)
                sub_464970(1);
            if (g_4d48c4 & 64 || g_4d48c0 & 64)
                sub_464970(-0x1);
            if (*((int *)&g_4d48c4) & 524289)
            {
                idx->field_2d4 = idx->field_2d4 & 0xfffffffd;
                sub_464cb0(idx);
                idx->field_30 = 2;
            }
        }
    }
    else if (v6 == 4)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = idx;
        /* unsupported instruction */
        sub_468d90((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v7 = idx->field_2dc;
        if (v7)
        {
            if (v7->field_4->field_4)
            {
                _security_check_cookie();
                return;
            }
            else if (v7)
            {
                v7->field_0->field_14(1);
            }
        }
        idx->field_2dc = NULL;
        idx->field_30 = 1;
    }
    _security_check_cookie();
    return;
}
