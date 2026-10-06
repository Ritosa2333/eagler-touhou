
// block 0042c720
0042c720  push       esi
0042c721  mov        esi, ecx
0042c723  mov        eax, dword ptr [esi + 0xfa0]
0042c729  test       eax, eax
0042c72b  je         0x42c740

// block 0042c72d
0042c72d  push       eax
0042c72e  call       0x46c941

// block 0042c733
0042c733  add        esp, 4
0042c736  mov        dword ptr [esi + 0xfa0], 0

// block 0042c740
0042c740  mov        eax, dword ptr [esi + 0xf9c]
0042c746  test       eax, eax
0042c748  je         0x42c75d

// block 0042c74a
0042c74a  push       eax
0042c74b  call       0x46c941

// block 0042c750
0042c750  add        esp, 4
0042c753  mov        dword ptr [esi + 0xf9c], 0

// block 0042c75d
0042c75d  xor        eax, eax
0042c75f  pop        esi
0042c760  ret        
