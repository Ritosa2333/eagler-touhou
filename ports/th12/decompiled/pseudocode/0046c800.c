// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c800; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct SECURITY_ATTRIBUTES {
    unsigned int nLength;
    void* lpSecurityDescriptor;
    int bInheritHandle;
} SECURITY_ATTRIBUTES;

HANDLE sub_46c800(int a0, int a1, PSTR a2, unsigned int a3)
{
    SECURITY_ATTRIBUTES *v0;  // [bp+0x0]

    return CreateEventA(v0, a0, a1, a2);
}
