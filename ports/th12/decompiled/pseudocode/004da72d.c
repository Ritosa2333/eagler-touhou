// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da72d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4da72d(void)
{
    unsigned int v2;  // eax
    unsigned int v3;  // cc_op
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4122
    unsigned int v8;  // ebx
    char v9;  // cl
    unsigned long long v10;  // 4123
    unsigned int v0;  // [bp+0x0]

    v0 = v2;
    v7 = _ccall(v3, v4, v5, v6);
    v10 = _ccall(*((char *)(v8 - 1933736773)), v9, v7, 1);
    *((char *)(v8 - 1933736773)) = v10;
}
