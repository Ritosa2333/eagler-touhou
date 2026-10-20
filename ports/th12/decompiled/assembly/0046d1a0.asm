
// block 0046d190
0046d190  lea        eax, [edx - 1]
0046d193  pop        ebx
0046d194  ret        

// block 0046d1a0
0046d1a0  xor        eax, eax
0046d1a2  mov        al, byte ptr [esp + 8]
0046d1a6  push       ebx
0046d1a7  mov        ebx, eax
0046d1a9  shl        eax, 8
0046d1ac  mov        edx, dword ptr [esp + 8]
0046d1b0  test       edx, 3
0046d1b6  je         0x46d1cd

// block 0046d1b8
0046d1b8  mov        cl, byte ptr [edx]
0046d1ba  add        edx, 1
0046d1bd  cmp        cl, bl
0046d1bf  je         0x46d190

// block 0046d1c1
0046d1c1  test       cl, cl
0046d1c3  je         0x46d216

// block 0046d1c5
0046d1c5  test       edx, 3
0046d1cb  jne        0x46d1b8

// block 0046d1cd
0046d1cd  or         ebx, eax
0046d1cf  push       edi
0046d1d0  mov        eax, ebx
0046d1d2  shl        ebx, 0x10
0046d1d5  push       esi
0046d1d6  or         ebx, eax

// block 0046d1d8
0046d1d8  mov        ecx, dword ptr [edx]
0046d1da  mov        edi, 0x7efefeff
0046d1df  mov        eax, ecx
0046d1e1  mov        esi, edi
0046d1e3  xor        ecx, ebx
0046d1e5  add        esi, eax
0046d1e7  add        edi, ecx
0046d1e9  xor        ecx, 0xffffffff
0046d1ec  xor        eax, 0xffffffff
0046d1ef  xor        ecx, edi
0046d1f1  xor        eax, esi
0046d1f3  add        edx, 4
0046d1f6  and        ecx, 0x81010100
0046d1fc  jne        0x46d21a

// block 0046d1fe
0046d1fe  and        eax, 0x81010100
0046d203  je         0x46d1d8

// block 0046d205
0046d205  and        eax, 0x1010100
0046d20a  jne        0x46d214

// block 0046d20c
0046d20c  and        esi, 0x80000000
0046d212  jne        0x46d1d8

// block 0046d214
0046d214  pop        esi
0046d215  pop        edi

// block 0046d216
0046d216  pop        ebx
0046d217  xor        eax, eax
0046d219  ret        

// block 0046d21a
0046d21a  mov        eax, dword ptr [edx - 4]
0046d21d  cmp        al, bl
0046d21f  je         0x46d257

// block 0046d221
0046d221  test       al, al
0046d223  je         0x46d214

// block 0046d225
0046d225  cmp        ah, bl
0046d227  je         0x46d250

// block 0046d229
0046d229  test       ah, ah
0046d22b  je         0x46d214

// block 0046d22d
0046d22d  shr        eax, 0x10
0046d230  cmp        al, bl
0046d232  je         0x46d249

// block 0046d234
0046d234  test       al, al
0046d236  je         0x46d214

// block 0046d238
0046d238  cmp        ah, bl
0046d23a  je         0x46d242

// block 0046d23c
0046d23c  test       ah, ah
0046d23e  je         0x46d214

// block 0046d240
0046d240  jmp        0x46d1d8

// block 0046d242
0046d242  pop        esi
0046d243  pop        edi
0046d244  lea        eax, [edx - 1]
0046d247  pop        ebx
0046d248  ret        

// block 0046d249
0046d249  lea        eax, [edx - 2]
0046d24c  pop        esi
0046d24d  pop        edi
0046d24e  pop        ebx
0046d24f  ret        

// block 0046d250
0046d250  lea        eax, [edx - 3]
0046d253  pop        esi
0046d254  pop        edi
0046d255  pop        ebx
0046d256  ret        

// block 0046d257
0046d257  lea        eax, [edx - 4]
0046d25a  pop        esi
0046d25b  pop        edi
0046d25c  pop        ebx
0046d25d  ret        
