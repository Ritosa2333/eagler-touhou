
// block 0048487a
0048487a  mov        edi, edi
0048487c  push       ebp
0048487d  mov        ebp, esp
0048487f  mov        eax, dword ptr [ebp + 8]
00484882  test       eax, eax
00484884  jne        0x48488c

// block 00484886
00484886  pop        ebp
00484887  jmp        0x484851

// block 0048488c
0048488c  mov        eax, dword ptr [eax]
0048488e  mov        eax, dword ptr [eax + 0xac]
00484894  pop        ebp
00484895  ret        
