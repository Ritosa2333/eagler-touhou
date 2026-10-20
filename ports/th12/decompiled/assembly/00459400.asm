
// block 00459400
00459400  push       esi
00459401  mov        esi, dword ptr [edx]
00459403  sub        esi, dword ptr [ecx]
00459405  mov        dword ptr [eax], esi
00459407  mov        esi, dword ptr [edx + 4]
0045940a  sub        esi, dword ptr [ecx + 4]
0045940d  mov        edx, dword ptr [edx + 8]
00459410  sub        edx, dword ptr [ecx + 8]
00459413  mov        dword ptr [eax + 4], esi
00459416  mov        dword ptr [eax + 8], edx
00459419  pop        esi
0045941a  ret        
