
// block 004701f7
004701f7  mov        edi, edi
004701f9  push       ebp
004701fa  mov        ebp, esp
004701fc  mov        eax, dword ptr [ebp + 8]
004701ff  add        dword ptr [eax], 8
00470202  mov        ecx, dword ptr [eax]
00470204  mov        eax, dword ptr [ecx - 8]
00470207  mov        edx, dword ptr [ecx - 4]
0047020a  pop        ebp
0047020b  ret        
