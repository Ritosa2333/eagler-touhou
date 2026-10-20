// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44b710; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44b710_2 {
    void* field_0;
    unsigned int field_4;
    int field_8;
    struct st_44b710_3 *field_c;
} st_44b710_2;

typedef struct st_44b710_0 {
    char padding_0[28];
    struct st_44b710_1 *field_1c;
} st_44b710_0;

typedef struct st_44b710_3 {
    struct st_44b710_0 *field_0;
} st_44b710_3;

typedef struct st_44b710_1 {
    unsigned int field_0;
} st_44b710_1;

void sub_44b710(void)
{
    unsigned int v2;  // edi
    st_44b710_2 *idx;  // esi
    void* v4;  // eax
    int v5;  // ebx
    st_44b710_0 **v6;  // ecx
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    if (idx->field_8)
    {
        sub_44bcd0("info : %s close arcfile\r\n", idx->field_8);
        if (idx->field_8)
        {
            sub_46c941(idx->field_8);
            idx->field_8 = 0;
        }
    }
    v4 = idx->field_0;
    idx->field_8 = 0;
    if (v4)
    {
        sub_46cf1e(v4, 16, *((int *)((char *)v4 - 4)), sub_44bc50, v5);
        sub_46ca4f(v4 - 4);
    }
    v6 = idx->field_c;
    idx->field_0 = NULL;
    if (v6)
        *(v6)->field_1c(1);
    idx->field_4 = 0;
    idx->field_c = NULL;
    return;
}
