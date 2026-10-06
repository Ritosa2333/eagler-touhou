
// block 0040d7f0
0040d7f0  mov        eax, dword ptr [ecx + 0x80]
0040d7f6  cmp        eax, 0x1869f
0040d7fb  jge        0x40d804

// block 0040d7fd
0040d7fd  inc        eax
0040d7fe  mov        dword ptr [ecx + 0x80], eax

// block 0040d804
0040d804  ret        
