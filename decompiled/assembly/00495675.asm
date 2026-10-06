
// block 00495675
00495675  mov        edx, dword ptr [esp + 4]
00495679  and        edx, 0x300
0049567f  or         edx, 0x7f
00495682  mov        word ptr [esp + 6], dx
00495687  fldcw      word ptr [esp + 6]
0049568b  ret        
