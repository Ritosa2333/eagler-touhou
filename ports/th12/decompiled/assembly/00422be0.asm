
// block 00422be0
00422be0  test       byte ptr [ecx + 0x60], 4
00422be4  jne        0x422c05

// block 00422be6
00422be6  mov        eax, dword ptr [0x4ce8cc]
00422beb  xor        ecx, ecx
00422bed  mov        dword ptr [eax + 0xa4], ecx
00422bf3  mov        dword ptr [eax + 0xa8], ecx
00422bf9  mov        dword ptr [eax + 0xa0], ecx
00422bff  mov        dword ptr [eax + 0xac], ecx

// block 00422c05
00422c05  mov        eax, 1
00422c0a  ret        
