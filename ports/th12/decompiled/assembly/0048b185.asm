
// block 0048b185
0048b185  push       0xc
0048b187  push       0x4aaf98
0048b18c  call       0x46fcec

// block 0048b191
0048b191  push       7
0048b193  call       0x46ec9a

// block 0048b198
0048b198  pop        ecx
0048b199  xor        ebx, ebx
0048b19b  mov        dword ptr [ebp - 4], ebx
0048b19e  xor        eax, eax
0048b1a0  mov        edi, dword ptr [ebp + 8]
0048b1a3  cmp        edi, ebx
0048b1a5  setne      al
0048b1a8  cmp        eax, ebx
0048b1aa  jne        0x48b1c8

// block 0048b1ac
0048b1ac  call       0x46e96f

// block 0048b1b1
0048b1b1  push       0x16
0048b1b3  pop        esi
0048b1b4  mov        dword ptr [eax], esi
0048b1b6  push       ebx
0048b1b7  push       ebx
0048b1b8  push       ebx
0048b1b9  push       ebx
0048b1ba  push       ebx
0048b1bb  call       0x470f2d

// block 0048b1c0
0048b1c0  add        esp, 0x14
0048b1c3  mov        dword ptr [ebp - 0x1c], esi
0048b1c6  jmp        0x48b23f

// block 0048b1c8
0048b1c8  mov        dword ptr [edi], ebx
0048b1ca  mov        ecx, dword ptr [ebp + 0xc]
0048b1cd  cmp        ecx, ebx
0048b1cf  je         0x48b1da

// block 0048b1d1
0048b1d1  cmp        dword ptr [ebp + 0x10], ebx
0048b1d4  ja         0x48b1df

// block 0048b1d6
0048b1d6  cmp        ecx, ebx
0048b1d8  jne        0x48b1e4

// block 0048b1da
0048b1da  cmp        dword ptr [ebp + 0x10], ebx
0048b1dd  jne        0x48b1e4

// block 0048b1df
0048b1df  xor        eax, eax
0048b1e1  inc        eax
0048b1e2  jmp        0x48b1e6

// block 0048b1e4
0048b1e4  xor        eax, eax

// block 0048b1e6
0048b1e6  cmp        eax, ebx
0048b1e8  je         0x48b1ac

// block 0048b1ea
0048b1ea  cmp        ecx, ebx
0048b1ec  je         0x48b1f0

// block 0048b1ee
0048b1ee  mov        byte ptr [ecx], bl

// block 0048b1f0
0048b1f0  push       dword ptr [ebp + 0x14]
0048b1f3  call       0x48af42

// block 0048b1f8
0048b1f8  pop        ecx
0048b1f9  mov        esi, eax
0048b1fb  cmp        esi, ebx
0048b1fd  je         0x48b23c

// block 0048b1ff
0048b1ff  push       esi
0048b200  call       0x475860

// block 0048b205
0048b205  pop        ecx
0048b206  inc        eax
0048b207  mov        dword ptr [edi], eax
0048b209  cmp        dword ptr [ebp + 0x10], ebx
0048b20c  je         0x48b23c

// block 0048b20e
0048b20e  cmp        eax, dword ptr [ebp + 0x10]
0048b211  jbe        0x48b21c

// block 0048b213
0048b213  mov        dword ptr [ebp - 0x1c], 0x22
0048b21a  jmp        0x48b23f

// block 0048b21c
0048b21c  push       esi
0048b21d  push       dword ptr [ebp + 0x10]
0048b220  push       dword ptr [ebp + 0xc]
0048b223  call       0x47a2b9

// block 0048b228
0048b228  add        esp, 0xc
0048b22b  cmp        eax, ebx
0048b22d  je         0x48b23c

// block 0048b22f
0048b22f  push       ebx
0048b230  push       ebx
0048b231  push       ebx
0048b232  push       ebx
0048b233  push       ebx
0048b234  call       0x470dc6

// block 0048b239
0048b239  add        esp, 0x14

// block 0048b23c
0048b23c  mov        dword ptr [ebp - 0x1c], ebx

// block 0048b23f
0048b23f  mov        dword ptr [ebp - 4], 0xfffffffe
0048b246  call       0x48b254

// block 0048b24b
0048b24b  mov        eax, dword ptr [ebp - 0x1c]
0048b24e  call       0x46fd31

// block 0048b253
0048b253  ret        
