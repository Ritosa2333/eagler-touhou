
// block 004666c0
004666c0  cmp        dword ptr [esi + 0x1c], 1
004666c4  jne        0x466703

// block 004666c6
004666c6  dec        dword ptr [esi + 0x14]
004666c9  mov        eax, dword ptr [esi + 0x14]
004666cc  test       eax, eax
004666ce  jg         0x4666ea

// block 004666d0
004666d0  mov        eax, dword ptr [esi + 4]
004666d3  mov        dword ptr [esi + 0x1c], 0
004666da  mov        eax, dword ptr [eax]
004666dc  mov        ecx, dword ptr [eax]
004666de  mov        edx, dword ptr [ecx + 0x48]
004666e1  push       eax
004666e2  call       edx

// block 004666e4
004666e4  mov        eax, 1
004666e9  ret        

// block 004666ea
004666ea  imul       eax, eax, 0x1388
004666f0  cdq        
004666f1  idiv       dword ptr [esi + 0x18]
004666f4  mov        ecx, eax
004666f6  sub        ecx, 0x1388
004666fc  mov        eax, esi
004666fe  call       0x4663e0

// block 00466703
00466703  xor        eax, eax
00466705  ret        
