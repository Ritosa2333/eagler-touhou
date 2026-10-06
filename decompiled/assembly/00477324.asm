
// block 00477324
00477324  mov        edi, edi
00477326  push       ebp
00477327  mov        ebp, esp
00477329  mov        eax, dword ptr [ebp + 8]
0047732c  push       esi
0047732d  xor        esi, esi
0047732f  cmp        eax, esi
00477331  jne        0x477350

// block 00477333
00477333  call       0x46e96f

// block 00477338
00477338  push       esi
00477339  push       esi
0047733a  push       esi
0047733b  push       esi
0047733c  push       esi
0047733d  mov        dword ptr [eax], 0x16
00477343  call       0x470f2d

// block 00477348
00477348  add        esp, 0x14
0047734b  push       0x16
0047734d  pop        eax
0047734e  jmp        0x47735a

// block 00477350
00477350  mov        ecx, dword ptr [0x4adaf8]
00477356  mov        dword ptr [eax], ecx
00477358  xor        eax, eax

// block 0047735a
0047735a  pop        esi
0047735b  pop        ebp
0047735c  ret        
