
// block 0047676f
0047676f  mov        edi, edi
00476771  push       ebp
00476772  mov        ebp, esp
00476774  sub        esp, 0x10
00476777  mov        eax, dword ptr [0x4ad138]
0047677c  xor        eax, ebp
0047677e  mov        dword ptr [ebp - 4], eax
00476781  mov        edx, dword ptr [ebp + 0x18]
00476784  mov        ecx, dword ptr [ebp + 0x10]
00476787  mov        eax, dword ptr [ebp + 0xc]
0047678a  push       esi
0047678b  mov        esi, dword ptr [ebp + 8]
0047678e  push       edi
0047678f  push       edx
00476790  xor        edx, edx
00476792  push       edx
00476793  push       edx
00476794  push       edx
00476795  push       dword ptr [ebp + 0x14]
00476798  push       ecx
00476799  push       eax
0047679a  lea        eax, [ebp - 0x10]
0047679d  push       eax
0047679e  call       0x476077

// block 004767a3
004767a3  mov        edi, eax
004767a5  lea        eax, [ebp - 0x10]
004767a8  push       esi
004767a9  push       eax
004767aa  call       0x48a072

// block 004767af
004767af  add        esp, 0x28
004767b2  cmp        eax, 1
004767b5  jne        0x4767ba

// block 004767b7
004767b7  or         edi, 2

// block 004767ba
004767ba  mov        ecx, dword ptr [ebp - 4]
004767bd  mov        eax, edi
004767bf  pop        edi
004767c0  xor        ecx, ebp
004767c2  pop        esi
004767c3  call       0x46c932

// block 004767c8
004767c8  leave      
004767c9  ret        
