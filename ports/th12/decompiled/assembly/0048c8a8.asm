
// block 0048c8a8
0048c8a8  mov        ecx, dword ptr [esp + 4]
0048c8ac  test       dword ptr [ecx + 4], 6
0048c8b3  mov        eax, 1
0048c8b8  je         0x48c8ec

// block 0048c8ba
0048c8ba  mov        eax, dword ptr [esp + 0x14]
0048c8be  mov        ecx, dword ptr [eax - 4]
0048c8c1  xor        ecx, eax
0048c8c3  call       0x46c932

// block 0048c8ec
0048c8ec  ret        
