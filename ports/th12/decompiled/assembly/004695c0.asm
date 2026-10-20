
// block 004695c0
004695c0  push       ecx
004695c1  mov        dword ptr [esi + 0x102c], eax
004695c7  call       0x4694f0

// block 004695cc
004695cc  fldz       
004695ce  mov        edx, dword ptr [esi + 4]
004695d1  mov        dword ptr [edx + 4], eax
004695d4  mov        eax, dword ptr [esi + 4]
004695d7  fstp       dword ptr [eax]
004695d9  xor        eax, eax
004695db  ret        
