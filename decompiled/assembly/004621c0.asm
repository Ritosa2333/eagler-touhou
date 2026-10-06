
// block 004621c0
004621c0  push       ebp
004621c1  push       esi
004621c2  push       edi
004621c3  mov        edi, dword ptr [0x4ce8cc]
004621c9  mov        eax, dword ptr [edi + 0x4b50bc]
004621cf  mov        ecx, eax
004621d1  imul       ecx, ecx, 0x4b4
004621d7  cmp        byte ptr [eax + edi + 0x4b40bc], 0
004621df  lea        ebp, [ecx + edi + 0xbc]
004621e6  je         0x46225d

// block 004621e8
004621e8  inc        eax
004621e9  and        eax, 0x80000fff
004621ee  jns        0x4621f7

// block 004621f0
004621f0  dec        eax
004621f1  or         eax, 0xfffff000
004621f6  inc        eax

// block 004621f7
004621f7  mov        edx, eax
004621f9  imul       edx, edx, 0x4b4
004621ff  mov        dword ptr [edi + 0x4b50bc], eax
00462205  cmp        byte ptr [eax + edi + 0x4b40bc], 0
0046220d  lea        ebp, [edx + edi + 0xbc]
00462214  je         0x462246

// block 00462216
00462216  push       0x4b4
0046221b  call       0x46c9ea

// block 00462220
00462220  add        esp, 4
00462223  test       eax, eax
00462225  je         0x462239

// block 00462227
00462227  mov        ecx, eax
00462229  call       0x4027e0

// block 0046222e
0046222e  mov        esi, eax
00462230  mov        ebp, eax
00462232  call       0x402520

// block 00462237
00462237  jmp        0x462265

// block 00462239
00462239  xor        eax, eax
0046223b  mov        esi, eax
0046223d  mov        ebp, eax
0046223f  call       0x402520

// block 00462244
00462244  jmp        0x462265

// block 00462246
00462246  mov        esi, ebp
00462248  call       0x402520

// block 0046224d
0046224d  mov        eax, dword ptr [edi + 0x4b50bc]
00462253  mov        byte ptr [edi + eax + 0x4b40bc], 1
0046225b  jmp        0x462265

// block 0046225d
0046225d  mov        byte ptr [eax + edi + 0x4b40bc], 1

// block 00462265
00462265  mov        ecx, dword ptr [edi + 0x4b50bc]
0046226b  inc        ecx
0046226c  and        ecx, 0x80000fff
00462272  jns        0x46227c

// block 00462274
00462274  dec        ecx
00462275  or         ecx, 0xfffff000
0046227b  inc        ecx

// block 0046227c
0046227c  mov        dword ptr [edi + 0x4b50bc], ecx
00462282  pop        edi
00462283  pop        esi
00462284  mov        eax, ebp
00462286  pop        ebp
00462287  ret        
