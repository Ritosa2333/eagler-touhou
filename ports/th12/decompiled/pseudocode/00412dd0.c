// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412dd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412dd0_1 {
    unsigned int field_0;
} st_412dd0_1;

typedef struct st_412dd0_0 {
    char padding_0[8];
    struct st_412dd0_1 *field_8;
} st_412dd0_0;

extern int g_49f434;
extern unsigned int g_4b43dc;

unsigned int sub_412dd0(st_412dd0_0 **a0, unsigned int a1, void* a2)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // esi
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // 4101
    void* v17;  // edi
    unsigned int v18;  // esi
    void* v19;  // ebx
    void* v20;  // eax
    void* v21;  // eax
    void* v22;  // eax
    unsigned int i;  // esi
    void* v23;  // eax
    void* v6;  // ebx
    unsigned int v7;  // ebp
    unsigned int v8;  // eax
    void* v9;  // eax
    void* v10;  // eax
    void* v11;  // eax
    void* v12;  // eax
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0xc]
    st_412dd0_0 **v2;  // [bp-0x4]

    v2 = a0;
    v2 = a0;
    if (*((int *)a2) != 1296649793)
        return 0;
    v1 = v3;
    v0 = v4;
    i = 0;
    v6 = a2 + 8;
    if ((int)a2[4] > 0)
    {
        v7 = 0x44;
        do
        {
            v8 = sub_45fe60();
            *((unsigned int *)(g_4b43dc + v7)) = v8;
            if (!v8)
            {
                sub_464220(&g_49f434);
                return 0xffffffff;
            }
            v9 = v6;
            v10 = v9;
            do
            {
                v11 = v10;
                v12 = v11 + 1;
                v10 = v12;
            } while (*((char *)v11));
            i += 1;
            v7 += 4;
            v6 = v6 + v12 - (v9 + 1) + 1;
        } while (i < (int)a2[4]);
        a0 = v2;
    }
    v13 = v6 - a2;
    v14 = v13 & 0x80000003;
    if ((v13 & 0x80000003) < 0)
    {
        v15 = v14 - 1 | 0xfffffffc;
        v14 = v15 + 1;
        v16 = _ccall(4, 18, v15 + 1, 0, 0);
        if (!(v16 & 1))
            goto LABEL_412e4c;
    }
    else if (v13 & 0x80000003)
    {
LABEL_412e4c:
        v6 += 4 - v14;
    }
    v17 = v6;
    if (*((int *)v6) != 1229734725)
        return 0;
    v18 = 0;
    v19 = v6 + 8;
    if ((int)v17[4] <= 0)
        return 0;
    while (1)
    {
        *(a0)->field_8(v19);
        v20 = v19;
        v21 = v20;
        do
        {
            v22 = v21;
            v23 = v22 + 1;
            v21 = v23;
        } while (*((char *)v22));
        v18 += 1;
        v19 = v19 + v23 - (v20 + 1) + 1;
        if (v18 >= (int)v17[4])
            break;
        a0 = v2;
    }
    return 0;
}
