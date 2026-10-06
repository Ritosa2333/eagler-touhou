
// block 0043f1e0
0043f1e0  mov        eax, dword ptr [0x4b451c]
0043f1e5  mov        ecx, 0x11111111
0043f1ea  mov        dword ptr [eax + 0x1e9ca], ecx
0043f1f0  push       esi
0043f1f1  mov        dword ptr [eax + 0x1e9ce], ecx
0043f1f7  push       edi
0043f1f8  mov        dword ptr [eax + 0x1e9d2], ecx
0043f1fe  mov        dword ptr [eax + 0x1e9d6], ecx
0043f204  lea        esi, [eax + 0x5b1]
0043f20a  mov        edi, 6
0043f20f  nop        

// block 0043f210
0043f210  mov        eax, esi
0043f212  mov        edx, 4
0043f217  jmp        0x43f220

// block 0043f220
0043f220  mov        ecx, 6

// block 0043f225
0043f225  mov        byte ptr [eax], 1
0043f228  add        eax, 8
0043f22b  sub        ecx, 1
0043f22e  jne        0x43f225

// block 0043f230
0043f230  sub        edx, 1
0043f233  jne        0x43f220

// block 0043f235
0043f235  add        esi, 0x45f4
0043f23b  sub        edi, 1
0043f23e  jne        0x43f210

// block 0043f240
0043f240  pop        edi
0043f241  pop        esi
0043f242  ret        
