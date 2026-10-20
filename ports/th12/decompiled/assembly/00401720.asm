
// block 00401720
00401720  sub        esp, 0x104
00401726  mov        eax, dword ptr [0x4ad138]
0040172b  xor        eax, esp
0040172d  mov        dword ptr [esp + 0x100], eax
00401734  mov        ecx, dword ptr [esp + 0x108]
0040173b  lea        eax, [esp + 0x10c]
00401742  push       eax
00401743  push       ecx
00401744  lea        edx, [esp + 8]
00401748  push       edx
00401749  call       0x46cad8

// block 0040174e
0040174e  add        esp, 0xc
00401751  lea        eax, [esp]
00401754  mov        ecx, esi
00401756  call       0x4014f0

// block 0040175b
0040175b  mov        eax, dword ptr [esi + 0x18f7c]
00401761  mov        ecx, dword ptr [esp + 0x100]
00401768  imul       eax, eax, 0x138
0040176e  xor        ecx, esp
00401770  mov        dword ptr [eax + esi + 0x964], 1
0040177b  call       0x46c932

// block 00401780
00401780  add        esp, 0x104
00401786  ret        
