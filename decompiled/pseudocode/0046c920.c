// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c920; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct Guid {
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    char Data4[8];
} Guid;

int sub_46c920(unsigned int a0, enum CLSCTX a1, Guid *a2, void* *a3, unsigned int a4)
{
    Guid *v0;  // [bp+0x0]

    return CoCreateInstance(v0, a0, a1, a2, a3);
}
