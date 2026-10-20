
// block 00466280
00466280  push       ecx
00466281  cmp        dword ptr [edi + 4], 0
00466285  jne        0x46628b

// block 00466287
00466287  xor        eax, eax
00466289  pop        ecx
0046628a  ret        

// block 0046628b
0046628b  push       esi
0046628c  xor        esi, esi
0046628e  cmp        esi, dword ptr [edi + 0x10]
00466291  jae        0x4662c6

// block 00466293
00466293  mov        eax, dword ptr [edi + 4]
00466296  cmp        dword ptr [eax + esi*4], 0
0046629a  je         0x4662bd

// block 0046629c
0046629c  mov        ecx, eax
0046629e  mov        dword ptr [esp + 4], 0
004662a6  mov        eax, dword ptr [ecx + esi*4]
004662a9  mov        edx, dword ptr [eax]
004662ab  mov        edx, dword ptr [edx + 0x24]
004662ae  lea        ecx, [esp + 4]
004662b2  push       ecx
004662b3  push       eax
004662b4  call       edx

// block 004662b6
004662b6  test       byte ptr [esp + 4], 1
004662bb  je         0x4662c3

// block 004662bd
004662bd  inc        esi
004662be  cmp        esi, dword ptr [edi + 0x10]
004662c1  jb         0x466293

// block 004662c3
004662c3  cmp        esi, dword ptr [edi + 0x10]

// block 004662c6
004662c6  je         0x4662d1

// block 004662c8
004662c8  mov        eax, dword ptr [edi + 4]
004662cb  mov        eax, dword ptr [eax + esi*4]
004662ce  pop        esi
004662cf  pop        ecx
004662d0  ret        

// block 004662d1
004662d1  call       0x46e60d

// block 004662d6
004662d6  xor        edx, edx
004662d8  div        dword ptr [edi + 0x10]
004662db  mov        ecx, dword ptr [edi + 4]
004662de  pop        esi
004662df  mov        eax, dword ptr [ecx + edx*4]
004662e2  pop        ecx
004662e3  ret        
