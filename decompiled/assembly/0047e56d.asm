
// block 0047e56d
0047e56d  mov        edi, edi
0047e56f  push       ebp
0047e570  mov        ebp, esp
0047e572  xor        eax, eax
0047e574  push       esi
0047e575  mov        esi, ecx
0047e577  mov        byte ptr [esi + 4], al
0047e57a  and        dword ptr [esi + 4], 0xffff00ff
0047e581  mov        dword ptr [esi], eax
0047e583  cmp        byte ptr [ebp + 8], al
0047e586  je         0x47e593

// block 0047e588
0047e588  push       1
0047e58a  lea        eax, [ebp + 8]
0047e58d  push       eax
0047e58e  call       0x47e4f1

// block 0047e593
0047e593  mov        eax, esi
0047e595  pop        esi
0047e596  pop        ebp
0047e597  ret        4
