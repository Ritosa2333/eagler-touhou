
// block 0048a139
0048a139  mov        edi, edi
0048a13b  push       ebp
0048a13c  mov        ebp, esp
0048a13e  sub        esp, 0x10
0048a141  push       esi
0048a142  push       dword ptr [ebp + 0xc]
0048a145  lea        ecx, [ebp - 0x10]
0048a148  call       0x46d3b4

// block 0048a14d
0048a14d  mov        esi, dword ptr [ebp + 8]
0048a150  movsx      eax, byte ptr [esi]
0048a153  push       eax
0048a154  call       0x47ab27

// block 0048a159
0048a159  cmp        eax, 0x65
0048a15c  jmp        0x48a16a

// block 0048a15e
0048a15e  inc        esi
0048a15f  movzx      eax, byte ptr [esi]
0048a162  push       eax
0048a163  call       0x48ba71

// block 0048a168
0048a168  test       eax, eax

// block 0048a16a
0048a16a  pop        ecx
0048a16b  jne        0x48a15e

// block 0048a16d
0048a16d  movsx      eax, byte ptr [esi]
0048a170  push       eax
0048a171  call       0x47ab27

// block 0048a176
0048a176  pop        ecx
0048a177  cmp        eax, 0x78
0048a17a  jne        0x48a17e

// block 0048a17c
0048a17c  inc        esi
0048a17d  inc        esi

// block 0048a17e
0048a17e  mov        ecx, dword ptr [ebp - 0x10]
0048a181  mov        ecx, dword ptr [ecx + 0xbc]
0048a187  mov        ecx, dword ptr [ecx]
0048a189  mov        al, byte ptr [esi]
0048a18b  mov        cl, byte ptr [ecx]
0048a18d  mov        byte ptr [esi], cl
0048a18f  inc        esi

// block 0048a190
0048a190  mov        cl, byte ptr [esi]
0048a192  mov        byte ptr [esi], al
0048a194  mov        al, cl
0048a196  mov        cl, byte ptr [esi]
0048a198  inc        esi
0048a199  test       cl, cl
0048a19b  jne        0x48a190

// block 0048a19d
0048a19d  pop        esi
0048a19e  cmp        byte ptr [ebp - 4], cl
0048a1a1  je         0x48a1aa

// block 0048a1a3
0048a1a3  mov        eax, dword ptr [ebp - 8]
0048a1a6  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048a1aa
0048a1aa  leave      
0048a1ab  ret        
