
// block 00412894
00412894  add        al, byte ptr [eax]
00412896  add        bh, al
00412898  inc        eax
00412899  cmp        byte ptr [eax - 0x3cfffffc], dh
0041289f  int3       
