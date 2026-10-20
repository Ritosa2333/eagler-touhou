// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db993; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db993_0 {
    char padding_0[69];
    unsigned int field_45;
} st_4db993_0;

int sub_4db993(void)
{
    st_4db993_0 *idx;  // ebp
    char v3;  // ah
    void* v4;  // ebx
    unsigned int v5;  // eax
    unsigned long v6;  // ldt
    unsigned long v7;  // gdt
    unsigned long long v8;  // 4136
    char v0;  // [bp+0x0]

    *((unsigned int *)(&idx->padding_0[0] - 886453345)) = *((int *)(&idx->padding_0[0] - 886453345)) & 64;
    v5 = _INSERT(0, 1, v3 | *((char *)v4 - 113));
    x86g_dirtyhelper_OUT(Conv(16->32, vvar_2{r16|2b}), vvar_13{r8|4b}, 4<32>)
    *((char **)&(idx->padding_0)[1]) = &(&v0)[*((int *)&(idx->padding_0)[1])];
    v8 = _ccall(v6, v7, 192, 1593639491);
    goto *((void *)((unsigned int)v8));
}
