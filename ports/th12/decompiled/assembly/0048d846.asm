
// block 0048d846
0048d846  push       ecx
0048d847  lea        ecx, [esp + 8]
0048d84b  sub        ecx, eax
0048d84d  and        ecx, 7
0048d850  add        eax, ecx
0048d852  sbb        ecx, ecx
0048d854  or         eax, ecx
0048d856  pop        ecx
0048d857  jmp        0x48d060
