
// block 0041b1c0
0041b1c0  sub        esp, 8
0041b1c3  mov        eax, dword ptr [0x4ceaa4]
0041b1c8  push       ebx
0041b1c9  push       ebp
0041b1ca  mov        ebp, dword ptr [0x4b43c8]
0041b1d0  push       esi
0041b1d1  push       edi
0041b1d2  push       eax
0041b1d3  add        ebp, 0x64
0041b1d6  call       0x455630

// block 0041b1db
0041b1db  lea        edi, [ebp + 0x49c]
0041b1e1  mov        dword ptr [esp + 0x10], 0x7d0
0041b1e9  mov        ebx, 1
0041b1ee  mov        edi, edi

// block 0041b1f0
0041b1f0  test       byte ptr [ebp], bl
0041b1f3  je         0x41b446

// block 0041b1f9
0041b1f9  cmp        word ptr [edi + 0x96], bx
0041b200  jne        0x41b446

// block 0041b206
0041b206  mov        eax, dword ptr [esp + 0x1c]
0041b20a  fld        dword ptr [edi + 0x24]
0041b20d  fsub       dword ptr [eax + 0x38]
0041b210  fld        dword ptr [edi + 0x20]
0041b213  fsub       dword ptr [eax + 0x34]
0041b216  fld        dword ptr [eax + 0x1758]
0041b21c  fsub       qword ptr [0x4a3d70]
0041b222  fld        st(1)
0041b224  fmulp      st(2)
0041b226  fld        st(2)
0041b228  fmulp      st(3)
0041b22a  fxch       st(1)
0041b22c  faddp      st(2)
0041b22e  fxch       st(1)
0041b230  fstp       dword ptr [esp + 0x14]
0041b234  fld        dword ptr [esp + 0x14]
0041b238  fld        st(1)
0041b23a  fmulp      st(2)
0041b23c  fcompp     
0041b23e  fnstsw     ax
0041b240  test       ah, 1
0041b243  jne        0x41b32b

// block 0041b249
0041b249  cmp        dword ptr [edi + 0xa4], 0
0041b250  je         0x41b446

// block 0041b256
0041b256  lea        esi, [edi - 0x494]
0041b25c  mov        dword ptr [edi + 0xa4], 0
0041b266  call       0x402520

// block 0041b26b
0041b26b  movzx      eax, word ptr [edi + 0x55a]
0041b272  mov        ecx, 3
0041b277  mov        dword ptr [edi + 0x18], 0x40d430
0041b27e  mov        dword ptr [edi + 0x1c], ebp
0041b281  mov        word ptr [edi + 0x558], cx
0041b288  cmp        ax, bx
0041b28b  jne        0x41b292

// block 0041b28d
0041b28d  lea        eax, [ecx + 3]
0041b290  jmp        0x41b2a4

// block 0041b292
0041b292  xor        edx, edx
0041b294  cmp        ax, 3
0041b298  setne      dl
0041b29b  dec        edx
0041b29c  and        edx, 0xb
0041b29f  add        edx, 2
0041b2a2  mov        eax, edx

// block 0041b2a4
0041b2a4  mov        ecx, dword ptr [0x4b43c8]
0041b2aa  mov        word ptr [edi + 0x55a], ax
0041b2b1  fld        dword ptr [0x4af5b4]
0041b2b7  and        dword ptr [ebp], 0xffffffef
0041b2bb  fstp       dword ptr [esp + 0x14]
0041b2bf  fld        dword ptr [esp + 0x14]
0041b2c3  mov        eax, esi
0041b2c5  fst        dword ptr [edi + 0x44]
0041b2c8  fstp       dword ptr [edi + 0x40]
0041b2cb  mov        edx, dword ptr [0x4af4f0]
0041b2d1  mov        ecx, dword ptr [ecx + 0x4debdc]
0041b2d7  call       0x454ee0

// block 0041b2dc
0041b2dc  movsx      edx, word ptr [edi + 0x558]
0041b2e3  imul       edx, edx, 0xd0
0041b2e9  mov        eax, dword ptr [edx + 0x4af34c]
0041b2ef  cmp        eax, 4
0041b2f2  ja         0x41b42b

// block 0041b2f8
0041b2f8  jmp        dword ptr [eax*4 + 0x41b468]

// block 0041b2ff
0041b2ff  movsx      edx, word ptr [edi + 0x55a]
0041b306  mov        eax, dword ptr [edx*4 + 0x4b0bb0]
0041b30d  mov        dword ptr [edi + 0x88], eax
0041b313  jmp        0x41b42b

// block 0041b318
0041b318  or         dword ptr [ebp], 0x10
0041b31c  mov        dword ptr [edi + 0x88], 0xffffffff
0041b326  jmp        0x41b42b

// block 0041b32b
0041b32b  cmp        dword ptr [edi + 0xa4], 0
0041b332  jne        0x41b446

// block 0041b338
0041b338  fld        dword ptr [edi + 0x20]
0041b33b  push       ecx
0041b33c  mov        eax, 0x28
0041b341  fstp       dword ptr [esp]
0041b344  call       0x453e20

// block 0041b349
0041b349  lea        esi, [edi - 0x494]
0041b34f  mov        dword ptr [edi + 0xa4], ebx
0041b355  call       0x402520

// block 0041b35a
0041b35a  movzx      eax, word ptr [edi + 0x55a]
0041b361  mov        edx, 0x1b
0041b366  mov        dword ptr [edi + 0x18], 0x40d430
0041b36d  mov        dword ptr [edi + 0x1c], ebp
0041b370  mov        word ptr [edi + 0x558], dx
0041b377  cmp        ax, 6
0041b37b  jne        0x41b381

// block 0041b37d
0041b37d  mov        eax, ebx
0041b37f  jmp        0x41b390

// block 0041b381
0041b381  xor        ecx, ecx
0041b383  cmp        ax, 0xd
0041b387  setne      cl
0041b38a  dec        ecx
0041b38b  and        ecx, 3
0041b38e  mov        eax, ecx

// block 0041b390
0041b390  mov        ecx, dword ptr [0x4b43c8]
0041b396  mov        word ptr [edi + 0x55a], ax
0041b39d  fld        dword ptr [0x4b0934]
0041b3a3  fmul       qword ptr [0x4a4208]
0041b3a9  mov        eax, esi
0041b3ab  fstp       dword ptr [esp + 0x14]
0041b3af  fld        dword ptr [esp + 0x14]
0041b3b3  fst        dword ptr [edi + 0x44]
0041b3b6  fstp       dword ptr [edi + 0x40]
0041b3b9  mov        edx, dword ptr [0x4b0870]
0041b3bf  mov        ecx, dword ptr [ecx + 0x4debdc]
0041b3c5  call       0x454ee0

// block 0041b3ca
0041b3ca  movsx      edx, word ptr [edi + 0x558]
0041b3d1  and        dword ptr [ebp], 0xffffffef
0041b3d5  imul       edx, edx, 0xd0
0041b3db  mov        ecx, dword ptr [edx + 0x4af34c]
0041b3e1  mov        eax, dword ptr [ebp]
0041b3e4  cmp        ecx, 4
0041b3e7  ja         0x41b42b

// block 0041b3e9
0041b3e9  jmp        dword ptr [ecx*4 + 0x41b47c]

// block 0041b3f0
0041b3f0  movsx      eax, word ptr [edi + 0x55a]
0041b3f7  lea        ecx, [eax + eax + 4]
0041b3fb  mov        dword ptr [edi + 0x88], ecx
0041b401  jmp        0x41b42b

// block 0041b403
0041b403  or         eax, 0x10
0041b406  mov        dword ptr [edi + 0x88], 0xffffffff
0041b410  mov        dword ptr [ebp], eax
0041b413  jmp        0x41b42b

// block 0041b415
0041b415  mov        dword ptr [edi + 0x88], 0x10
0041b41f  jmp        0x41b42b

// block 0041b421
0041b421  mov        dword ptr [edi + 0x88], 6

// block 0041b42b
0041b42b  mov        eax, dword ptr [edi]
0041b42d  test       eax, eax
0041b42f  je         0x41b43a

// block 0041b431
0041b431  mov        edx, 2
0041b436  mov        ecx, esi
0041b438  call       eax

// block 0041b43a
0041b43a  mov        ecx, 2
0041b43f  mov        word ptr [edi - 0xd0], cx

// block 0041b446
0041b446  add        ebp, 0x9f8
0041b44c  add        edi, 0x9f8
0041b452  sub        dword ptr [esp + 0x10], ebx
0041b456  jne        0x41b1f0

// block 0041b45c
0041b45c  pop        edi
0041b45d  pop        esi
0041b45e  pop        ebp
0041b45f  xor        eax, eax
0041b461  pop        ebx
0041b462  add        esp, 8
0041b465  ret        4
