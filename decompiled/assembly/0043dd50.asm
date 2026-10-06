
// block 0043dd50
0043dd50  push       esi
0043dd51  push       0x80c
0043dd56  call       0x46c9ea

// block 0043dd5b
0043dd5b  mov        esi, eax
0043dd5d  add        esp, 4
0043dd60  test       esi, esi
0043dd62  je         0x43dd9d

// block 0043dd64
0043dd64  lea        ecx, [esi + 0x18]
0043dd67  call       0x4027e0

// block 0043dd6c
0043dd6c  mov        ecx, 0xc
0043dd71  lea        eax, [esi + 0x4f8]

// block 0043dd77
0043dd77  and        dword ptr [eax], 0xfffffffe
0043dd7a  add        eax, 0x40
0043dd7d  sub        ecx, 1
0043dd80  jns        0x43dd77

// block 0043dd82
0043dd82  push       0x80c
0043dd87  push       0
0043dd89  push       esi
0043dd8a  call       0x477420

// block 0043dd8f
0043dd8f  add        esp, 0xc
0043dd92  or         dword ptr [esi], 2
0043dd95  mov        dword ptr [0x4b4524], esi
0043dd9b  jmp        0x43dd9f

// block 0043dd9d
0043dd9d  xor        esi, esi

// block 0043dd9f
0043dd9f  mov        eax, esi
0043dda1  call       0x43db40

// block 0043dda6
0043dda6  test       eax, eax
0043dda8  je         0x43ddc1

// block 0043ddaa
0043ddaa  test       esi, esi
0043ddac  je         0x43ddbd

// block 0043ddae
0043ddae  push       esi
0043ddaf  call       0x43dc30

// block 0043ddb4
0043ddb4  push       esi
0043ddb5  call       0x46ca4f

// block 0043ddba
0043ddba  add        esp, 4

// block 0043ddbd
0043ddbd  xor        eax, eax
0043ddbf  pop        esi
0043ddc0  ret        

// block 0043ddc1
0043ddc1  mov        eax, esi
0043ddc3  pop        esi
0043ddc4  ret        
