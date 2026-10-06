
// block 004956e8
004956e8  mov        eax, dword ptr [esp + 8]
004956ec  and        eax, 0x7ff00000
004956f1  cmp        eax, 0x7ff00000
004956f6  je         0x4956f9

// block 004956f8
004956f8  ret        

// block 004956f9
004956f9  mov        eax, dword ptr [esp + 8]
004956fd  ret        
