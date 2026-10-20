
// block 0047776c
0047776c  mov        edi, edi
0047776e  push       ebp
0047776f  mov        ebp, esp
00477771  push       esi
00477772  call       0x477735

// block 00477777
00477777  mov        esi, eax
00477779  test       esi, esi
0047777b  je         0x477790

// block 0047777d
0047777d  push       dword ptr [ebp + 8]
00477780  push       esi
00477781  call       0x477602

// block 00477786
00477786  neg        eax
00477788  sbb        eax, eax
0047778a  pop        ecx
0047778b  not        eax
0047778d  pop        ecx
0047778e  and        eax, esi

// block 00477790
00477790  pop        esi
00477791  pop        ebp
00477792  ret        
