
// block 00496a31
00496a31  mov        edi, edi
00496a33  push       ebp
00496a34  mov        ebp, esp
00496a36  mov        eax, dword ptr [ebp + 0xe]
00496a39  shr        eax, 4
00496a3c  and        eax, 0x7ff
00496a41  sub        eax, 0x3fe
00496a46  cwde       
00496a47  pop        ebp
00496a48  ret        
