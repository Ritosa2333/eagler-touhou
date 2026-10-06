
// block 00466592
00466592  push       eax
00466593  mov        eax, dword ptr [ecx + 0x90]
00466599  call       0x466bc0

// block 0046659e
0046659e  mov        ecx, dword ptr [esi + 4]
004665a1  mov        eax, dword ptr [ecx]
004665a3  mov        ecx, dword ptr [esi + 0x24]
004665a6  push       ecx
004665a7  mov        ecx, dword ptr [esi + 0x20]
004665aa  push       ecx
004665ab  mov        dword ptr [esi + 0x30], 1
004665b2  mov        edx, dword ptr [eax]
004665b4  mov        edx, dword ptr [edx + 0x30]
004665b7  push       0
004665b9  push       eax
004665ba  call       edx

// block 004665bc
004665bc  ret        
