
// block 00430a70
00430a70  sub        esp, 0x28
00430a73  push       ebx
00430a74  push       ebp
00430a75  push       esi
00430a76  mov        esi, dword ptr [0x4ce8cc]
00430a7c  test       esi, esi
00430a7e  je         0x430a85

// block 00430a80
00430a80  call       0x45a3c0

// block 00430a85
00430a85  fld        dword ptr [edi + 0xc]
00430a88  lea        esi, [edi + 0x18]
00430a8b  fadd       dword ptr [edi]
00430a8d  push       esi
00430a8e  lea        eax, [esp + 0x20]
00430a92  push       eax
00430a93  fstp       dword ptr [esp + 0x24]
00430a97  lea        ecx, [esp + 0x30]
00430a9b  fld        dword ptr [edi + 0x10]
00430a9e  push       ecx
00430a9f  fadd       dword ptr [edi + 4]
00430aa2  lea        ebx, [edi + 0x4c]
00430aa5  push       ebx
00430aa6  fstp       dword ptr [esp + 0x30]
00430aaa  fld        dword ptr [edi + 0x14]
00430aad  fadd       dword ptr [edi + 8]
00430ab0  fstp       dword ptr [esp + 0x34]
00430ab4  fld        dword ptr [edi + 0x3c]
00430ab7  fadd       dword ptr [edi]
00430ab9  fstp       dword ptr [esp + 0x38]
00430abd  fld        dword ptr [edi + 0x40]
00430ac0  fadd       dword ptr [edi + 4]
00430ac3  fstp       dword ptr [esp + 0x3c]
00430ac7  fld        dword ptr [edi + 0x44]
00430aca  fadd       dword ptr [edi + 8]
00430acd  fstp       dword ptr [esp + 0x40]
00430ad1  call       0x46c6da

// block 00430ad6
00430ad6  fld        dword ptr [0x4a3e18]
00430adc  mov        edx, dword ptr [edi + 0xd4]
00430ae2  sub        esp, 0x10
00430ae5  fstp       dword ptr [esp + 0xc]
00430ae9  lea        ebp, [edi + 0x8c]
00430aef  fld        dword ptr [0x4a3e14]
00430af5  fstp       dword ptr [esp + 8]
00430af9  fild       dword ptr [edi + 0xd4]
00430aff  test       edx, edx
00430b01  jge        0x430b09

// block 00430b03
00430b03  fadd       dword ptr [0x4a3e10]

// block 00430b09
00430b09  mov        eax, dword ptr [edi + 0xd8]
00430b0f  fild       dword ptr [edi + 0xd8]
00430b15  test       eax, eax
00430b17  jge        0x430b1f

// block 00430b19
00430b19  fadd       dword ptr [0x4a3e10]

// block 00430b1f
00430b1f  fdivp      st(1)
00430b21  fstp       dword ptr [esp + 0x1c]
00430b25  fld        dword ptr [esp + 0x1c]
00430b29  fstp       dword ptr [esp + 4]
00430b2d  fld        dword ptr [edi + 0x48]
00430b30  fstp       dword ptr [esp]
00430b33  push       ebp
00430b34  call       0x46c6e0

// block 00430b39
00430b39  mov        eax, dword ptr [0x4ce8f0]
00430b3e  mov        ecx, dword ptr [eax]
00430b40  mov        edx, dword ptr [ecx + 0xb0]
00430b46  push       ebx
00430b47  push       2
00430b49  push       eax
00430b4a  call       edx

// block 00430b4c
00430b4c  mov        eax, dword ptr [0x4ce8f0]
00430b51  mov        ecx, dword ptr [eax]
00430b53  mov        edx, dword ptr [ecx + 0xb0]
00430b59  push       ebp
00430b5a  push       3
00430b5c  push       eax
00430b5d  call       edx

// block 00430b5f
00430b5f  fld        dword ptr [edi + 0x10]
00430b62  fmul       dword ptr [esi + 8]
00430b65  lea        eax, [edi + 0x30]
00430b68  fld        dword ptr [edi + 0x14]
00430b6b  push       eax
00430b6c  fmul       dword ptr [esi + 4]
00430b6f  push       eax
00430b70  fsubp      st(1)
00430b72  fstp       dword ptr [esp + 0x18]
00430b76  mov        ecx, dword ptr [esp + 0x18]
00430b7a  fld        dword ptr [esi]
00430b7c  fmul       dword ptr [edi + 0x14]
00430b7f  fld        dword ptr [edi + 0xc]
00430b82  fmul       dword ptr [esi + 8]
00430b85  fsubp      st(1)
00430b87  fstp       dword ptr [esp + 0x1c]
00430b8b  mov        edx, dword ptr [esp + 0x1c]
00430b8f  fld        dword ptr [esi + 4]
00430b92  fmul       dword ptr [edi + 0xc]
00430b95  fld        dword ptr [esi]
00430b97  fmul       dword ptr [edi + 0x10]
00430b9a  mov        dword ptr [eax], ecx
00430b9c  mov        dword ptr [eax + 4], edx
00430b9f  fsubp      st(1)
00430ba1  fstp       dword ptr [esp + 0x20]
00430ba5  mov        ecx, dword ptr [esp + 0x20]
00430ba9  mov        dword ptr [eax + 8], ecx
00430bac  call       0x46c6bc

// block 00430bb1
00430bb1  mov        eax, dword ptr [0x4ce8cc]
00430bb6  pop        esi
00430bb7  pop        ebp
00430bb8  pop        ebx
00430bb9  test       eax, eax
00430bbb  je         0x430bd5

// block 00430bbd
00430bbd  fld        dword ptr [edi + 0xe8]
00430bc3  fstp       dword ptr [eax + 0xb0]
00430bc9  fld        dword ptr [edi + 0xec]
00430bcf  fstp       dword ptr [eax + 0xb4]

// block 00430bd5
00430bd5  add        esp, 0x28
00430bd8  ret        
