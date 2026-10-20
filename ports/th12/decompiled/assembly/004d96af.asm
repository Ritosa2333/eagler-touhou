
// block 004d96af
004d96af  in         eax, 0xc9

// block 004d96b1
004d96b1  popfd      
004d96b2  xchg       ebx, eax
004d96b3  lodsd      eax, dword ptr [esi]
004d96b4  popfd      
004d96b5  jns        0x4d964d

// block 004d96b7
004d96b7  aaa        
004d96b8  xor        esi, ecx
004d96ba  leave      
004d96bb  xchg       ebp, eax
004d96bc  jb         0x4d96b1

// block 004d96be
004d96be  push       esp
004d96bf  imul       edx, dword ptr [esi], 4
004d96c2  and        eax, dword ptr [eax]
004d96c4  test       eax, 0x89cc088b

// block 004d96c9
004d96c9  push       0xf2b2465e
004d96ce  aaa        
004d96cf  dec        edi
004d96d0  inc        eax
004d96d1  test       eax, 0xbb0b2fc0
004d96d6  loopne     0x4d9707

// block 004d96d8
004d96d8  xor        bh, byte ptr [esi]
004d96da  cmpsb      byte ptr [esi], byte ptr es:[edi]
004d96db  cli        

// block 004d96dc
004d96dc  in         al, 0xf5
004d96de  pop        ecx
004d96df  jge        0x4d9744

// block 004d96e1
004d96e1  fisub      word ptr [eax - 0x22f61663]
004d96e7  call       0x22eeacbc

// block 004d96ec
004d96ec  in         eax, 0xdd
004d96ee  cmp        edx, ebp
004d96f0  test       byte ptr [esi + ecx*8], ah

// block 004d9744
004d9744  sub        eax, 0x971e0fa0
004d9749  mov        ecx, 0x24223e54
004d974e  push       -0x1b
004d9750  push       ebx
004d9751  rcl        byte ptr [eax], 0x87
004d9754  inc        esp
004d9755  sbb        dword ptr [ecx - 0x34], edi
004d9758  jns        0x4d97af

// block 004d975a
004d975a  inc        esp
004d975b  in         eax, dx
004d975d  daa        
004d975e  adc        al, byte ptr [ecx]
004d9760  mov        al, 2
004d9762  sbb        dword ptr [ebp + 0x6f559d99], edi

// block 004d9768
004d9768  aam        0xac
004d976a  mov        esi, edi
004d976c  fcom       dword ptr [ecx + 0x49613a41]
004d9772  pop        ebx

// block 004d97af
004d97af  pop        esi
004d97b0  pop        esi
004d97b1  push       esi
004d97b2  fsub       st(7)
004d97b5  leave      
004d97b6  stosd      dword ptr es:[edi], eax
004d97b7  rol        byte ptr [edx - 0x7e], cl
004d97ba  pop        ecx
004d97bb  leave      
