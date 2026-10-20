
// block 0043cc50
0043cc50  mov        ecx, dword ptr [eax + 0x18a8]
0043cc56  xor        edx, edx
0043cc58  push       esi
0043cc59  cmp        ecx, edx
0043cc5b  je         0x43cc66

// block 0043cc5d
0043cc5d  mov        esi, dword ptr [eax + 0x18ac]
0043cc63  mov        dword ptr [ecx + 8], esi

// block 0043cc66
0043cc66  mov        ecx, dword ptr [eax + 0x18ac]
0043cc6c  cmp        ecx, edx
0043cc6e  je         0x43cc79

// block 0043cc70
0043cc70  mov        esi, dword ptr [eax + 0x18a8]
0043cc76  mov        dword ptr [ecx + 4], esi

// block 0043cc79
0043cc79  mov        dword ptr [eax + 0x18ac], edx
0043cc7f  mov        dword ptr [eax + 0x18a8], edx
0043cc85  pop        esi
0043cc86  ret        
