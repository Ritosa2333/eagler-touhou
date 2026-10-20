
// block 00412100
00412100  push       edi
00412101  mov        edi, eax
00412103  mov        eax, dword ptr [esi]
00412105  test       eax, eax
00412107  je         0x412126

// block 00412109
00412109  fld        dword ptr [0x4a42c0]
0041210f  sub        esp, 8
00412112  fstp       dword ptr [esp + 4]
00412116  fld        dword ptr [0x4a4060]
0041211c  fstp       dword ptr [esp]
0041211f  push       edi
00412120  push       eax
00412121  call       0x4273f0

// block 00412126
00412126  push       esi
00412127  call       0x412140

// block 0041212c
0041212c  mov        dword ptr [esi], 0
00412132  pop        edi
00412133  ret        
