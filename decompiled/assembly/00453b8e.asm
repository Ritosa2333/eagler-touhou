
// block 00453b8e
00453b8e  push       ecx
00453b8f  mov        ecx, dword ptr [0x49c120]
00453b95  sub        esi, edx
00453b97  mov        edx, dword ptr [0x49c11c]
00453b9d  push       esi
00453b9e  sub        esp, 0x10
00453ba1  mov        eax, esp
00453ba3  mov        dword ptr [eax], edx
00453ba5  mov        edx, dword ptr [0x49c124]
00453bab  mov        dword ptr [eax + 4], ecx
00453bae  mov        ecx, dword ptr [0x49c128]
00453bb4  mov        dword ptr [eax + 8], edx
00453bb7  mov        edx, dword ptr [edi*4 + 0x4d0d68]
00453bbe  mov        dword ptr [eax + 0xc], ecx
00453bc1  mov        eax, dword ptr [edi*4 + 0x4d0e28]
00453bc8  mov        ecx, dword ptr [0x4cf4f8]
00453bce  push       edx
00453bcf  mov        edx, dword ptr [edi*4 + 0x4d0de8]
00453bd6  push       eax
00453bd7  push       ecx
00453bd8  mov        ecx, 0x4d4754
00453bdd  call       0x465aa0

// block 00453be2
00453be2  test       eax, eax
00453be4  jl         0x453aeb

// block 00453bea
00453bea  pop        esi
00453beb  mov        dword ptr [0x4d0e68], edi
00453bf1  xor        eax, eax
00453bf3  pop        ebx
00453bf4  ret        
