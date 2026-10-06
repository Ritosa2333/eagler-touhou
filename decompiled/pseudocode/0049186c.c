// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49186c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

HANDLE sub_49186c(UIntPtr a0, UIntPtr a1, unsigned int a2)
{
    enum HEAP_FLAGS v0;  // [bp+0x0]

    return HeapCreate(v0, a0, a1);
}
