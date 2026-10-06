
// block 0049568c
0049568c  test       eax, 0x80000
00495691  je         0x495699

// block 00495693
00495693  mov        eax, 7
00495698  ret        

// block 00495699
00495699  fadd       qword ptr [0x4a5dd0]
0049569f  mov        eax, 1
004956a4  ret        
