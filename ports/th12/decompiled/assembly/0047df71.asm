
// block 0047df71
0047df71  mov        edi, edi
0047df73  push       ebp
0047df74  mov        ebp, esp
0047df76  mov        eax, ecx
0047df78  mov        ecx, dword ptr [ebp + 8]
0047df7b  or         dword ptr [eax + 0xc], 0xffffffff
0047df7f  mov        dword ptr [eax + 4], ecx
0047df82  mov        ecx, dword ptr [ebp + 0xc]
0047df85  mov        dword ptr [eax], 0x49dde0
0047df8b  mov        dword ptr [eax + 8], ecx
0047df8e  pop        ebp
0047df8f  ret        8
