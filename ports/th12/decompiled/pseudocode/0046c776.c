// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c776; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct SECURITY_ATTRIBUTES {
    unsigned int nLength;
    void* lpSecurityDescriptor;
    int bInheritHandle;
} SECURITY_ATTRIBUTES;

HANDLE sub_46c776(unsigned int a0, enum FILE_SHARE_MODE a1, SECURITY_ATTRIBUTES *a2, enum FILE_CREATION_DISPOSITION a3, enum FILE_FLAGS_AND_ATTRIBUTES a4, HANDLE a5, unsigned int a6)
{
    PSTR v0;  // [bp+0x0]

    return CreateFileA(v0, a0, a1, a2, a3, a4, a5);
}
