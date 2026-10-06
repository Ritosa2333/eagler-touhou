// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c788; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

int sub_46c788(Byte *a0, unsigned int a1, unsigned int *a2, OVERLAPPED *a3, unsigned int a4)
{
    HANDLE v0;  // [bp+0x0]

    return ReadFile(v0, a0, a1, a2, a3);
}
