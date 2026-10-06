
// block 004395d0
004395d0  push       esi
004395d1  mov        esi, dword ptr [edx]
004395d3  mov        dword ptr [ecx], esi
004395d5  mov        edx, dword ptr [edx + 4]
004395d8  mov        dword ptr [ecx + 4], edx
004395db  mov        ecx, dword ptr [ecx + 4]
004395de  mov        edx, esi
004395e0  mov        dword ptr [eax], edx
004395e2  mov        dword ptr [eax + 4], ecx
004395e5  pop        esi
004395e6  ret        
