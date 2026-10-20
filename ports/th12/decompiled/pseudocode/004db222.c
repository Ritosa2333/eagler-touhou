// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db222; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db222_0 {
    unsigned int field_0;
    char padding_4[104];
    char field_6c;
} st_4db222_0;

int sub_4db222(void)
{
    char v1;  // bh
    st_4db222_0 *v2;  // edi
    unsigned int v3;  // 4122
    unsigned int v4;  // edx

    x86g_dirtyhelper_OUT(111<32>, vvar_0{r8|4b}, 4<32>)
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v3 = _ccall(1, v1, v2->field_6c, 0);
    v2->field_0 = v2->field_0 + v4 + (v3 & 1);
}
