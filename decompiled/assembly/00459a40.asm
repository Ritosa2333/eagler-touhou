
// block 00459a40
00459a40  movzx      eax, al
00459a43  movzx      ecx, cl
00459a46  imul       eax, ecx
00459a49  shr        eax, 7
00459a4c  cmp        eax, 0x100
00459a51  jb         0x459a58

// block 00459a53
00459a53  mov        eax, 0xff

// block 00459a58
00459a58  ret        
