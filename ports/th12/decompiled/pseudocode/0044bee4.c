// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bee4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct OVERLAPPED {
    UIntPtr Internal;
    UIntPtr InternalHigh;
    union {
        struct {
            unsigned int Offset;
            unsigned int OffsetHigh;
        } Anonymous;
        void* Pointer;
    } Anonymous;
    HANDLE hEvent;
} OVERLAPPED;

int sub_44bee4(void)
{
    HANDLE v3;  // ecx
    unsigned int count;  // edx
    unsigned int *v5;  // eax
    OVERLAPPED *v0;  // [bp+0x0]
    Byte *v2;  // [bp+0xc]

    ReadFile(v3, v2, count, v5, v0);
    return;
}
