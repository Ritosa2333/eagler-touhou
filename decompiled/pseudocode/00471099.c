// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x471099; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_471099_0 {
    struct st_471099_1 *field_0;
    int field_4;
    unsigned int field_8;
    char field_c;
} st_471099_0;

typedef struct st_471099_1 {
    char field_0;
} st_471099_1;

int sub_471099(void)
{
    st_471099_0 *idx;  // ecx
    int v2;  // 4096
    char v3;  // al
    unsigned int v4;  // eax
    unsigned int *v5;  // esi
    unsigned int *v6;  // esi

    if (!(idx->field_c & 64) || idx->field_8)
    {
        v2 = idx->field_4;
        idx->field_4 = idx->field_4 - 1;
        if (v2 - 1 >= 0)
        {
            idx->field_0->field_0 = v3;
            idx->field_0 = idx->field_0 + 1;
            v4 = v3;
        }
        else
        {
            v4 = sub_46ffdb(v3, idx);
        }
        if (v4 == 0xffffffff)
        {
            *(v5) = 0xffffffff;
            return;
        }
    }
    *(v6) = *(v6) + 1;
    return;
}
