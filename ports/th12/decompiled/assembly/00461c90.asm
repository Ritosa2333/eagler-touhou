
// block 00461c90
00461c90  mov        ecx, dword ptr [eax]
00461c92  mov        edx, dword ptr [0x4ce8cc]
00461c98  push       ecx
00461c99  call       0x461920

// block 00461c9e
00461c9e  test       eax, eax
00461ca0  je         0x461cdd

// block 00461ca2
00461ca2  mov        ecx, dword ptr [eax + 0x47c]
00461ca8  and        ecx, 0xfffffffd
00461cab  mov        edx, 0x40000
00461cb0  or         ecx, edx
00461cb2  cmp        dword ptr [eax + 0x18], 0
00461cb6  mov        dword ptr [eax + 0x47c], ecx
00461cbc  jne        0x461cdd

// block 00461cbe
00461cbe  mov        eax, dword ptr [eax + 0x14]
00461cc1  test       eax, eax
00461cc3  je         0x461cdd

// block 00461cc5
00461cc5  mov        ecx, dword ptr [eax]
00461cc7  and        dword ptr [ecx + 0x47c], 0xfffffffd
00461cce  mov        ecx, dword ptr [eax]
00461cd0  or         dword ptr [ecx + 0x47c], edx
00461cd6  mov        eax, dword ptr [eax + 4]
00461cd9  test       eax, eax
00461cdb  jne        0x461cc5

// block 00461cdd
00461cdd  ret        
