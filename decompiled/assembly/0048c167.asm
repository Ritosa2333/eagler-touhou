
// block 0048c167
0048c167  mov        edi, edi
0048c169  push       ebp
0048c16a  mov        ebp, esp
0048c16c  sub        esp, 0x10
0048c16f  push       ebx
0048c170  push       esi
0048c171  mov        esi, dword ptr [ebp + 0xc]
0048c174  xor        ebx, ebx
0048c176  cmp        esi, ebx
0048c178  je         0x48c18f

// block 0048c17a
0048c17a  cmp        dword ptr [ebp + 0x10], ebx
0048c17d  je         0x48c18f

// block 0048c17f
0048c17f  cmp        byte ptr [esi], bl
0048c181  jne        0x48c195

// block 0048c183
0048c183  mov        eax, dword ptr [ebp + 8]
0048c186  cmp        eax, ebx
0048c188  je         0x48c18f

// block 0048c18a
0048c18a  xor        ecx, ecx
0048c18c  mov        word ptr [eax], cx

// block 0048c18f
0048c18f  xor        eax, eax

// block 0048c191
0048c191  pop        esi
0048c192  pop        ebx
0048c193  leave      
0048c194  ret        

// block 0048c195
0048c195  push       dword ptr [ebp + 0x14]
0048c198  lea        ecx, [ebp - 0x10]
0048c19b  call       0x46d3b4

// block 0048c1a0
0048c1a0  mov        eax, dword ptr [ebp - 0x10]
0048c1a3  cmp        dword ptr [eax + 0x14], ebx
0048c1a6  jne        0x48c1c7

// block 0048c1a8
0048c1a8  mov        eax, dword ptr [ebp + 8]
0048c1ab  cmp        eax, ebx
0048c1ad  je         0x48c1b6

// block 0048c1af
0048c1af  movzx      cx, byte ptr [esi]
0048c1b3  mov        word ptr [eax], cx

// block 0048c1b6
0048c1b6  cmp        byte ptr [ebp - 4], bl
0048c1b9  je         0x48c1c2

// block 0048c1bb
0048c1bb  mov        eax, dword ptr [ebp - 8]
0048c1be  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048c1c2
0048c1c2  xor        eax, eax
0048c1c4  inc        eax
0048c1c5  jmp        0x48c191

// block 0048c1c7
0048c1c7  lea        eax, [ebp - 0x10]
0048c1ca  push       eax
0048c1cb  movzx      eax, byte ptr [esi]
0048c1ce  push       eax
0048c1cf  call       0x47c5a3

// block 0048c1d4
0048c1d4  pop        ecx
0048c1d5  pop        ecx
0048c1d6  test       eax, eax
0048c1d8  je         0x48c257

// block 0048c1da
0048c1da  mov        eax, dword ptr [ebp - 0x10]
0048c1dd  mov        ecx, dword ptr [eax + 0xac]
0048c1e3  cmp        ecx, 1
0048c1e6  jle        0x48c20d

// block 0048c1e8
0048c1e8  cmp        dword ptr [ebp + 0x10], ecx
0048c1eb  jl         0x48c20d

// block 0048c1ed
0048c1ed  xor        edx, edx
0048c1ef  cmp        dword ptr [ebp + 8], ebx
0048c1f2  setne      dl
0048c1f5  push       edx
0048c1f6  push       dword ptr [ebp + 8]
0048c1f9  push       ecx
0048c1fa  push       esi
0048c1fb  push       9
0048c1fd  push       dword ptr [eax + 4]
0048c200  call       dword ptr [0x4980dc]

// block 0048c206
0048c206  test       eax, eax
0048c208  mov        eax, dword ptr [ebp - 0x10]
0048c20b  jne        0x48c21d

// block 0048c20d
0048c20d  mov        ecx, dword ptr [ebp + 0x10]
0048c210  cmp        ecx, dword ptr [eax + 0xac]
0048c216  jb         0x48c238

// block 0048c218
0048c218  cmp        byte ptr [esi + 1], bl
0048c21b  je         0x48c238

// block 0048c21d
0048c21d  mov        eax, dword ptr [eax + 0xac]
0048c223  cmp        byte ptr [ebp - 4], bl
0048c226  je         0x48c191

// block 0048c22c
0048c22c  mov        ecx, dword ptr [ebp - 8]
0048c22f  and        dword ptr [ecx + 0x70], 0xfffffffd
0048c233  jmp        0x48c191

// block 0048c238
0048c238  call       0x46e96f

// block 0048c23d
0048c23d  mov        dword ptr [eax], 0x2a
0048c243  cmp        byte ptr [ebp - 4], bl
0048c246  je         0x48c24f

// block 0048c248
0048c248  mov        eax, dword ptr [ebp - 8]
0048c24b  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048c24f
0048c24f  or         eax, 0xffffffff
0048c252  jmp        0x48c191

// block 0048c257
0048c257  xor        eax, eax
0048c259  cmp        dword ptr [ebp + 8], ebx
0048c25c  setne      al
0048c25f  push       eax
0048c260  push       dword ptr [ebp + 8]
0048c263  mov        eax, dword ptr [ebp - 0x10]
0048c266  push       1
0048c268  push       esi
0048c269  push       9
0048c26b  push       dword ptr [eax + 4]
0048c26e  call       dword ptr [0x4980dc]

// block 0048c274
0048c274  test       eax, eax
0048c276  jne        0x48c1b6

// block 0048c27c
0048c27c  jmp        0x48c238
