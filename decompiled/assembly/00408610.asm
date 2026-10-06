
// block 00408610
00408610  mov        edx, dword ptr [eax]
00408612  mov        dword ptr [ecx], edx
00408614  mov        edx, dword ptr [eax + 4]
00408617  mov        dword ptr [ecx + 4], edx
0040861a  mov        eax, dword ptr [eax + 8]
0040861d  mov        dword ptr [ecx + 8], eax
00408620  ret        
