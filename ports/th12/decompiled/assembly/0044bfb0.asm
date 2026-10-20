
// block 0044bfb0
0044bfb0  push       esi
0044bfb1  mov        esi, ecx
0044bfb3  cmp        dword ptr [esi + 8], 0x80000000
0044bfba  je         0x44bfc2

// block 0044bfbc
0044bfbc  xor        eax, eax
0044bfbe  pop        esi
0044bfbf  ret        4

// block 0044bfc2
0044bfc2  mov        eax, dword ptr [esi]
0044bfc4  mov        edx, dword ptr [eax + 0x14]
0044bfc7  push       ebp
0044bfc8  call       edx

// block 0044bfca
0044bfca  mov        ebp, eax
0044bfcc  cmp        ebp, dword ptr [esp + 0xc]
0044bfd0  jbe        0x44bfd9

// block 0044bfd2
0044bfd2  pop        ebp
0044bfd3  xor        eax, eax
0044bfd5  pop        esi
0044bfd6  ret        4

// block 0044bfd9
0044bfd9  push       edi
0044bfda  push       ebp
0044bfdb  call       0x46d04a

// block 0044bfe0
0044bfe0  mov        edi, eax
0044bfe2  add        esp, 4
0044bfe5  test       edi, edi
0044bfe7  jne        0x44bfef

// block 0044bfe9
0044bfe9  pop        edi
0044bfea  pop        ebp
0044bfeb  pop        esi
0044bfec  ret        4

// block 0044bfef
0044bfef  mov        eax, dword ptr [esi]
0044bff1  mov        edx, dword ptr [eax + 0x10]
0044bff4  push       ebx
0044bff5  mov        ecx, esi
0044bff7  call       edx

// block 0044bff9
0044bff9  mov        ebx, eax
0044bffb  mov        eax, dword ptr [esi]
0044bffd  mov        edx, dword ptr [eax + 0x18]
0044c000  push       0
0044c002  push       ebx
0044c003  mov        ecx, esi
0044c005  call       edx

// block 0044c007
0044c007  test       al, al
0044c009  je         0x44c023

// block 0044c00b
0044c00b  mov        eax, dword ptr [esi]
0044c00d  mov        edx, dword ptr [eax + 8]
0044c010  push       ebp
0044c011  push       edi
0044c012  mov        ecx, esi
0044c014  call       edx

// block 0044c016
0044c016  test       eax, eax
0044c018  jne        0x44c02c

// block 0044c01a
0044c01a  push       edi
0044c01b  call       0x46c941

// block 0044c020
0044c020  add        esp, 4

// block 0044c023
0044c023  pop        ebx
0044c024  pop        edi
0044c025  pop        ebp
0044c026  xor        eax, eax
0044c028  pop        esi
0044c029  ret        4

// block 0044c02c
0044c02c  mov        eax, dword ptr [esi]
0044c02e  mov        edx, dword ptr [eax + 0x18]
0044c031  push       0
0044c033  push       ebx
0044c034  mov        ecx, esi
0044c036  call       edx

// block 0044c038
0044c038  pop        ebx
0044c039  mov        eax, edi
0044c03b  pop        edi
0044c03c  pop        ebp
0044c03d  pop        esi
0044c03e  ret        4
