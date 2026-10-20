
// block 0040e850
0040e850  cmp        dword ptr [esi + 0x34], 0
0040e854  je         0x40e8a2

// block 0040e856
0040e856  push       edi
0040e857  xor        edi, edi
0040e859  cmp        dword ptr [esi + 0x38], edi
0040e85c  jle        0x40e883

// block 0040e85e
0040e85e  mov        edi, edi

// block 0040e860
0040e860  mov        eax, dword ptr [esi + 0x34]
0040e863  mov        eax, dword ptr [eax + edi*4]
0040e866  test       eax, eax
0040e868  je         0x40e87d

// block 0040e86a
0040e86a  push       eax
0040e86b  call       0x46c941

// block 0040e870
0040e870  mov        ecx, dword ptr [esi + 0x34]
0040e873  add        esp, 4
0040e876  mov        dword ptr [ecx + edi*4], 0

// block 0040e87d
0040e87d  inc        edi
0040e87e  cmp        edi, dword ptr [esi + 0x38]
0040e881  jl         0x40e860

// block 0040e883
0040e883  mov        eax, dword ptr [esi + 0x34]
0040e886  pop        edi
0040e887  test       eax, eax
0040e889  je         0x40e89b

// block 0040e88b
0040e88b  push       eax
0040e88c  call       0x46c941

// block 0040e891
0040e891  add        esp, 4
0040e894  mov        dword ptr [esi + 0x34], 0

// block 0040e89b
0040e89b  mov        dword ptr [esi + 0x34], 0

// block 0040e8a2
0040e8a2  ret        
