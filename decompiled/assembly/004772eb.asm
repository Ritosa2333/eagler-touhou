
// block 004772eb
004772eb  mov        edi, edi
004772ed  push       ebp
004772ee  mov        ebp, esp
004772f0  mov        eax, dword ptr [ebp + 8]
004772f3  push       esi
004772f4  xor        esi, esi
004772f6  cmp        eax, esi
004772f8  jne        0x477317

// block 004772fa
004772fa  call       0x46e96f

// block 004772ff
004772ff  push       esi
00477300  push       esi
00477301  push       esi
00477302  push       esi
00477303  push       esi
00477304  mov        dword ptr [eax], 0x16
0047730a  call       0x470f2d

// block 0047730f
0047730f  add        esp, 0x14
00477312  push       0x16
00477314  pop        eax
00477315  jmp        0x477321

// block 00477317
00477317  mov        ecx, dword ptr [0x4adb00]
0047731d  mov        dword ptr [eax], ecx
0047731f  xor        eax, eax

// block 00477321
00477321  pop        esi
00477322  pop        ebp
00477323  ret        
