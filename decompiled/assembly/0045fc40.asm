
// block 0045fc40
0045fc40  push       ecx
0045fc41  push       0x4a329c
0045fc46  call       0x4622e0

// block 0045fc4b
0045fc4b  add        esp, 8
0045fc4e  push       eax
0045fc4f  push       edx
0045fc50  call       0x45fc90

// block 0045fc55
0045fc55  test       eax, eax
0045fc57  je         0x45fc81

// block 0045fc59
0045fc59  mov        dword ptr [eax + 0x124], 1
0045fc63  cmp        dword ptr [eax + 0x124], 0
0045fc6a  je         0x45fc81

// block 0045fc6c
0045fc6c  push       edi
0045fc6d  lea        ecx, [ecx]

// block 0045fc70
0045fc70  mov        edi, eax
0045fc72  call       0x45ffc0

// block 0045fc77
0045fc77  cmp        dword ptr [eax + 0x124], 0
0045fc7e  jne        0x45fc70

// block 0045fc80
0045fc80  pop        edi

// block 0045fc81
0045fc81  ret        
