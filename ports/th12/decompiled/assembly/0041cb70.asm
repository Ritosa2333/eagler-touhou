
// block 0041cb70
0041cb70  sub        esp, 8
0041cb73  push       esi
0041cb74  mov        esi, dword ptr [0x4b43e0]
0041cb7a  call       0x4508b0

// block 0041cb7f
0041cb7f  fcom       qword ptr [esi + 0x14]
0041cb82  fnstsw     ax
0041cb84  test       ah, 5
0041cb87  jp         0x41cb8c

// block 0041cb89
0041cb89  fst        qword ptr [esi + 0x14]

// block 0041cb8c
0041cb8c  fsub       qword ptr [esi + 0x14]
0041cb8f  fld1       
0041cb91  fcomp      st(1)
0041cb93  fnstsw     ax
0041cb95  test       ah, 0x41
0041cb98  jp         0x41cc5a

// block 0041cb9e
0041cb9e  mov        eax, dword ptr [esi + 0x20]
0041cba1  fld        st(0)
0041cba3  fadd       qword ptr [esi + 0x14]
0041cba6  fstp       qword ptr [esi + 0x14]
0041cba9  fild       dword ptr [esi + 0x20]
0041cbac  test       eax, eax
0041cbae  jge        0x41cbb6

// block 0041cbb0
0041cbb0  fadd       qword ptr [0x4a3d38]

// block 0041cbb6
0041cbb6  fdivrp     st(1)
0041cbb8  fstp       dword ptr [esp + 8]
0041cbbc  fld        dword ptr [esp + 8]
0041cbc0  fst        dword ptr [esi + 0x34]
0041cbc3  fld        dword ptr [0x4a3e50]
0041cbc9  fcomp      st(1)
0041cbcb  fnstsw     ax
0041cbcd  test       ah, 5
0041cbd0  jp         0x41cbd7

// block 0041cbd2
0041cbd2  inc        dword ptr [esi + 0x1c]
0041cbd5  jmp        0x41cbde

// block 0041cbd7
0041cbd7  mov        dword ptr [esi + 0x1c], 0

// block 0041cbde
0041cbde  mov        ecx, dword ptr [0x4b44e8]
0041cbe4  test       ecx, ecx
0041cbe6  je         0x41cc4f

// block 0041cbe8
0041cbe8  test       byte ptr [ecx + 0x60], 0x14
0041cbec  jne        0x41cc3d

// block 0041cbee
0041cbee  fld        qword ptr [esi + 0x2c]
0041cbf1  fld        qword ptr [0x4a3d20]
0041cbf7  fadd       st(1), st(0)
0041cbf9  fxch       st(1)
0041cbfb  fstp       qword ptr [esi + 0x2c]
0041cbfe  fxch       st(1)
0041cc00  fcom       qword ptr [0x4a3e48]
0041cc06  fnstsw     ax
0041cc08  test       ah, 0x41
0041cc0b  jne        0x41cc25

// block 0041cc0d
0041cc0d  fstp       st(0)
0041cc0f  fadd       qword ptr [esi + 0x24]
0041cc12  fstp       qword ptr [esi + 0x24]
0041cc15  and        dword ptr [ecx + 0x60], 0xffffff7f
0041cc1c  mov        dword ptr [esi + 0x20], 0
0041cc23  jmp        0x41cc5c

// block 0041cc25
0041cc25  fstp       st(1)
0041cc27  fadd       qword ptr [esi + 0x24]
0041cc2a  fstp       qword ptr [esi + 0x24]
0041cc2d  and        dword ptr [ecx + 0x60], 0xffffff7f
0041cc34  mov        dword ptr [esi + 0x20], 0
0041cc3b  jmp        0x41cc5c

// block 0041cc3d
0041cc3d  and        dword ptr [ecx + 0x60], 0xffffff7f
0041cc44  fstp       st(0)
0041cc46  mov        dword ptr [esi + 0x20], 0
0041cc4d  jmp        0x41cc5c

// block 0041cc4f
0041cc4f  fstp       st(0)
0041cc51  mov        dword ptr [esi + 0x20], 0
0041cc58  jmp        0x41cc5c

// block 0041cc5a
0041cc5a  fstp       st(0)

// block 0041cc5c
0041cc5c  movzx      ecx, byte ptr [0x4ceace]
0041cc63  inc        ecx
0041cc64  add        dword ptr [esi + 0x20], ecx
0041cc67  mov        eax, 1
0041cc6c  pop        esi
0041cc6d  add        esp, 8
0041cc70  ret        
