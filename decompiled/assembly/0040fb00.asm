
// block 0040fb00
0040fb00  push       esi
0040fb01  push       0x24
0040fb03  call       0x46c9ea

// block 0040fb08
0040fb08  add        esp, 4
0040fb0b  test       eax, eax
0040fb0d  je         0x40fb37

// block 0040fb0f
0040fb0f  xor        ecx, ecx
0040fb11  mov        dword ptr [eax], ecx
0040fb13  mov        dword ptr [eax + 4], ecx
0040fb16  mov        dword ptr [eax + 8], ecx
0040fb19  mov        dword ptr [eax + 0xc], ecx
0040fb1c  mov        dword ptr [eax + 0x10], ecx
0040fb1f  mov        dword ptr [eax + 0x14], ecx
0040fb22  mov        dword ptr [eax + 0x18], ecx
0040fb25  mov        dword ptr [eax + 0x1c], ecx
0040fb28  mov        dword ptr [eax + 0x20], ecx
0040fb2b  or         dword ptr [eax], 2
0040fb2e  mov        dword ptr [0x4b43d4], eax
0040fb33  mov        esi, eax
0040fb35  jmp        0x40fb39

// block 0040fb37
0040fb37  xor        esi, esi

// block 0040fb39
0040fb39  push       esi
0040fb3a  call       0x40f940

// block 0040fb3f
0040fb3f  test       eax, eax
0040fb41  je         0x40fb5b

// block 0040fb43
0040fb43  test       esi, esi
0040fb45  je         0x40fb57

// block 0040fb47
0040fb47  mov        eax, esi
0040fb49  call       0x40fa30

// block 0040fb4e
0040fb4e  push       esi
0040fb4f  call       0x46ca4f

// block 0040fb54
0040fb54  add        esp, 4

// block 0040fb57
0040fb57  xor        eax, eax
0040fb59  pop        esi
0040fb5a  ret        

// block 0040fb5b
0040fb5b  mov        eax, esi
0040fb5d  pop        esi
0040fb5e  ret        
