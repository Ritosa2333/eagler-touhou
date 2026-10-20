// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c7f4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct SECURITY_ATTRIBUTES {
    unsigned int nLength;
    void* lpSecurityDescriptor;
    int bInheritHandle;
} SECURITY_ATTRIBUTES;

HANDLE sub_46c7f4(UIntPtr a0, void* a1, void* a2, enum THREAD_CREATION_FLAGS a3, unsigned int *a4, unsigned int a5)
{
    SECURITY_ATTRIBUTES *v0;  // [bp+0x0]

    return CreateThread(v0, a0, a1, a2, a3, a4);
}
