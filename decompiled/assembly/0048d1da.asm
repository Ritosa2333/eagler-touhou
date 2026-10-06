
// block 0048d1da
0048d1da  push       0x14
0048d1dc  push       0x4ab120
0048d1e1  call       0x46fcec

// block 0048d1e6
0048d1e6  xor        edi, edi
0048d1e8  mov        dword ptr [ebp - 0x1c], edi
0048d1eb  mov        dword ptr [ebp - 0x24], edi
0048d1ee  push       1
0048d1f0  call       0x46ec9a

// block 0048d1f5
0048d1f5  pop        ecx
0048d1f6  mov        dword ptr [ebp - 4], edi
0048d1f9  xor        esi, esi

// block 0048d1fb
0048d1fb  mov        dword ptr [ebp - 0x20], esi
0048d1fe  cmp        esi, dword ptr [0x4d6300]
0048d204  jge        0x48d28d

// block 0048d20a
0048d20a  mov        eax, dword ptr [0x4d52e0]
0048d20f  lea        eax, [eax + esi*4]
0048d212  cmp        dword ptr [eax], edi
0048d214  je         0x48d274

// block 0048d216
0048d216  mov        eax, dword ptr [eax]
0048d218  test       byte ptr [eax + 0xc], 0x83
0048d21c  je         0x48d274

// block 0048d21e
0048d21e  push       eax
0048d21f  push       esi
0048d220  call       0x47c13d

// block 0048d225
0048d225  pop        ecx
0048d226  pop        ecx
0048d227  xor        edx, edx
0048d229  inc        edx
0048d22a  mov        dword ptr [ebp - 4], edx
0048d22d  mov        eax, dword ptr [0x4d52e0]
0048d232  mov        eax, dword ptr [eax + esi*4]
0048d235  mov        ecx, dword ptr [eax + 0xc]
0048d238  test       cl, 0x83
0048d23b  je         0x48d26c

// block 0048d23d
0048d23d  cmp        dword ptr [ebp + 8], edx
0048d240  jne        0x48d253

// block 0048d242
0048d242  push       eax
0048d243  call       0x48d192

// block 0048d248
0048d248  pop        ecx
0048d249  cmp        eax, -1
0048d24c  je         0x48d26c

// block 0048d24e
0048d24e  inc        dword ptr [ebp - 0x1c]
0048d251  jmp        0x48d26c

// block 0048d253
0048d253  cmp        dword ptr [ebp + 8], edi
0048d256  jne        0x48d26c

// block 0048d258
0048d258  test       cl, 2
0048d25b  je         0x48d26c

// block 0048d25d
0048d25d  push       eax
0048d25e  call       0x48d192

// block 0048d263
0048d263  pop        ecx
0048d264  cmp        eax, -1
0048d267  jne        0x48d26c

// block 0048d269
0048d269  or         dword ptr [ebp - 0x24], eax

// block 0048d26c
0048d26c  mov        dword ptr [ebp - 4], edi
0048d26f  call       0x48d27c

// block 0048d274
0048d274  inc        esi
0048d275  jmp        0x48d1fb

// block 0048d28d
0048d28d  mov        dword ptr [ebp - 4], 0xfffffffe
0048d294  call       0x48d2ab

// block 0048d299
0048d299  cmp        dword ptr [ebp + 8], 1
0048d29d  mov        eax, dword ptr [ebp - 0x1c]
0048d2a0  je         0x48d2a5

// block 0048d2a2
0048d2a2  mov        eax, dword ptr [ebp - 0x24]

// block 0048d2a5
0048d2a5  call       0x46fd31

// block 0048d2aa
0048d2aa  ret        
