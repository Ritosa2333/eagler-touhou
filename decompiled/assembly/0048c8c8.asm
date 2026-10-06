
// block 0048c8c8
0048c8c8  push       ebp
0048c8c9  mov        ebp, dword ptr [eax + 0x10]
0048c8cc  mov        edx, dword ptr [eax + 0x28]
0048c8cf  push       edx
0048c8d0  mov        edx, dword ptr [eax + 0x24]
0048c8d3  push       edx
0048c8d4  call       0x48c8ed

// block 0048c8d9
0048c8d9  add        esp, 8
0048c8dc  pop        ebp
0048c8dd  mov        eax, dword ptr [esp + 8]
0048c8e1  mov        edx, dword ptr [esp + 0x10]
0048c8e5  mov        dword ptr [edx], eax
0048c8e7  mov        eax, 3
