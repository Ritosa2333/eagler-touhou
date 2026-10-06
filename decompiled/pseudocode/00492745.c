// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492745; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492745_0 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
    char padding_c[4];
    unsigned int field_10;
} st_492745_0;


void sub_492745(void)
{
    void* index;  // ebp
    st_492745_0 *idx;  // esi
    void* v3;  // edi
    int v4;  // ecx
    unsigned int v5;  // ebx
    unsigned int v6;  // eax

    esp = *((int *)((char *)index - 24)) - 4;
    *((unsigned int *)(sub_475467() + 524)) = 0;
    idx = (int)index[20];
    v3 = (int)index[12];
    v4 = (idx->field_4 <= 128 ? (char)v3[8] : (int)v3[8]);
    v5 = idx->field_10;
    for (*((unsigned int *)((char *)index - 32)) = 0; *((int *)((char *)index - 32)) < idx->padding_c; *((unsigned int *)((char *)index - 32)) = *((int *)((char *)index - 32)) + 1)
    {
        v6 = *((int *)((char *)index - 32)) * 20 + v5;
        if (v4 > *((int *)(v6 + 4)) && v4 <= *((int *)(v6 + 8)))
        {
            v4 = *((int *)(idx->field_8 + *((int *)(v6 + 4)) * 8 + 8));
            break;
        }
    }
    esp = esp - 4;
    *((int *)(esp - 4)) = v4;
    esp = esp - 4;
    *((st_492745_0 **)(esp - 4)) = idx;
    *((unsigned int *)(esp - 4)) = 0;
    *((void* *)(esp - 8)) = v3;
    sub_49205b();
    *((unsigned int *)((char *)index - 28)) = 0;
    *((unsigned int *)((char *)index - 4)) = 0;
}
