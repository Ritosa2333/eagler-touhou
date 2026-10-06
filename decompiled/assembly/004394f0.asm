
// block 004394f0
004394f0  push       esi
004394f1  mov        esi, dword ptr [edx]
004394f3  mov        edx, dword ptr [edx + 4]
004394f6  imul       esi, ecx
004394f9  imul       edx, ecx
004394fc  mov        dword ptr [eax], esi
004394fe  mov        dword ptr [eax + 4], edx
00439501  pop        esi
00439502  ret        
