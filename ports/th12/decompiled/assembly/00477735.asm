
// block 00477735
00477735  mov        edi, edi
00477737  push       esi
00477738  call       0x4753ee

// block 0047773d
0047773d  mov        esi, eax
0047773f  test       esi, esi
00477741  jne        0x477752

// block 00477743
00477743  call       0x46e96f

// block 00477748
00477748  mov        dword ptr [eax], 0xc
0047774e  xor        eax, eax
00477750  pop        esi
00477751  ret        

// block 00477752
00477752  cmp        dword ptr [esi + 0x44], 0
00477756  jne        0x477767

// block 00477758
00477758  push       0x24
0047775a  call       0x4777ce

// block 0047775f
0047775f  pop        ecx
00477760  mov        dword ptr [esi + 0x44], eax
00477763  test       eax, eax
00477765  je         0x477743

// block 00477767
00477767  mov        eax, dword ptr [esi + 0x44]
0047776a  pop        esi
0047776b  ret        
