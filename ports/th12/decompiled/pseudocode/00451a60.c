// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451a60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_451a60_0 {
    char padding_0[8];
    struct st_451a60_1 *field_8;
    struct st_451a60_1 *field_c;
    struct st_451a60_1 *field_10;
} st_451a60_0;

typedef struct st_451a60_15 {
    struct st_451a60_0 *field_0;
} st_451a60_15;

typedef struct st_451a60_1 {
    unsigned int field_0;
} st_451a60_1;

typedef struct st_451a60_14 {
    char padding_0[12];
    struct st_451a60_15 *field_c;
    char padding_10[16];
    struct st_451a60_15 *field_20;
    struct st_451a60_15 *field_24;
    char padding_28[472];
    char field_200;
} st_451a60_14;

typedef struct st_451a60_9 {
    char padding_0[12];
    struct st_451a60_1 *field_c;
    struct st_451a60_1 *field_10;
    char padding_14[24];
    struct st_451a60_1 *field_2c;
    char padding_30[4];
    struct st_451a60_1 *field_34;
} st_451a60_9;

typedef struct st_451a60_4 {
    char padding_0[8];
    struct st_451a60_1 *field_8;
    char padding_c[16];
    struct st_451a60_1 *field_1c;
    char padding_20[12];
    struct st_451a60_1 *field_2c;
    char padding_30[4];
    struct st_451a60_1 *field_34;
} st_451a60_4;

typedef struct Guid {
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    char Data4[8];
} Guid;

extern char g_49855c;
extern char g_498764;
extern Guid g_4993cc;
extern char g_49953c;
extern int g_4a29b4;
extern st_451a60_4 *g_4a29d4;
extern unsigned int g_4a2a04;
extern int g_4a2a38;
extern int g_4a2a60;
extern unsigned int g_4ce8c8;
extern unsigned int g_4ce914;
extern unsigned int g_4cf3f0;
extern HINSTANCE g_4cf3f8;

int sub_451a60(void)
{
    unsigned int v5;  // esi
    st_451a60_14 *idx;  // eax
    st_451a60_4 **v15;  // eax
    st_451a60_9 **v16;  // esi
    unsigned int *v17;  // ecx
    unsigned int v7;  // ebx
    st_451a60_14 *v8;  // ebx
    st_451a60_14 *v9;  // edi
    unsigned int v10;  // edi
    st_451a60_0 **v11;  // eax
    int v12;  // eax
    st_451a60_4 **v13;  // eax
    st_451a60_0 **v14;  // eax
    unsigned int v0;  // [bp-0x38]
    int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    v3 = v5;
    if (idx->field_200 & 8)
        return;
    v2 = v7;
    v8 = &idx->field_c;
    if (DirectInput8Create(g_4cf3f8, 0x800, &g_4993cc.Data1, v8, 0) < 0)
    {
        *((st_451a60_0 ***)&v8->padding_0[0]) = NULL;
        sub_464220(&g_4a29b4);
        return;
    }
    v9 = &idx->field_20;
    if ((*((int *)(*((int *)*((int *)&v8->padding_0[0])) + 12)))(*((int *)&v8->padding_0[0]), &g_49953c, v9, 0, v10) < 0)
    {
        v11 = *((int *)&v8->padding_0[0]);
        if (v11)
        {
            *(v11)->field_8(v11);
            *((st_451a60_0 ***)&v8->padding_0[0]) = NULL;
        }
        v1 = &g_4a29b4;
    }
    else
    {
        v12 = (*((int *)(*((int *)*((int *)&v9->padding_0[0])) + 44)))(*((int *)&v9->padding_0[0]), &g_49855c);
        v13 = *((int *)&v9->padding_0[0]);
        if (v12 < 0)
        {
            if (v13)
            {
                *(v13)->field_8(v13);
                *((st_451a60_4 ***)&v9->padding_0[0]) = NULL;
            }
            v14 = *((int *)&v8->padding_0[0]);
            if (v14)
            {
                *(v14)->field_8(v14);
                *((st_451a60_0 ***)&v8->padding_0[0]) = NULL;
            }
            v13 = &g_4a29d4;
        }
        else if (*(v13)->field_34(v13, g_4cf3f0, 22) < 0)
        {
            v15 = *((int *)&v9->padding_0[0]);
            if (v15)
            {
                *(v15)->field_8(v15);
                *((st_451a60_4 ***)&v9->padding_0[0]) = NULL;
            }
            sub_4318f0();
            v0 = &g_4a2a04;
        }
        else
        {
            (*((int *)(*((int *)*((int *)&v9->padding_0[0])) + 28)))(*((int *)&v9->padding_0[0]));
            sub_464220(&g_4a2a38);
            (*((int *)(*((int *)*((int *)&v8->padding_0[0])) + 16)))(*((int *)&v8->padding_0[0]), 4, sub_451c70, 0, 1);
            if (!idx->field_24)
                return;
            (*((int *)&idx->field_24->field_0[2].padding_0[4]))(idx->field_24, &g_498764);
            idx->field_24->field_0[2].field_c(idx->field_24, g_4cf3f0, 10);
            g_4ce914 = 44;
            idx->field_24->field_0->field_c(idx->field_24, &g_4ce914);
            v16 = idx->field_24;
            v17 = &idx->field_24->field_0->field_10->field_0;
            g_4ce8c8 = 0;
            v17(v16, sub_451cb0, 0, 0);
            sub_464220(&g_4a2a60);
            return;
        }
    }
    sub_464220(v13);
    return;
}
