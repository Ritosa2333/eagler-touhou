
// block 00459450
00459450  push       esi
00459451  mov        esi, dword ptr [ecx]
00459453  add        esi, dword ptr [edx]
00459455  mov        dword ptr [eax], esi
00459457  mov        esi, dword ptr [ecx + 4]
0045945a  add        esi, dword ptr [edx + 4]
0045945d  mov        ecx, dword ptr [ecx + 8]
00459460  add        ecx, dword ptr [edx + 8]
00459463  mov        dword ptr [eax + 4], esi
00459466  mov        dword ptr [eax + 8], ecx
00459469  pop        esi
0045946a  ret        
