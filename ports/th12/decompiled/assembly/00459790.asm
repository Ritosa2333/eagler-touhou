
// block 00459790
00459790  movzx      edx, byte ptr [ecx + 2]
00459794  mov        dword ptr [eax + 8], edx
00459797  movzx      edx, byte ptr [ecx + 1]
0045979b  mov        dword ptr [eax + 4], edx
0045979e  movzx      ecx, byte ptr [ecx]
004597a1  mov        dword ptr [eax], ecx
004597a3  ret        
