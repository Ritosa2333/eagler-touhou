
// block 0048e1cf
0048e1cf  mov        edi, edi
0048e1d1  push       ebp
0048e1d2  mov        ebp, esp
0048e1d4  mov        eax, dword ptr [ebp + 0x10]
0048e1d7  mov        ecx, dword ptr [ebp + 0xc]
0048e1da  and        eax, 0xfff7ffff
0048e1df  and        ecx, eax
0048e1e1  push       esi
0048e1e2  test       ecx, 0xfcf0fce0
0048e1e8  je         0x48e21b

// block 0048e1ea
0048e1ea  push       edi
0048e1eb  mov        edi, dword ptr [ebp + 8]
0048e1ee  xor        esi, esi
0048e1f0  cmp        edi, esi
0048e1f2  je         0x48e1ff

// block 0048e1f4
0048e1f4  push       esi
0048e1f5  push       esi
0048e1f6  call       0x4901bb

// block 0048e1fb
0048e1fb  pop        ecx
0048e1fc  pop        ecx
0048e1fd  mov        dword ptr [edi], eax

// block 0048e1ff
0048e1ff  call       0x46e96f

// block 0048e204
0048e204  push       0x16
0048e206  pop        edi
0048e207  push       esi
0048e208  push       esi
0048e209  push       esi
0048e20a  push       esi
0048e20b  push       esi
0048e20c  mov        dword ptr [eax], edi
0048e20e  call       0x470f2d

// block 0048e213
0048e213  add        esp, 0x14
0048e216  mov        eax, edi
0048e218  pop        edi
0048e219  jmp        0x48e238

// block 0048e21b
0048e21b  mov        esi, dword ptr [ebp + 8]
0048e21e  push       eax
0048e21f  push       dword ptr [ebp + 0xc]
0048e222  test       esi, esi
0048e224  je         0x48e22f

// block 0048e226
0048e226  call       0x4901bb

// block 0048e22b
0048e22b  mov        dword ptr [esi], eax
0048e22d  jmp        0x48e234

// block 0048e22f
0048e22f  call       0x4901bb

// block 0048e234
0048e234  pop        ecx
0048e235  pop        ecx
0048e236  xor        eax, eax

// block 0048e238
0048e238  pop        esi
0048e239  pop        ebp
0048e23a  ret        
