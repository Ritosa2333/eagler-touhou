// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ac64; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct Guid {
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    char Data4[8];
} Guid;

// attributes: PLT stub
int DirectInput8Create(HINSTANCE hinst, unsigned int dwVersion, struct Guid *riidltf, void* *ppvOut, IUnknown punkOuter)
{
    HINSTANCE v0;  // [bp+0x0]

    return ::dinput8.dll::DirectInput8Create(v0, hinst, dwVersion, riidltf, ppvOut);
}
