// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9915; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d9915(void)
{
    void* v1;  // ebx
    unsigned int v3;  // cc_op
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4101
    unsigned int v8;  // 4101

    if (_INSERT(0, 0, *((char *)v1 - 63)))
        goto LABEL_0x4d991a;
    v7 = _ccall(14, v3, v4, v5, v6);
    if (!(v7 & 1))
    {
        v8 = _ccall(6, v3, v4, v5, v6);
        return;
    }
}
