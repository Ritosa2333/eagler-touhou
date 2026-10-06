
// block 0047a10a
0047a10a  mov        edi, edi
0047a10c  push       ebp
0047a10d  mov        ebp, esp
0047a10f  mov        ecx, dword ptr [ebp + 0xc]
0047a112  mov        eax, dword ptr [0x4adba0]
0047a117  mov        edx, dword ptr [ebp + 8]
0047a11a  and        edx, dword ptr [ebp + 0xc]
0047a11d  not        ecx
0047a11f  and        ecx, eax
0047a121  or         ecx, edx
0047a123  mov        dword ptr [0x4adba0], ecx
0047a129  pop        ebp
0047a12a  ret        
