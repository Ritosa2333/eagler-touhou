
// block 0049213c
0049213c  mov        eax, dword ptr [eax]
0049213e  cmp        dword ptr [eax], 0xe06d7363
00492144  jne        0x49217e

// block 00492146
00492146  cmp        dword ptr [eax + 0x10], 3
0049214a  jne        0x49217e

// block 0049214c
0049214c  mov        ecx, dword ptr [eax + 0x14]
0049214f  cmp        ecx, 0x19930520
00492155  je         0x492167

// block 00492157
00492157  cmp        ecx, 0x19930521
0049215d  je         0x492167

// block 0049215f
0049215f  cmp        ecx, 0x19930522
00492165  jne        0x49217e

// block 00492167
00492167  cmp        dword ptr [eax + 0x1c], 0
0049216b  jne        0x49217e

// block 0049216d
0049216d  call       0x475467

// block 00492172
00492172  xor        ecx, ecx
00492174  inc        ecx
00492175  mov        dword ptr [eax + 0x20c], ecx
0049217b  mov        eax, ecx
0049217d  ret        

// block 0049217e
0049217e  xor        eax, eax
00492180  ret        
