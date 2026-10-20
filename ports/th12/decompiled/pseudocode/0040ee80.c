// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40ee80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40ee80_9 {
    char padding_0[140];
    unsigned int field_8c;
} st_40ee80_9;

typedef struct st_40ee80_0 {
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
    int field_118;
    int field_11c;
    char padding_120[196];
    unsigned int field_1e4;
    char padding_1e8[4];
    unsigned int field_1ec;
    unsigned int field_1f0;
    unsigned int field_1f4;
    char padding_1f8[196];
    unsigned int field_2bc;
    char padding_2c0[1212];
    unsigned int field_77c;
    unsigned int field_780;
    unsigned int field_784;
    unsigned int field_788;
    unsigned int field_78c;
} st_40ee80_0;

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

typedef struct st_40ee80_1 {
    char padding_0[4];
    unsigned int field_4;
} st_40ee80_1;

typedef struct FILETIME {
    unsigned int dwLowDateTime;
    unsigned int dwHighDateTime;
} FILETIME;

typedef struct st_40ee80_10 {
    char padding_0[8];
    struct st_40ee80_1 *field_8;
    struct st_40ee80_1 *field_c;
    char padding_10[84];
    struct st_40ee80_9 *field_64;
} st_40ee80_10;

typedef struct st_40ee80_13 {
    char padding_0[8];
    struct st_40ee80_1 *field_8;
    struct st_40ee80_1 *field_c;
    char padding_10[4];
    char field_14;
} st_40ee80_13;

typedef struct st_40ee80_3 {
    char padding_0[8];
    struct st_40ee80_1 *field_8;
    struct st_40ee80_1 *field_c;
} st_40ee80_3;

extern unsigned int g_4ad138;
extern st_40ee80_3 *g_4b43c8;
extern st_40ee80_10 *g_4b43dc;
extern st_40ee80_13 *g_4b44f0;
extern st_40ee80_3 *g_4b4514;
extern unsigned int g_4d48b8;
extern char g_4d48c0;
extern void g_4d48c4;
extern unsigned int g_4d48c8;
extern unsigned int g_4d49d0;
extern unsigned int g_4d49d4;
extern unsigned int g_4d49dc;

void sub_40ee80(st_40ee80_0 *idx)
{
    unsigned int v9;  // ebx
    unsigned int v10;  // esi
    int idx1;  // edi
    char *v20;  // eax
    char *v21;  // eax
    char *v22;  // eax
    unsigned int v23;  // eax
    char *iter;  // edx
    char *node;  // ecx
    char i;  // 4103
    int v27;  // eax
    int v28;  // eax
    unsigned int v11;  // edi
    unsigned int v12;  // eax
    st_40ee80_1 *index;  // eax
    st_40ee80_1 *idx2;  // eax
    st_40ee80_0 *v15;  // edi
    st_40ee80_0 *v16;  // edi
    int v17;  // ebx
    HANDLE v18;  // edi
    unsigned int v0;  // [bp-0x1b0]
    unsigned int v1;  // [bp-0x1ac]
    unsigned int v2;  // [bp-0x1a8]
    unsigned int v3;  // [bp-0x1a4]
    int <0x40ee80[is_1]|Stack bp-0x1a0, 1 B>;  // [bp-0x1a0]
    HANDLE v4;  // [bp-0x1a0]
    unsigned int v5;  // [bp-0x19c]
    WIN32_FIND_DATAA v6;  // [bp-0x14c]
    unsigned int v7;  // [bp-0x8]

    v7 = g_4ad138 ^ &<0x40ee80[is_1]|Stack bp-0x1a0, 1 B>;
    v3 = v9;
    v2 = v10;
    v1 = v11;
    if (!idx->field_30)
    {
        sub_40e850();
        v17 = 0;
        v18 = FindFirstFileA("../../data/*.ecl", &v6);
        if (v18 != 0xffffffff)
        {
            do
            {
                v17 += 1;
            } while (FindNextFileA(v18, &v6));
        }
        FindClose(v18);
        idx1 = 0;
        idx->field_38 = v17;
        if (v17)
        {
            idx->field_34 = sub_46d04a(v17 * 4);
            v4 = FindFirstFileA("../../data/*.ecl", &v6);
            if (v17 > 0)
            {
                do
                {
                    v20 = &v6.cFileName[0];
                    do
                    {
                        v21 = v20;
                        v22 = v21 + 1;
                        v20 = v22;
                    } while (*(v21));
                    v23 = sub_46d04a(-((struct WIN32_FIND_DATAA *)v6.cFileName) + v22);
                    *((unsigned int *)(idx->field_34 + idx1 * 4)) = v23;
                    iter = *((int *)(idx->field_34 + idx1 * 4));
                    node = &v6.cFileName[0];
                    do
                    {
                        i = *(node);
                        *(iter) = *(node);
                        node += 1;
                        iter += 1;
                    } while (i);
                } while (FindNextFileA(v4, &v6) && (idx1 = (int)(idx1 + 1), v6 = v6, idx1 < v17));
            }
            FindClose(v4);
        }
        idx->field_44 = 3;
        idx->field_10c = 1;
        v27 = idx->field_44;
        if (v27)
        {
            if (v27 <= 0)
                idx->field_3c = v27 - 1;
            else
                idx->field_3c = 0;
        }
        else
        {
            idx->field_3c = 0;
        }
        v28 = v17;
        idx->field_11c = v17;
        idx->field_1e4 = 1;
        if (v28)
        {
            if (v28 <= 0)
                idx->field_114 = v28 - 1;
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
        idx->field_77c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        idx->field_788 = 1000;
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_780 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_784 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else if (idx->field_30 == 1)
    {
        v15 = &idx->field_3c;
        *((unsigned int *)&v15->padding_0[4]) = idx->field_3c;
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
        {
            v0 = 1;
            sub_464970(1);
        }
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (*((int *)&v15->padding_0[0]))
        {
            if (*((int *)&v15->padding_0[0]) != 1)
            {
                if (*((int *)&v15->padding_0[0]) == 2 && *((int *)&g_4d48c4) & 524289)
                    sub_40f720();
            }
            else
            {
                if (g_4b43dc)
                {
                    v16 = &idx->field_1ec;
                    *((unsigned int *)&v16->padding_0[4]) = idx->field_1ec;
                    if (g_4d48c4 & 128 || g_4d48c0 & 128)
                        sub_464970(1);
                    if (g_4d48c4 & 64 || g_4d48c0 & 64)
                        sub_464970(-0x1);
                    if (g_4d48c8 & 524289)
                    {
                        sub_40ea10();
                        sub_436100();
                        sub_40e810();
                        sub_409630();
                        sub_412ee0();
                        sub_40e940(g_4b43dc);
                        sub_41d560(g_4b43dc);
                        sub_40e810();
                        sub_40e8b0();
                        sub_477420(&v0, 0, 80);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v5 = 10000;
                        sub_412990(*((int *)(g_4b43dc->field_64->field_8c + *((int *)&v16->padding_0[0]) * 8)), &v0);
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
                sub_41e000();
                sub_4364d0();
                sub_4098e0();
                sub_406bd0();
                sub_425c00();
                sub_43ddf0();
                sub_413170();
                idx->field_78c = idx->field_78c & 0xfffffffd;
                sub_464cb0(idx);
                idx->field_30 = 2;
            }
        }
    }
    else if (idx->field_30 == 4)
    {
        v12 = g_4d49d0;
        g_4d49d0 = g_4d48b8;
        g_4d49d4 = v12;
        sub_40f690();
        g_4d49dc = *((int *)&g_4d48c4);
        if (*((int *)&g_4d48c4) & 0x100)
        {
            if (g_4b4514->field_8)
                g_4b4514->field_8->field_4 = g_4b4514->field_8->field_4 & 0xfffffffd;
            if (g_4b4514->field_c)
                g_4b4514->field_c->field_4 = g_4b4514->field_c->field_4 & 0xfffffffd;
            sub_436100();
            if (g_4b43c8->field_8)
                g_4b43c8->field_8->field_4 = g_4b43c8->field_8->field_4 & 0xfffffffd;
            if (g_4b43c8->field_c)
                g_4b43c8->field_c->field_4 = g_4b43c8->field_c->field_4 & 0xfffffffd;
            sub_409630();
            if (g_4b43dc->field_8)
                g_4b43dc->field_8->field_4 = g_4b43dc->field_8->field_4 & 0xfffffffd;
            if (g_4b43dc->field_c)
                g_4b43dc->field_c->field_4 = g_4b43dc->field_c->field_4 & 0xfffffffd;
            sub_40e940(g_4b43dc);
            index = g_4b44f0->field_8;
            if (index)
                index->field_4 = index->field_4 & 0xfffffffd;
            idx2 = g_4b44f0->field_c;
            if (idx2)
                idx2->field_4 = idx2->field_4 & 0xfffffffd;
            sub_477420(&g_4b44f0->field_14, 0, 6713280);
            *((unsigned int *)&g_4b44f0[319681].padding_0[3]) = 0;
            *((unsigned int *)((char *)&g_4b44f0[319681].field_8 + 3)) = 0;
            idx->field_30 = 1;
        }
    }
    _security_check_cookie();
    return;
}
