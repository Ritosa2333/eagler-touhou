// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f4f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f4f0_0 {
    char padding_0[12];
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[1624];
    int field_66c;
    char padding_670[21920];
    int field_5c10;
} st_43f4f0_0;

extern char g_4a20cc;
extern char g_4a3738;
extern unsigned int g_4ad138;
extern unsigned int g_4b4530;
extern char g_4b5120;
extern char g_4b5124;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee78;
extern char g_4cf0f8;
extern char g_4cf218;

void sub_43f4f0(st_43f4f0_0 *idx)
{
    unsigned long v8;  // ldt
    unsigned long v9;  // gdt
    int v18;  // esi
    int v19;  // esi
    unsigned int v20;  // edi
    unsigned long long v21;  // 4122
    unsigned short v10;  // fs
    unsigned long long v11;  // 4141
    unsigned int v12;  // eax
    unsigned int v13;  // ecx
    unsigned long long v14;  // 4165
    unsigned int v15;  // ebx
    unsigned int v16;  // ebx
    st_43f4f0_0 *iter;  // ebx
    unsigned int v0;  // [bp-0x28]
    char v1;  // [bp-0x20]
    int v2;  // [bp-0x1c]
    int v3;  // [bp-0x18]
    int v4;  // [bp-0x14]
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x4]

    v7 = 0xffffffff;
    v11 = _ccall(v8, v9, v10, 0);
    v12 = *((int *)(unsigned int)v11);
    v6 = *((int *)(unsigned int)v11);
    v5 = v13;
    v14 = _ccall(v8, v9, v10, 0);
    *((unsigned int **)(unsigned int)v14) = &v6;
    *((char **)&idx->padding_0[0]) = &g_4a20cc;
    sub_464c40(g_4ad138 ^ &v1, v1, v2, v3, v4, &idx[1].padding_0[8], v12, sub_4974ae, 0);
    if (idx->field_c)
    {
        if (g_4cee78 & 0x8000)
        {
            v0 = &g_4cf0f8;
            EnterCriticalSection(&g_4cf0f8);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            v0 = &g_4cf0f8;
            LeaveCriticalSection(&g_4cf0f8);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (idx->field_10)
    {
        if (g_4cee78 & 0x8000)
        {
            v0 = &g_4cf0f8;
            EnterCriticalSection(&g_4cf0f8);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            v0 = &g_4cf0f8;
            LeaveCriticalSection(&g_4cf0f8);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    v15 = &(&g_4b5120)[g_4ce8cc];
    if (*((int *)&(&g_4b5120)[g_4ce8cc]))
    {
        sub_4604e0();
        v0 = *((int *)v15);
        sub_46ca4f(*((int *)v15));
        *((unsigned int *)v15) = 0;
    }
    v16 = &(&g_4b5124)[g_4ce8cc];
    if (*((int *)&(&g_4b5124)[g_4ce8cc]))
    {
        sub_4604e0();
        v0 = *((int *)v16);
        sub_46ca4f(*((int *)v16));
        *((unsigned int *)v16) = 0;
    }
    iter = &idx->padding_670[21520];
    v18 = 100;
    do
    {
        v19 = v18;
        v20 = *((int *)&iter->padding_0[0]);
        if (v20)
        {
            sub_43b450(v20);
            sub_46ca4f(v20);
        }
    } while ((iter += 4, v18 = (int)(v19 - 1), v19 != 1));
    sub_461970(idx->field_66c, v0);
    if (idx->field_5c10 != v18)
    {
        sub_46c941(idx->field_5c10);
        idx->field_5c10 = v18;
    }
    g_4b4530 = v18;
    *((char **)&idx[1].padding_0[8]) = &g_4a3738;
    sub_464c40();
    v21 = _ccall(v8, v9, v10, 0);
    *((unsigned int *)(unsigned int)v21) = v12;
    return;
}
