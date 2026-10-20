
// block 00406850
00406850  mov        eax, 0xfffffffe
00406855  and        dword ptr [esi + 0x24], eax
00406858  and        dword ptr [esi + 0x38], eax
0040685b  lea        ecx, [esi + 0x4c]
0040685e  call       0x4027e0

// block 00406863
00406863  push       0x524
00406868  push       0
0040686a  push       esi
0040686b  call       0x477420

// block 00406870
00406870  or         dword ptr [esi], 2
00406873  add        esp, 0xc
00406876  mov        dword ptr [0x4b43c4], esi
0040687c  mov        eax, esi
0040687e  ret        
