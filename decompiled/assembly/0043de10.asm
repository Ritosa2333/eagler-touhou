
// block 0043de10
0043de10  push       ecx
0043de11  fld        qword ptr [0x4a3cf8]
0043de17  push       esi
0043de18  fld        qword ptr [0x4a3db0]
0043de1e  mov        esi, eax
0043de20  fld        dword ptr [0x4a3dac]
0043de26  push       edi
0043de27  fld1       
0043de29  add        esi, 0x4f0
0043de2f  mov        edi, 0xd

// block 0043de34
0043de34  cmp        byte ptr [esi + 0x14], 0
0043de38  je         0x43de9b

// block 0043de3a
0043de3a  fld        dword ptr [esi - 0x14]
0043de3d  fld        dword ptr [0x4b2ed0]
0043de43  fmul       st(5)
0043de45  fsubp      st(1)
0043de47  fstp       dword ptr [esi - 0x14]
0043de4a  mov        edx, dword ptr [esi - 4]
0043de4d  mov        ecx, dword ptr [esi + 4]
0043de50  mov        dword ptr [esi - 8], edx
0043de53  fld        dword ptr [ecx]
0043de55  fcomp      st(3)
0043de57  fnstsw     ax
0043de59  test       ah, 0x41
0043de5c  jne        0x43de77

// block 0043de5e
0043de5e  fxch       st(1)
0043de60  fcom       dword ptr [ecx]
0043de62  fnstsw     ax
0043de64  test       ah, 0x41
0043de67  jne        0x43de75

// block 0043de69
0043de69  fld        dword ptr [esi]
0043de6b  inc        edx
0043de6c  fadd       st(2)
0043de6e  mov        dword ptr [esi - 4], edx
0043de71  fstp       dword ptr [esi]
0043de73  jmp        0x43de8f

// block 0043de75
0043de75  fxch       st(1)

// block 0043de77
0043de77  fld        dword ptr [esi]
0043de79  fadd       dword ptr [ecx]
0043de7b  fstp       dword ptr [esp + 8]
0043de7f  fld        dword ptr [esp + 8]
0043de83  fst        dword ptr [esi]
0043de85  call       0x4931e0

// block 0043de8a
0043de8a  mov        dword ptr [esi - 4], eax
0043de8d  fxch       st(1)

// block 0043de8f
0043de8f  cmp        dword ptr [esi - 4], 0x3c
0043de93  jle        0x43de99

// block 0043de95
0043de95  mov        byte ptr [esi + 0x14], 0

// block 0043de99
0043de99  fxch       st(1)

// block 0043de9b
0043de9b  add        esi, 0x40
0043de9e  sub        edi, 1
0043dea1  jne        0x43de34

// block 0043dea3
0043dea3  fstp       st(3)
0043dea5  lea        eax, [edi + 1]
0043dea8  fstp       st(1)
0043deaa  pop        edi
0043deab  fstp       st(0)
0043dead  pop        esi
0043deae  fstp       st(0)
0043deb0  pop        ecx
0043deb1  ret        
