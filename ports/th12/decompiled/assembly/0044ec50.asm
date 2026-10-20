
// block 0044ec50
0044ec50  cmp        dword ptr [ecx + 0x18], 0x10
0044ec54  jb         0x44ec5a

// block 0044ec56
0044ec56  mov        eax, dword ptr [ecx + 4]
0044ec59  ret        

// block 0044ec5a
0044ec5a  lea        eax, [ecx + 4]
0044ec5d  ret        
