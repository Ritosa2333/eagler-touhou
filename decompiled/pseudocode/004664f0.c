// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4664f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4664f0_3 {
    char padding_0[120];
    unsigned int field_78;
    char padding_7c[16];
    HANDLE field_8c;
    char padding_90[8];
    unsigned int field_98;
} st_4664f0_3;

typedef struct st_4664f0_2 {
    struct st_4664f0_0 *field_0;
} st_4664f0_2;

typedef struct st_4664f0_1 {
    unsigned int field_0;
} st_4664f0_1;

typedef struct st_4664f0_4 {
    char padding_0[4];
    struct st_4664f0_2 *field_4;
    char padding_8[4];
    struct st_4664f0_3 *field_c;
    char padding_10[32];
    unsigned int field_30;
    unsigned int field_34;
} st_4664f0_4;

typedef struct st_4664f0_0 {
    char padding_0[72];
    struct st_4664f0_1 *field_48;
} st_4664f0_0;

int sub_4664f0(void)
{
    st_4664f0_4 *idx;  // eax
    st_4664f0_2 **v2;  // eax
    unsigned int v3;  // edi
    unsigned int v4;  // ebx
    unsigned int v5;  // eax
    st_4664f0_3 *v6;  // esi

    v2 = idx->field_4;
    if (!v2)
    {
        return;
    }
    else if (!idx->field_30)
    {
        idx->field_34 = 0;
        return;
    }
    else
    {
        idx->field_30 = 0;
        idx->field_34 = 1;
        *(v2)->field_0->field_48(*(v2), v3, v4);
        v5 = SetFilePointer(idx->field_c->field_8c, 0, NULL, FILE_CURRENT);
        idx->field_c->field_98 = v5;
        v6 = idx->field_c;
        if (v6->field_78 != 1)
            return;
        CloseHandle(v6->field_8c);
        v6->field_8c = 0xffffffff;
        return;
    }
}
