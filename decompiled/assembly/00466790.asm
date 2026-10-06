
// block 00466790
00466790  cmp        dword ptr [esi + 0x1c], 4
00466794  jne        0x4667c6

// block 00466796
00466796  dec        dword ptr [esi + 0x14]
00466799  mov        eax, dword ptr [esi + 0x14]
0046679c  test       eax, eax
0046679e  jg         0x4667ad

// block 004667a0
004667a0  mov        dword ptr [esi + 0x1c], 0
004667a7  mov        eax, 1
004667ac  ret        

// block 004667ad
004667ad  imul       eax, eax, 0x3e8
004667b3  cdq        
004667b4  idiv       dword ptr [esi + 0x18]
004667b7  mov        ecx, eax
004667b9  sub        ecx, 0x3e8
004667bf  mov        eax, esi
004667c1  call       0x4663e0

// block 004667c6
004667c6  xor        eax, eax
004667c8  ret        
