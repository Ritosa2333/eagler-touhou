
// block 00439490
00439490  push       esi
00439491  mov        esi, dword ptr [edx]
00439493  add        dword ptr [ecx], esi
00439495  mov        edx, dword ptr [edx + 4]
00439498  add        dword ptr [ecx + 4], edx
0043949b  mov        edx, dword ptr [ecx]
0043949d  mov        ecx, dword ptr [ecx + 4]
004394a0  mov        dword ptr [eax], edx
004394a2  mov        dword ptr [eax + 4], ecx
004394a5  pop        esi
004394a6  ret        
