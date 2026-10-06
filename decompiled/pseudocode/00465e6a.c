// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465e6a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465e6a_0 {
    char padding_0[12];
    struct st_465e6a_1 *field_c;
} st_465e6a_0;

typedef struct st_465e6a_1 {
    unsigned int field_0;
} st_465e6a_1;

typedef struct st_465e6a_4 {
    struct st_465e6a_1 *field_0;
    char padding_4[4];
    struct st_465e6a_1 *field_8;
} st_465e6a_4;

extern char g_49981c;

int sub_465e6a(void)
{
    st_465e6a_0 **v3;  // eax
    unsigned int v4;  // ebp
    unsigned int v13;  // ecx
    st_465e6a_4 **v14;  // eax
    unsigned int v15;  // ebx
    unsigned int v5;  // edx
    st_465e6a_0 *v6;  // ecx
    unsigned int v7;  // edi
    unsigned int *idx;  // esi
    st_465e6a_4 **v9;  // edi
    int v10;  // edi
    unsigned int index;  // eax
    int v12;  // edi
    unsigned int v0;  // [bp-0x1c]
    char v1;  // [bp+0x0]

    if (v6->field_c(v3, v4, v5) < 0)
        return;
    v9 = *((int *)(v7 + idx[1]));
    if (*(v9)->field_0(v9, &g_49981c, &v1) < 0)
        return;
    v10 = sub_46c9ea(128);
    if (!v10)
        return;
    index = 0;
    v12 = v10;
    do
    {
        v13 = index + 1;
        *((unsigned int *)(v12 + index * 8)) = v13 * idx[28] - 1;
        *((unsigned int *)(v12 + index * 8 + 4)) = idx[29];
        index = v13;
    } while (index < 16);
    v14 = v9;
    if (*(v3)->field_c(v3, 16, v12) >= 0)
    {
        if (v14)
        {
            *(v14)->field_8(v14);
            v0 = 0;
        }
        sub_46ca4f(v12);
        if (v15 + 1 < idx[4])
            goto LABEL_0x465e55;
        else
            goto LABEL_0x465f09;
    }
    else
    {
        if (v14)
        {
            *(v14)->field_8(v14);
            v0 = 0;
        }
        sub_46ca4f(v12);
        return;
    }
}
