
// block 0047dcc2
0047dcc2  mov        edi, edi
0047dcc4  push       ebp
0047dcc5  mov        ebp, esp
0047dcc7  mov        eax, ecx
0047dcc9  mov        ecx, dword ptr [ebp + 8]
0047dccc  mov        edx, dword ptr [ecx]
0047dcce  mov        dword ptr [eax], edx
0047dcd0  mov        ecx, dword ptr [ecx + 4]
0047dcd3  mov        dword ptr [eax + 4], ecx
0047dcd6  pop        ebp
0047dcd7  ret        4
