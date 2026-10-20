
// block 0043c570
0043c570  mov        eax, dword ptr [0x4b44e8]
0043c575  test       eax, eax
0043c577  je         0x43c585

// block 0043c579
0043c579  test       byte ptr [eax + 0x60], 4
0043c57d  je         0x43c585

// block 0043c57f
0043c57f  mov        eax, 1
0043c584  ret        

// block 0043c585
0043c585  jmp        0x43bb20
