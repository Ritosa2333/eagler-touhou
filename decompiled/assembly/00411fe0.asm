
// block 00411fe0
00411fe0  mov        ecx, dword ptr [eax + 0x14]
00411fe3  xor        ecx, dword ptr [esp + 4]
00411fe7  and        ecx, 1
00411fea  xor        dword ptr [eax + 0x14], ecx
00411fed  ret        4
