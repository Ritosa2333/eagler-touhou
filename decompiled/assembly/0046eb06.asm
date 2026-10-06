
// block 0046eb06
0046eb06  mov        edi, edi
0046eb08  push       esi
0046eb09  push       edi
0046eb0a  xor        esi, esi
0046eb0c  mov        edi, 0x4b3c08

// block 0046eb11
0046eb11  cmp        dword ptr [esi*8 + 0x4ad2bc], 1
0046eb19  jne        0x46eb39

// block 0046eb1b
0046eb1b  lea        eax, [esi*8 + 0x4ad2b8]
0046eb22  mov        dword ptr [eax], edi
0046eb24  push       0xfa0
0046eb29  push       dword ptr [eax]
0046eb2b  add        edi, 0x18
0046eb2e  call       0x47b416

// block 0046eb33
0046eb33  pop        ecx
0046eb34  pop        ecx
0046eb35  test       eax, eax
0046eb37  je         0x46eb45

// block 0046eb39
0046eb39  inc        esi
0046eb3a  cmp        esi, 0x24
0046eb3d  jl         0x46eb11

// block 0046eb3f
0046eb3f  xor        eax, eax
0046eb41  inc        eax

// block 0046eb42
0046eb42  pop        edi
0046eb43  pop        esi
0046eb44  ret        

// block 0046eb45
0046eb45  and        dword ptr [esi*8 + 0x4ad2b8], 0
0046eb4d  xor        eax, eax
0046eb4f  jmp        0x46eb42
