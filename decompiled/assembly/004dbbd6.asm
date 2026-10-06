
// block 004dbbd6
004dbbd6  jae        0x4dbc0c

// block 004dbbd8
004dbbd8  out        dx, al
004dbbd9  and        ebp, ebx
004dbbdb  and        edi, dword ptr [ebx - 0x4b819399]
004dbbe1  pop        ds
004dbbe2  mov        byte ptr [edi + 0x54fa4ec], ah
004dbbe8  int3       
