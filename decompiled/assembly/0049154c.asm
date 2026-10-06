
// block 0049154c
0049154c  cmp        dword ptr [ecx + 0x24], 0x10
00491550  jb         0x491556

// block 00491552
00491552  mov        eax, dword ptr [ecx + 0x10]
00491555  ret        

// block 00491556
00491556  lea        eax, [ecx + 0x10]
00491559  ret        
