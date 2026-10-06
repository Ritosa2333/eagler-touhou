
// block 00434890
00434890  fldz       
00434892  mov        dword ptr [esi + 4], 0x1a
00434899  mov        eax, dword ptr [esi + 0x20]
0043489c  xor        ecx, ecx
0043489e  test       al, 1
004348a0  jne        0x4348bc

// block 004348a2
004348a2  or         eax, 1
004348a5  fst        dword ptr [esi + 0x18]
004348a8  mov        dword ptr [esi + 0x14], ecx
004348ab  mov        dword ptr [esi + 0x10], 0xfff0bdc1
004348b2  mov        dword ptr [esi + 0x1c], 0x4b2ed0
004348b9  mov        dword ptr [esi + 0x20], eax

// block 004348bc
004348bc  fstp       dword ptr [esi + 0x18]
004348bf  push       ebx
004348c0  mov        dword ptr [esi + 0x14], ecx
004348c3  mov        dword ptr [esi + 0x10], 0xffffffff
004348ca  mov        eax, dword ptr [esi + 0x1ec]
004348d0  push       eax
004348d1  mov        ebx, 1
004348d6  call       0x461970

// block 004348db
004348db  mov        ecx, dword ptr [esi + 0x1e8]
004348e1  push       ecx
004348e2  call       0x461970

// block 004348e7
004348e7  fld        dword ptr [esi + 0x2d8]
004348ed  fstp       dword ptr [0x4b2ed0]
004348f3  pop        ebx
004348f4  ret        
