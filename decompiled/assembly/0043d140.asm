
// block 0043d140
0043d140  push       ebp
0043d141  mov        ebp, dword ptr [esp + 8]
0043d145  mov        eax, dword ptr [ebp]
0043d148  push       edi
0043d149  test       eax, eax
0043d14b  je         0x43d28f

// block 0043d151
0043d151  cmp        dword ptr [eax], 0x31324854
0043d157  jne        0x43d27f

// block 0043d15d
0043d15d  mov        edi, 2
0043d162  cmp        word ptr [eax + 8], di
0043d166  jne        0x43d27f

// block 0043d16c
0043d16c  mov        ecx, dword ptr [eax + 0x10]
0043d16f  push       ebx
0043d170  push       esi
0043d171  push       ecx
0043d172  push       0x10
0043d174  push       0x35
0043d176  add        eax, 0x18
0043d179  push       ecx
0043d17a  push       eax
0043d17b  mov        al, 0xac
0043d17d  call       0x4639d0

// block 0043d182
0043d182  mov        eax, dword ptr [ebp]
0043d185  lea        esi, [eax + 0x18]
0043d188  mov        eax, dword ptr [eax + 0x14]
0043d18b  add        eax, eax
0043d18d  add        eax, eax
0043d18f  push       eax
0043d190  call       0x46d04a

// block 0043d195
0043d195  mov        ecx, dword ptr [ebp]
0043d198  add        esp, 4
0043d19b  push       eax
0043d19c  mov        dword ptr [ebp + 4], eax
0043d19f  mov        eax, dword ptr [ecx + 0x10]
0043d1a2  push       eax
0043d1a3  mov        eax, dword ptr [ecx + 0x14]
0043d1a6  push       esi
0043d1a7  call       0x44c710

// block 0043d1ac
0043d1ac  mov        ecx, dword ptr [ebp]
0043d1af  mov        esi, dword ptr [ecx + 0x14]
0043d1b2  mov        ebx, dword ptr [ebp + 4]
0043d1b5  mov        dword ptr [esp + 0x14], esi
0043d1b9  test       esi, esi
0043d1bb  jg         0x43d1d5

// block 0043d1bd
0043d1bd  pop        esi
0043d1be  pop        ebx
0043d1bf  pop        edi
0043d1c0  xor        eax, eax
0043d1c2  pop        ebp
0043d1c3  ret        4

// block 0043d1d0
0043d1d0  mov        edi, 2

// block 0043d1d5
0043d1d5  movzx      eax, word ptr [ebx]
0043d1d8  mov        edx, 0x5243
0043d1dd  cmp        ax, dx
0043d1e0  jne        0x43d220

// block 0043d1e2
0043d1e2  cmp        word ptr [ebx + 2], di
0043d1e6  jne        0x43d25d

// block 0043d1e8
0043d1e8  push       0x45f4
0043d1ed  mov        ecx, ebx
0043d1ef  call       0x43cdb0

// block 0043d1f4
0043d1f4  cmp        eax, dword ptr [ebx + 4]
0043d1f7  jne        0x43d25d

// block 0043d1f9
0043d1f9  cmp        dword ptr [ebx + 8], 0x45f4
0043d200  jne        0x43d25d

// block 0043d202
0043d202  mov        eax, dword ptr [ebx + 0xc]
0043d205  imul       eax, eax, 0x45f4
0043d20b  push       0x45f4
0043d210  lea        ecx, [eax + ebp + 8]
0043d214  push       ebx
0043d215  push       ecx
0043d216  call       0x47a330

// block 0043d21b
0043d21b  add        esp, 0xc
0043d21e  jmp        0x43d25d

// block 0043d220
0043d220  mov        edx, 0x5453
0043d225  cmp        ax, dx
0043d228  jne        0x43d1bd

// block 0043d22a
0043d22a  cmp        word ptr [ebx + 2], di
0043d22e  jne        0x43d25d

// block 0043d230
0043d230  push       0x448
0043d235  mov        ecx, ebx
0043d237  call       0x43cdb0

// block 0043d23c
0043d23c  cmp        eax, dword ptr [ebx + 4]
0043d23f  jne        0x43d25d

// block 0043d241
0043d241  cmp        dword ptr [ebx + 8], 0x448
0043d248  jne        0x43d25d

// block 0043d24a
0043d24a  lea        edi, [ebp + 0x1e9b4]
0043d250  mov        ecx, 0x112
0043d255  mov        esi, ebx

// block 0043d257
0043d257  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0043d259
0043d259  mov        esi, dword ptr [esp + 0x14]

// block 0043d25d
0043d25d  mov        eax, dword ptr [ebx + 8]
0043d260  sub        esi, eax
0043d262  mov        dword ptr [esp + 0x14], esi
0043d266  js         0x43d1bd

// block 0043d26c
0043d26c  add        ebx, eax
0043d26e  test       esi, esi
0043d270  jg         0x43d1d0

// block 0043d276
0043d276  pop        esi
0043d277  pop        ebx
0043d278  pop        edi
0043d279  xor        eax, eax
0043d27b  pop        ebp
0043d27c  ret        4

// block 0043d27f
0043d27f  push       eax
0043d280  call       0x46c941

// block 0043d285
0043d285  add        esp, 4
0043d288  mov        dword ptr [ebp], 0

// block 0043d28f
0043d28f  push       0x18
0043d291  call       0x46d04a

// block 0043d296
0043d296  mov        dword ptr [ebp], eax
0043d299  xor        ecx, ecx
0043d29b  mov        dword ptr [eax], ecx
0043d29d  mov        dword ptr [eax + 4], ecx
0043d2a0  mov        dword ptr [eax + 8], ecx
0043d2a3  mov        dword ptr [eax + 0xc], ecx
0043d2a6  mov        dword ptr [eax + 0x10], ecx
0043d2a9  mov        dword ptr [eax + 0x14], ecx
0043d2ac  mov        eax, dword ptr [ebp]
0043d2af  mov        dword ptr [eax], 0x31324854
0043d2b5  mov        ecx, dword ptr [ebp]
0043d2b8  mov        edx, 2
0043d2bd  add        esp, 4
0043d2c0  mov        word ptr [ecx + 8], dx
0043d2c4  mov        eax, dword ptr [ebp]
0043d2c7  pop        edi
0043d2c8  mov        dword ptr [eax + 0xc], 0x100
0043d2cf  xor        eax, eax
0043d2d1  pop        ebp
0043d2d2  ret        4
