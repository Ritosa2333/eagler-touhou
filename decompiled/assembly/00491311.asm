
// block 00491311
00491311  mov        edi, edi
00491313  push       ebp
00491314  mov        ebp, esp
00491316  call       0x491202

// block 0049131b
0049131b  mov        ecx, dword ptr [ebp + 8]
0049131e  and        ecx, 0x3f
00491321  or         eax, ecx
00491323  push       eax
00491324  call       0x491220

// block 00491329
00491329  pop        ecx
0049132a  pop        ebp
0049132b  ret        
