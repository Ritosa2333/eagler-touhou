
// block 0048be06
0048be06  mov        edi, edi
0048be08  push       ebp
0048be09  mov        ebp, esp
0048be0b  cmp        dword ptr [0x4b40dc], 0
0048be12  jne        0x48be26

// block 0048be14
0048be14  mov        eax, dword ptr [ebp + 8]
0048be17  mov        ecx, dword ptr [0x4adaa8]
0048be1d  movzx      eax, word ptr [ecx + eax*2]
0048be21  and        eax, 0x20
0048be24  pop        ebp
0048be25  ret        

// block 0048be26
0048be26  push       0
0048be28  push       dword ptr [ebp + 8]
0048be2b  call       0x48bdb5

// block 0048be30
0048be30  pop        ecx
0048be31  pop        ecx
0048be32  pop        ebp
0048be33  ret        
