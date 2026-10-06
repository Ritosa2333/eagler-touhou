// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466d70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466d70_0 {
    char padding_0[8];
    unsigned int field_8;
    char padding_c[112];
    unsigned int field_7c;
    unsigned int field_80;
    int field_84;
    unsigned int field_88;
    HANDLE field_8c;
} st_466d70_0;


int sub_466d70(void)
{
    void* v2;  // ebx
    unsigned int v3;  // eax
    unsigned int count;  // edi
    st_466d70_0 *idx;  // esi
    int v6;  // ecx
    void* v7;  // eax
    HANDLE v8;  // edx
    unsigned int v9;  // eax
    unsigned int v0;  // [bp+0x4]
    void* v1;  // [bp+0x8]

    v2 = v1;
    count = v3;
    if (idx->field_7c)
    {
        v6 = idx->field_84;
        if (!v6)
            return;
        if (v2)
            *((unsigned int *)v2) = 0;
        if (v6 + count > idx->field_80 + idx->field_88)
            count = idx->field_80 - v6 + idx->field_88;
        sub_47a330(v0, v6, count);
        v7 = v1;
        idx->field_84 = idx->field_84 + count;
        if (!v7)
            return;
        *((unsigned int *)v7) = count;
        return;
    }
    else
    {
        v8 = idx->field_8c;
        if (!v8)
        {
            return;
        }
        else if (!v0)
        {
            return;
        }
        else if (v2)
        {
            v9 = idx->field_8;
            if (count > v9)
                count = v9;
            idx->field_8 = v9 - count;
            ReadFile(v8, v0, count, &v0, NULL);
            *((unsigned int *)v2) = v0;
            return;
        }
        else
        {
            return;
        }
    }
}
