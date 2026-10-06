
// block 004080f0
004080f0  push       ecx
004080f1  push       esi
004080f2  push       edi
004080f3  mov        edi, eax
004080f5  mov        esi, 8
004080fa  lea        ebx, [ebx]

// block 00408100
00408100  call       0x408030

// block 00408105
00408105  add        edi, 0xb4
0040810b  sub        esi, 1
0040810e  jne        0x408100

// block 00408110
00408110  pop        edi
00408111  pop        esi
00408112  pop        ecx
00408113  ret        
