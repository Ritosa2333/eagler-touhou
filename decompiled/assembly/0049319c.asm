
// block 0049319c
0049319c  mov        edi, edi
0049319e  push       ebp
0049319f  mov        ebp, esp
004931a1  xor        eax, eax
004931a3  inc        eax
004931a4  cmp        dword ptr [ebp + 8], 0
004931a8  jne        0x4931ac

// block 004931aa
004931aa  xor        eax, eax

// block 004931ac
004931ac  pop        ebp
004931ad  ret        
