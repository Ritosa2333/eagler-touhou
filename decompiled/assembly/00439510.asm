
// block 00439510
00439510  push       esi
00439511  mov        esi, eax
00439513  mov        edx, dword ptr [esi]
00439515  mov        esi, dword ptr [esi + 4]
00439518  mov        eax, 0x51eb851f
0043951d  imul       edx
0043951f  sar        edx, 5
00439522  mov        eax, edx
00439524  shr        eax, 0x1f
00439527  add        eax, edx
00439529  mov        dword ptr [ecx], eax
0043952b  mov        eax, 0x51eb851f
00439530  imul       esi
00439532  sar        edx, 5
00439535  mov        eax, edx
00439537  shr        eax, 0x1f
0043953a  add        eax, edx
0043953c  mov        dword ptr [ecx + 4], eax
0043953f  mov        eax, ecx
00439541  pop        esi
00439542  ret        
