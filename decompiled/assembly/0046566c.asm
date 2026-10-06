
// block 0046566c
0046566c  push       edx
0046566d  push       eax
0046566e  mov        eax, dword ptr [ecx + 0x18]
00465671  call       eax

// block 00465673
00465673  test       eax, eax
00465675  jl         0x46567f

// block 00465677
00465677  push       esi
00465678  call       0x465690

// block 0046567d
0046567d  xor        eax, eax
