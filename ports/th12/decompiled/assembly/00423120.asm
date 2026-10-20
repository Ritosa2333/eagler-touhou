
// block 00423120
00423120  mov        ecx, dword ptr [eax + 0xa0]
00423126  xor        ecx, dword ptr [esp + 4]
0042312a  and        ecx, 1
0042312d  xor        dword ptr [eax + 0xa0], ecx
00423133  ret        4
