// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d12a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48d12a_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_48d12a_0;


unsigned int sub_48d12a(st_48d12a_0 *idx)
{
    unsigned int v1;  // ebx
    unsigned int v2;  // edi
    int v3;  // edi
    unsigned int v4;  // eax
    unsigned int v0;  // [bp-0x10]

    v1 = 0;
    if (((char)idx->field_c & 3) == 2 && idx->field_c & 264)
    {
        v0 = v2;
        v3 = idx->field_0 - idx->field_8;
        if (v3 > 0)
        {
            if (sub_47be9c(sub_47c1da(idx, idx->field_8, v3)) != v3)
            {
                idx->field_c = idx->field_c | 32;
                v1 = 0xffffffff;
            }
            else if ((char)idx->field_c < 0)
            {
                idx->field_c = idx->field_c & 0xfffffffd;
            }
        }
    }
    v4 = idx->field_8;
    idx->field_4 = 0;
    idx->field_0 = v4;
    return v1;
}
