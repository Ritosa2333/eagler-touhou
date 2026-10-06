
// block 00465640
00465640  mov        eax, dword ptr [esi]
00465642  test       eax, eax
00465644  je         0x465654

// block 00465646
00465646  mov        ecx, dword ptr [eax]
00465648  mov        edx, dword ptr [ecx + 8]
0046564b  push       eax
0046564c  call       edx

// block 0046564e
0046564e  mov        dword ptr [esi], 0

// block 00465654
00465654  push       0
00465656  push       esi
00465657  push       0
00465659  call       0x46c6b0

// block 0046565e
0046565e  test       eax, eax
00465660  jl         0x46567f

// block 00465662
00465662  mov        eax, dword ptr [esi]
00465664  mov        edx, dword ptr [esp + 4]
00465668  mov        ecx, dword ptr [eax]
0046566a  push       2

// block 0046567f
0046567f  ret        4
