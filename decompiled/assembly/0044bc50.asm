
// block 0044bc50
0044bc50  push       esi
0044bc51  mov        esi, ecx
0044bc53  mov        eax, dword ptr [esi]
0044bc55  test       eax, eax
0044bc57  je         0x44bc68

// block 0044bc59
0044bc59  push       eax
0044bc5a  call       0x46c941

// block 0044bc5f
0044bc5f  add        esp, 4
0044bc62  mov        dword ptr [esi], 0

// block 0044bc68
0044bc68  pop        esi
0044bc69  ret        
