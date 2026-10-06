
// block 0047020c
0047020c  mov        edi, edi
0047020e  push       ebp
0047020f  mov        ebp, esp
00470211  mov        eax, dword ptr [ebp + 8]
00470214  add        dword ptr [eax], 4
00470217  mov        eax, dword ptr [eax]
00470219  mov        ax, word ptr [eax - 4]
0047021d  pop        ebp
0047021e  ret        
