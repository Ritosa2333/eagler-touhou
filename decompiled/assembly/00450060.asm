
// block 00450060
00450060  push       ecx
00450061  cmp        dword ptr [0x4b43e0], 0
00450068  je         0x45006f

// block 0045006a
0045006a  call       0x41cb70

// block 0045006f
0045006f  cmp        dword ptr [0x4b43cc], 0
00450076  je         0x45007d

// block 00450078
00450078  call       0x40dd50

// block 0045007d
0045007d  pop        ecx
0045007e  ret        
