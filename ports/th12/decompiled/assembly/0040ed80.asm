
// block 0040ed80
0040ed80  push       ecx
0040ed81  push       esi
0040ed82  mov        esi, dword ptr [0x4b43d0]
0040ed88  push       edi
0040ed89  call       0x436410

// block 0040ed8e
0040ed8e  call       0x4097f0

// block 0040ed93
0040ed93  call       0x406b20

// block 0040ed98
0040ed98  call       0x425b10

// block 0040ed9d
0040ed9d  call       0x43dd50

// block 0040eda2
0040eda2  mov        eax, dword ptr [esi + 0x114]
0040eda8  mov        ecx, dword ptr [esi + 0x34]
0040edab  mov        edx, dword ptr [ecx + eax*4]
0040edae  push       edx
0040edaf  call       0x4130e0

// block 0040edb4
0040edb4  call       0x41df40

// block 0040edb9
0040edb9  mov        eax, dword ptr [0x4b43dc]
0040edbe  mov        ecx, dword ptr [eax + 0x64]
0040edc1  mov        edx, dword ptr [ecx + 8]
0040edc4  lea        edi, [esi + 0x1ec]
0040edca  push       1
0040edcc  mov        eax, edi
0040edce  mov        dword ptr [esi + 0x1f4], edx
0040edd4  call       0x464970

// block 0040edd9
0040edd9  push       -1
0040eddb  mov        eax, edi
0040eddd  call       0x464970

// block 0040ede2
0040ede2  pop        edi
0040ede3  mov        dword ptr [esi + 0x30], 1
0040edea  xor        eax, eax
0040edec  pop        esi
0040eded  pop        ecx
0040edee  ret        
