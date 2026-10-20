// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db2f3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db2f3_0 {
    char padding_0[66];
    char field_42;
} st_4db2f3_0;

int sub_4db2f3(void)
{
    unsigned int index;  // ebp
    unsigned int v3;  // ecx
    st_4db2f3_0 *idx;  // edx
    char v5;  // bl
    st_4db2f3_0 *v0;  // [bp-0x4]

    *((int *)(index + 537797040)) = *((int *)(index + 537797040));
    x86g_dirtyhelper_OUT(Conv(16->32, Extract(vvar_2{r16|4b}, 16bits@0<64>)), Conv(8->32, Extract(vvar_0{r8|4b}, 8bits@0<64>)), 1<32>)
    x86g_dirtyhelper_OUT(Conv(16->32, Extract(vvar_2{r16|4b}, 16bits@0<64>)), vvar_0{r8|4b}, 4<32>)
    if (v3 == 0xffffffff)
        goto LABEL_0x4db2d6;
    idx->field_42 = idx->field_42 & v5;
    v0 = idx;
}
