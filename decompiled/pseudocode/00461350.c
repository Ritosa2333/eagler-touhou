// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461350; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461350_1 {
    unsigned int field_0;
    struct st_461350_1 *field_4;
    struct st_461350_1 *field_8;
    struct st_461350_0 *field_c;
} st_461350_1;

typedef struct st_461350_0 {
    char padding_0[4];
    struct st_461350_1 *field_4;
} st_461350_0;

extern unsigned int g_4ce8cc;

int sub_461350(void)
{
    st_461350_1 *v3;  // ebx
    st_461350_1 *idx;  // edx
    unsigned int v5;  // esi
    st_461350_0 *index;  // esi
    unsigned int v7;  // edi
    unsigned int *v8;  // eax
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    idx = &v3->field_4;
    idx->field_0 = v3;
    idx->field_4 = NULL;
    idx->field_8 = NULL;
    if (!*((int *)(g_4ce8cc + 8935104)))
    {
        *((st_461350_1 **)(g_4ce8cc + 8935104)) = idx;
    }
    else
    {
        v1 = v5;
        index = *((int *)(g_4ce8cc + 8935108));
        v0 = v7;
        if (index->field_4)
        {
            idx->field_4 = index->field_4;
            index->field_4->field_8 = idx;
        }
        index->field_4 = idx;
        idx->field_8 = index;
    }
    *((st_461350_1 **)(g_4ce8cc + 8935108)) = idx;
    *((unsigned int *)(g_4ce8cc + 8973640)) = *((int *)(g_4ce8cc + 8973640)) + 1;
    if (!*((int *)(g_4ce8cc + 8973640)))
        *((unsigned int *)(g_4ce8cc + 8973640)) = *((int *)(g_4ce8cc + 8973640)) + 1;
    v3->field_0 = *((int *)(g_4ce8cc + 8973640));
    *(v8) = *((int *)(g_4ce8cc + 8973640));
    return;
}
