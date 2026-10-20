// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465f50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465f50_0 {
    char padding_0[120];
    unsigned int field_78;
    char padding_7c[16];
    HANDLE field_8c;
} st_465f50_0;

typedef struct st_465f50_1 {
    char padding_0[4];
    int field_4;
    char padding_8[4];
    struct st_465f50_0 *field_c;
} st_465f50_1;

typedef struct st_465f50_4 {
    struct st_465f50_2 *field_0;
} st_465f50_4;

typedef struct st_465f50_3 {
    unsigned int field_0;
} st_465f50_3;

typedef struct st_465f50_2 {
    char padding_0[8];
    struct st_465f50_3 *field_8;
} st_465f50_2;

extern char g_4a3b1c;

void sub_465f50(st_465f50_1 *idx)
{
    unsigned int index;  // edi
    st_465f50_4 **v2;  // eax
    st_465f50_0 *v3;  // edi

    index = 0;
    *((char **)&idx->padding_0[0]) = &g_4a3b1c;
    if (idx[1].padding_0 > NULL)
    {
        do
        {
            v2 = idx->field_4 + index * 4;
            if (*((int *)(idx->field_4 + index * 4)))
            {
                *(v2)->field_0->field_8(*(v2));
                *((unsigned int *)(idx->field_4 + index * 4)) = 0;
            }
        } while ((index += 1, index < idx[1].padding_0));
    }
    if (idx->field_4)
    {
        sub_46ca4f(idx->field_4);
        idx->field_4 = 0;
    }
    v3 = idx->field_c;
    if (!v3)
        return;
    if (v3->field_78 == 1)
    {
        CloseHandle(v3->field_8c);
        v3->field_8c = 0xffffffff;
    }
    sub_46ca4f(v3);
    idx->field_c = NULL;
    return;
}
