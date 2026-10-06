
// block 0048b39a
0048b39a  push       ebp
0048b39b  mov        ebp, esp
0048b39d  sub        esp, 0x10
0048b3a0  mov        dword ptr [ebp - 4], edi
0048b3a3  mov        eax, dword ptr [ebp + 8]
0048b3a6  cdq        
0048b3a7  mov        edi, eax
0048b3a9  xor        edi, edx
0048b3ab  sub        edi, edx
0048b3ad  and        edi, 0xf
0048b3b0  xor        edi, edx
0048b3b2  sub        edi, edx
0048b3b4  test       edi, edi
0048b3b6  jne        0x48b3f4

// block 0048b3b8
0048b3b8  mov        ecx, dword ptr [ebp + 0x10]
0048b3bb  mov        edx, ecx
0048b3bd  and        edx, 0x7f
0048b3c0  mov        dword ptr [ebp - 0xc], edx
0048b3c3  cmp        ecx, edx
0048b3c5  je         0x48b3d9

// block 0048b3c7
0048b3c7  sub        ecx, edx
0048b3c9  push       ecx
0048b3ca  push       eax
0048b3cb  call       0x48b343

// block 0048b3d0
0048b3d0  add        esp, 8
0048b3d3  mov        eax, dword ptr [ebp + 8]
0048b3d6  mov        edx, dword ptr [ebp - 0xc]

// block 0048b3d9
0048b3d9  test       edx, edx
0048b3db  je         0x48b422

// block 0048b3dd
0048b3dd  add        eax, dword ptr [ebp + 0x10]
0048b3e0  sub        eax, edx
0048b3e2  mov        dword ptr [ebp - 8], eax
0048b3e5  xor        eax, eax
0048b3e7  mov        edi, dword ptr [ebp - 8]
0048b3ea  mov        ecx, dword ptr [ebp - 0xc]

// block 0048b3ed
0048b3ed  rep stosb  byte ptr es:[edi], al

// block 0048b3ef
0048b3ef  mov        eax, dword ptr [ebp + 8]
0048b3f2  jmp        0x48b422

// block 0048b3f4
0048b3f4  neg        edi
0048b3f6  add        edi, 0x10
0048b3f9  mov        dword ptr [ebp - 0x10], edi
0048b3fc  xor        eax, eax
0048b3fe  mov        edi, dword ptr [ebp + 8]
0048b401  mov        ecx, dword ptr [ebp - 0x10]

// block 0048b404
0048b404  rep stosb  byte ptr es:[edi], al

// block 0048b406
0048b406  mov        eax, dword ptr [ebp - 0x10]
0048b409  mov        ecx, dword ptr [ebp + 8]
0048b40c  mov        edx, dword ptr [ebp + 0x10]
0048b40f  add        ecx, eax
0048b411  sub        edx, eax
0048b413  push       edx
0048b414  push       0
0048b416  push       ecx
0048b417  call       0x48b39a

// block 0048b41c
0048b41c  add        esp, 0xc
0048b41f  mov        eax, dword ptr [ebp + 8]

// block 0048b422
0048b422  mov        edi, dword ptr [ebp - 4]
0048b425  mov        esp, ebp
0048b427  pop        ebp
0048b428  ret        
