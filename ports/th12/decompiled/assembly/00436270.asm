
// block 00436270
00436270  push       -1
00436272  push       0x4970b9
00436277  mov        eax, dword ptr fs:[0]
0043627d  push       eax
0043627e  push       ebx
0043627f  push       ebp
00436280  push       esi
00436281  push       edi
00436282  mov        eax, dword ptr [0x4ad138]
00436287  xor        eax, esp
00436289  push       eax
0043628a  lea        eax, [esp + 0x14]
0043628e  mov        dword ptr fs:[0], eax
00436294  mov        ebp, dword ptr [esp + 0x24]
00436298  mov        dword ptr [esp + 0x1c], 1
004362a0  mov        esi, dword ptr [ebp + 8]
004362a3  mov        edi, dword ptr [0x4ce89c]
004362a9  mov        ebx, dword ptr [0x498088]
004362af  test       esi, esi
004362b1  je         0x4362f2

// block 004362b3
004362b3  test       dword ptr [0x4cee78], 0x8000
004362bd  je         0x4362cc

// block 004362bf
004362bf  push       0x4cf0f8
004362c4  call       ebx

// block 004362c6
004362c6  inc        byte ptr [0x4cf218]

// block 004362cc
004362cc  mov        ecx, esi
004362ce  mov        edx, edi
004362d0  call       0x462890

// block 004362d5
004362d5  test       dword ptr [0x4cee78], 0x8000
004362df  je         0x4362f2

// block 004362e1
004362e1  push       0x4cf0f8
004362e6  call       dword ptr [0x49808c]

// block 004362ec
004362ec  dec        byte ptr [0x4cf218]

// block 004362f2
004362f2  mov        esi, dword ptr [ebp + 0xc]
004362f5  mov        edi, dword ptr [0x4ce89c]
004362fb  test       esi, esi
004362fd  je         0x43633e

// block 004362ff
004362ff  test       dword ptr [0x4cee78], 0x8000
00436309  je         0x436318

// block 0043630b
0043630b  push       0x4cf0f8
00436310  call       ebx

// block 00436312
00436312  inc        byte ptr [0x4cf218]

// block 00436318
00436318  mov        ecx, esi
0043631a  mov        edx, edi
0043631c  call       0x462890

// block 00436321
00436321  test       dword ptr [0x4cee78], 0x8000
0043632b  je         0x43633e

// block 0043632d
0043632d  push       0x4cf0f8
00436332  call       dword ptr [0x49808c]

// block 00436338
00436338  dec        byte ptr [0x4cf218]

// block 0043633e
0043633e  test       byte ptr [0x4b0ce0], 1
00436345  mov        esi, dword ptr [0x4ce8cc]
0043634b  mov        dword ptr [0x4b4514], 0
00436355  je         0x43636c

// block 00436357
00436357  mov        edx, dword ptr [ebp + 0x10]
0043635a  call       0x461be0

// block 0043635f
0043635f  mov        eax, dword ptr [ebp + 0xa2c]
00436365  mov        dword ptr [0x4ce8a8], eax
0043636a  jmp        0x4363b9

// block 0043636c
0043636c  mov        edi, dword ptr [esi + 0x4b50dc]
00436372  add        esi, 0x4b50dc
00436378  test       edi, edi
0043637a  je         0x436392

// block 0043637c
0043637c  call       0x4604e0

// block 00436381
00436381  mov        ecx, dword ptr [esi]
00436383  push       ecx
00436384  call       0x46ca4f

// block 00436389
00436389  add        esp, 4
0043638c  mov        dword ptr [esi], 0

// block 00436392
00436392  mov        eax, dword ptr [ebp + 0xa2c]
00436398  test       eax, eax
0043639a  je         0x4363af

// block 0043639c
0043639c  push       eax
0043639d  call       0x46c941

// block 004363a2
004363a2  add        esp, 4
004363a5  mov        dword ptr [ebp + 0xa2c], 0

// block 004363af
004363af  mov        dword ptr [0x4ce8a8], 0

// block 004363b9
004363b9  mov        eax, dword ptr [ebp + 0x940]
004363bf  test       eax, eax
004363c1  je         0x4363cc

// block 004363c3
004363c3  push       eax
004363c4  call       0x46c941

// block 004363c9
004363c9  add        esp, 4

// block 004363cc
004363cc  xor        ecx, ecx
004363ce  mov        dword ptr [ebp + 0x940], ecx
004363d4  mov        eax, dword ptr [ebp + 0x48c]
004363da  cmp        eax, ecx
004363dc  je         0x4363f3

// block 004363de
004363de  push       eax
004363df  call       0x46c941

// block 004363e4
004363e4  add        esp, 4
004363e7  mov        dword ptr [ebp + 0x48c], 0
004363f1  jmp        0x4363f9

// block 004363f3
004363f3  mov        dword ptr [ebp + 0x48c], ecx

// block 004363f9
004363f9  mov        ecx, dword ptr [esp + 0x14]
004363fd  mov        dword ptr fs:[0], ecx
00436404  pop        ecx
00436405  pop        edi
00436406  pop        esi
00436407  pop        ebp
00436408  pop        ebx
00436409  add        esp, 0xc
0043640c  ret        4
