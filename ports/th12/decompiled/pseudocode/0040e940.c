// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e940; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e940_4 {
    char padding_0[4];
    unsigned int field_4;
} st_40e940_4;

typedef struct st_40e940_1 {
    struct st_40e940_0 *field_0;
    struct st_40e940_1 *field_4;
} st_40e940_1;

typedef struct st_40e940_15 {
    char padding_0[1016];
    unsigned int field_3f8;
} st_40e940_15;

typedef struct st_40e940_0 {
    char padding_0[1016];
    unsigned int field_3f8;
    char padding_3fc[128];
    unsigned int field_47c;
} st_40e940_0;

typedef struct st_40e940_10 {
    char padding_0[8];
    struct st_40e940_4 *field_8;
    struct st_40e940_4 *field_c;
    char padding_10[12];
    unsigned int field_1c;
    char padding_20[72];
    struct st_40e940_1 *field_68;
    char padding_6c[8];
    unsigned int field_74;
} st_40e940_10;

extern char g_4b50e0;
extern unsigned int g_4ce8cc;

void sub_40e940(unsigned int a0, unsigned int a1, st_40e940_10 *idx)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    st_40e940_1 *v13;  // eax
    st_40e940_1 *v14;  // eax
    st_40e940_4 *idx1;  // eax
    st_40e940_4 *idx2;  // eax
    unsigned int iter;  // esi
    st_40e940_1 *v6;  // eax
    unsigned int v7;  // edx
    unsigned int v8;  // ecx
    st_40e940_15 *v9;  // eax
    st_40e940_1 *v10;  // eax
    unsigned int v11;  // ecx
    unsigned int index;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int i;  // [bp-0x4]

    i = a0;
    v1 = v3;
    v0 = v4;
    iter = &(&g_4b50e0)[g_4ce8cc];
    i = 4;
    do
    {
        v6 = *((int *)(g_4ce8cc + 8935096));
        v7 = *((int *)iter);
        if (v6)
        {
            do
            {
                v8 = v6->field_4;
                v9 = v6->field_0;
                if (v9->field_3f8 == v7)
                    *((unsigned int *)&v9[1].padding_0[128]) = *((int *)&v9[1].padding_0[128]) | 0x10000000;
            } while ((v6 = (st_40e940_1 *)v8, v6));
        }
        v10 = *((int *)(g_4ce8cc + 8935104));
        if (v10)
        {
            do
            {
                v11 = v10->field_4;
                index = v10->field_0;
                if (*((int *)(index + 1016)) == v7)
                    *((unsigned int *)(index + 1148)) = *((int *)(index + 1148)) | 0x10000000;
            } while ((v10 = (st_40e940_1 *)v11, v10));
        }
        iter += 4;
        i -= 1;
    } while (i != 1);
    v13 = idx->field_68;
    if (idx->field_68)
    {
        do
        {
            v14 = v13;
            if (v14->field_0)
                (*((int *)(*((int *)&v14->field_0->padding_0[0]) + 20)))(1);
        } while ((v13 = (st_40e940_1 *)v14->field_4, v14->field_4));
    }
    idx->field_74 = 0;
    idx->field_1c = 0;
    idx1 = idx->field_8;
    if (idx1)
        idx1->field_4 = idx1->field_4 & 0xfffffffd;
    idx2 = idx->field_c;
    if (idx2)
        idx2->field_4 = idx2->field_4 & 0xfffffffd;
    return;
}
