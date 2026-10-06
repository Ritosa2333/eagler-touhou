
// block 004109d0
004109d0  mov        edx, dword ptr [ecx + 4]
004109d3  test       edx, edx
004109d5  je         0x4109e0

// block 004109d7
004109d7  mov        dword ptr [eax + 4], edx
004109da  mov        edx, dword ptr [ecx + 4]
004109dd  mov        dword ptr [edx + 8], eax

// block 004109e0
004109e0  mov        dword ptr [ecx + 4], eax
004109e3  mov        dword ptr [eax + 8], ecx
004109e6  ret        
