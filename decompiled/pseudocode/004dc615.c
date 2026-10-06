// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc615; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc615_1 {
    char field_0;
} st_4dc615_1;

typedef struct st_4dc615_0 {
    struct st_4dc615_1 *field_0;
    unsigned int field_4;
    unsigned int field_8;
} st_4dc615_0;


int sub_4dc615(st_4dc615_0 *idx, unsigned int a1, unsigned int a2)
{
    unsigned int v2;  // ebx
    unsigned int i;  // edi
    unsigned int v4;  // esi
    unsigned int v5;  // eax
    unsigned int v0;  // [bp-0x10]
    st_4dc615_0 *v1;  // [bp-0x4]

    v1 = idx;
    if (idx->field_4 >= 8)
    {
        v0 = v2;
        do
        {
            *((char *)&v1) = idx->field_0->field_0;
            idx->field_0 = idx->field_0 + 1;
            i = idx->field_4 + 0xfffffff8;
            idx->field_8 = idx->field_8 * 0x100 | v1 & 0xff;
            idx->field_4 = i;
        } while (i >= 8);
    }
    v4 = idx->field_4;
    v5 = idx->field_8;
    idx->field_4 = v4 + a2;
    return (v5 >> ((char)(8 - v4) & 31) & 0xffffff) >> ((char)(24 - a2) & 31);
}
