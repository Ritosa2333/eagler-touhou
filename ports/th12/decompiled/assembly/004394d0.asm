
// block 004394d0
004394d0  push       esi
004394d1  mov        esi, dword ptr [edx]
004394d3  sub        esi, dword ptr [ecx]
004394d5  mov        edx, dword ptr [edx + 4]
004394d8  sub        edx, dword ptr [ecx + 4]
004394db  mov        dword ptr [eax], esi
004394dd  mov        dword ptr [eax + 4], edx
004394e0  pop        esi
004394e1  ret        
