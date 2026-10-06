
// block 004956a5
004956a5  mov        eax, dword ptr [edx + 4]
004956a8  and        eax, 0x7ff00000
004956ad  cmp        eax, 0x7ff00000
004956b2  je         0x4956b7

// block 004956b4
004956b4  fld        qword ptr [edx]
004956b6  ret        

// block 004956b7
004956b7  mov        eax, dword ptr [edx + 4]
004956ba  sub        esp, 0xa
004956bd  or         eax, 0x7fff0000
004956c2  mov        dword ptr [esp + 6], eax
004956c6  mov        eax, dword ptr [edx + 4]
004956c9  mov        ecx, dword ptr [edx]
004956cb  shld       eax, ecx, 0xb
004956cf  shl        ecx, 0xb
004956d2  mov        dword ptr [esp + 4], eax
004956d6  mov        dword ptr [esp], ecx
004956d9  fld        xword ptr [esp]
004956dc  add        esp, 0xa
004956df  test       eax, 0
004956e4  mov        eax, dword ptr [edx + 4]
004956e7  ret        
