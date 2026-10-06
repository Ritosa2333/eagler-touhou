// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da4e8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4da4e8_0 {
    char padding_0[106];
    char field_6a;
} st_4da4e8_0;


int sub_4da4e8(void)
{
    void* v1;  // ebp
    unsigned int *v2;  // esi
    unsigned int v3;  // eax
    unsigned int v4;  // 4143
    st_4da4e8_0 *idx;  // edi
    char v6;  // bl
    unsigned short v7;  // ds

    __unsupported_jumpkind_Ijk_Privileged()
    x86g_dirtyhelper_OUT(161<32>, vvar_16{r8|4b}, 4<32>)
    *((unsigned int *)((char *)v1 - 95)) = (unsigned int)(*((int *)((char *)v1 - 95))) >> 1 | *((int *)((char *)v1 - 95)) * 0x80000000;
    *(v2) = ~(*(v2));
    v4 = _ccall(10, 13, (char)v3 & 234, 0, 0);
    if (v4 & 1)
    {
        esp = v3 + 1;
        *((unsigned int *)&idx[19246723].padding_0[0x33]) = *((int *)&idx[19246723].padding_0[0x33]) * 2;
    }
    idx->field_6a = idx->field_6a - v6;
    *((unsigned short *)(esp - 4)) = v7;
}
