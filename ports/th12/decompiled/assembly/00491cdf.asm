
// block 00491cdf
00491cdf  mov        edi, edi
00491ce1  push       ebp
00491ce2  mov        ebp, esp
00491ce4  push       ecx
00491ce5  push       ebx
00491ce6  push       esi
00491ce7  push       edi
00491ce8  mov        edi, dword ptr [ebp + 8]
00491ceb  mov        eax, dword ptr [edi + 0x10]
00491cee  mov        esi, dword ptr [edi + 0xc]
00491cf1  mov        dword ptr [ebp - 4], eax
00491cf4  mov        ebx, esi
00491cf6  jmp        0x491d25

// block 00491cf8
00491cf8  cmp        esi, -1
00491cfb  jne        0x491d02

// block 00491cfd
00491cfd  call       0x472b94

// block 00491d02
00491d02  mov        ecx, dword ptr [ebp - 4]
00491d05  dec        esi
00491d06  mov        eax, esi
00491d08  imul       eax, eax, 0x14
00491d0b  add        eax, ecx
00491d0d  mov        ecx, dword ptr [ebp + 0x10]
00491d10  cmp        dword ptr [eax + 4], ecx
00491d13  jge        0x491d1a

// block 00491d15
00491d15  cmp        ecx, dword ptr [eax + 8]
00491d18  jle        0x491d1f

// block 00491d1a
00491d1a  cmp        esi, -1
00491d1d  jne        0x491d28

// block 00491d1f
00491d1f  dec        dword ptr [ebp + 0xc]
00491d22  mov        ebx, dword ptr [ebp + 8]

// block 00491d25
00491d25  mov        dword ptr [ebp + 8], esi

// block 00491d28
00491d28  cmp        dword ptr [ebp + 0xc], 0
00491d2c  jge        0x491cf8

// block 00491d2e
00491d2e  mov        eax, dword ptr [ebp + 0x14]
00491d31  inc        esi
00491d32  mov        dword ptr [eax], esi
00491d34  mov        eax, dword ptr [ebp + 0x18]
00491d37  mov        dword ptr [eax], ebx
00491d39  cmp        ebx, dword ptr [edi + 0xc]
00491d3c  ja         0x491d42

// block 00491d3e
00491d3e  cmp        esi, ebx
00491d40  jbe        0x491d47

// block 00491d42
00491d42  call       0x472b94

// block 00491d47
00491d47  mov        eax, esi
00491d49  imul       eax, eax, 0x14
00491d4c  add        eax, dword ptr [ebp - 4]
00491d4f  pop        edi
00491d50  pop        esi
00491d51  pop        ebx
00491d52  leave      
00491d53  ret        
