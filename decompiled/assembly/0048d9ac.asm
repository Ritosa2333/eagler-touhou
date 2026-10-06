
// block 0048d9ac
0048d9ac  mov        edi, edi
0048d9ae  push       ebp
0048d9af  mov        ebp, esp
0048d9b1  push       ecx
0048d9b2  push       ebx
0048d9b3  push       esi
0048d9b4  mov        esi, ecx
0048d9b6  xor        ebx, ebx
0048d9b8  cmp        esi, ebx
0048d9ba  jne        0x48d9da

// block 0048d9bc
0048d9bc  call       0x46e96f

// block 0048d9c1
0048d9c1  push       0x16
0048d9c3  pop        esi
0048d9c4  push       ebx
0048d9c5  push       ebx
0048d9c6  push       ebx
0048d9c7  push       ebx
0048d9c8  push       ebx
0048d9c9  mov        dword ptr [eax], esi
0048d9cb  call       0x470f2d

// block 0048d9d0
0048d9d0  add        esp, 0x14
0048d9d3  mov        eax, esi
0048d9d5  jmp        0x48da83

// block 0048d9da
0048d9da  push       edi
0048d9db  cmp        dword ptr [ebp + 8], ebx
0048d9de  ja         0x48d9fe

// block 0048d9e0
0048d9e0  call       0x46e96f

// block 0048d9e5
0048d9e5  push       0x16

// block 0048d9e7
0048d9e7  pop        esi
0048d9e8  push       ebx
0048d9e9  push       ebx
0048d9ea  push       ebx
0048d9eb  push       ebx
0048d9ec  push       ebx
0048d9ed  mov        dword ptr [eax], esi
0048d9ef  call       0x470f2d

// block 0048d9f4
0048d9f4  add        esp, 0x14
0048d9f7  mov        eax, esi
0048d9f9  jmp        0x48da82

// block 0048d9fe
0048d9fe  xor        ecx, ecx
0048da00  cmp        dword ptr [ebp + 0x10], ebx
0048da03  mov        byte ptr [esi], bl
0048da05  setne      cl
0048da08  inc        ecx
0048da09  cmp        dword ptr [ebp + 8], ecx
0048da0c  ja         0x48da17

// block 0048da0e
0048da0e  call       0x46e96f

// block 0048da13
0048da13  push       0x22
0048da15  jmp        0x48d9e7

// block 0048da17
0048da17  mov        ecx, dword ptr [ebp + 0xc]
0048da1a  add        ecx, -2
0048da1d  cmp        ecx, 0x22
0048da20  ja         0x48d9e0

// block 0048da22
0048da22  mov        dword ptr [ebp - 4], ebx
0048da25  mov        ecx, esi
0048da27  cmp        dword ptr [ebp + 0x10], ebx
0048da2a  je         0x48da3b

// block 0048da2c
0048da2c  mov        byte ptr [esi], 0x2d
0048da2f  lea        ecx, [esi + 1]
0048da32  mov        dword ptr [ebp - 4], 1
0048da39  neg        eax

// block 0048da3b
0048da3b  mov        edi, ecx

// block 0048da3d
0048da3d  xor        edx, edx
0048da3f  div        dword ptr [ebp + 0xc]
0048da42  cmp        edx, 9
0048da45  jbe        0x48da4c

// block 0048da47
0048da47  add        dl, 0x57
0048da4a  jmp        0x48da4f

// block 0048da4c
0048da4c  add        dl, 0x30

// block 0048da4f
0048da4f  mov        byte ptr [ecx], dl
0048da51  inc        ecx
0048da52  inc        dword ptr [ebp - 4]
0048da55  xor        ebx, ebx
0048da57  cmp        eax, ebx
0048da59  jbe        0x48da63

// block 0048da5b
0048da5b  mov        edx, dword ptr [ebp + 8]
0048da5e  cmp        dword ptr [ebp - 4], edx
0048da61  jb         0x48da3d

// block 0048da63
0048da63  mov        eax, dword ptr [ebp - 4]
0048da66  cmp        eax, dword ptr [ebp + 8]
0048da69  jb         0x48da6f

// block 0048da6b
0048da6b  mov        byte ptr [esi], bl
0048da6d  jmp        0x48da0e

// block 0048da6f
0048da6f  mov        byte ptr [ecx], bl
0048da71  dec        ecx

// block 0048da72
0048da72  mov        dl, byte ptr [edi]
0048da74  mov        al, byte ptr [ecx]
0048da76  mov        byte ptr [ecx], dl
0048da78  dec        ecx
0048da79  mov        byte ptr [edi], al
0048da7b  inc        edi
0048da7c  cmp        edi, ecx
0048da7e  jb         0x48da72

// block 0048da80
0048da80  xor        eax, eax

// block 0048da82
0048da82  pop        edi

// block 0048da83
0048da83  pop        esi
0048da84  pop        ebx
0048da85  leave      
0048da86  ret        0xc
