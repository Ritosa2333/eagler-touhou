
// block 00439560
00439560  push       esi
00439561  mov        esi, dword ptr [ecx]
00439563  add        esi, dword ptr [edx]
00439565  mov        ecx, dword ptr [ecx + 4]
00439568  add        ecx, dword ptr [edx + 4]
0043956b  mov        dword ptr [eax], esi
0043956d  mov        dword ptr [eax + 4], ecx
00439570  pop        esi
00439571  ret        
