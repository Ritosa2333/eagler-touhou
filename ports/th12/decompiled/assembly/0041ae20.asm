
// block 0041ae20
0041ae20  sub        esp, 0x24
0041ae23  push       ebp
0041ae24  mov        ebp, dword ptr [esp + 0x2c]
0041ae28  cmp        dword ptr [ebp + 0x23c], 1
0041ae2f  push       esi
0041ae30  push       edi
0041ae31  jne        0x41af01

// block 0041ae37
0041ae37  mov        eax, dword ptr [0x4b4514]
0041ae3c  fld        dword ptr [ebp + 0x34]
0041ae3f  mov        ecx, dword ptr [eax + 0x97c]
0041ae45  mov        edx, dword ptr [eax + 0x980]
0041ae4b  mov        eax, dword ptr [eax + 0x984]
0041ae51  mov        dword ptr [esp + 0x18], ecx
0041ae55  fsub       dword ptr [esp + 0x18]
0041ae59  mov        dword ptr [esp + 0x1c], edx
0041ae5d  mov        dword ptr [esp + 0x20], eax
0041ae61  fstp       dword ptr [esp + 0x24]
0041ae65  mov        ecx, dword ptr [esp + 0x24]
0041ae69  fld        dword ptr [ebp + 0x38]
0041ae6c  mov        dword ptr [esp + 0xc], ecx
0041ae70  fsub       dword ptr [esp + 0x1c]
0041ae74  lea        ecx, [esp + 0xc]
0041ae78  push       ecx
0041ae79  fstp       dword ptr [esp + 0x2c]
0041ae7d  mov        edx, dword ptr [esp + 0x2c]
0041ae81  fld        dword ptr [ebp + 0x3c]
0041ae84  mov        dword ptr [esp + 0x14], edx
0041ae88  fsub       dword ptr [esp + 0x24]
0041ae8c  mov        edx, ecx
0041ae8e  push       edx
0041ae8f  fstp       dword ptr [esp + 0x34]
0041ae93  mov        eax, dword ptr [esp + 0x34]
0041ae97  fldz       
0041ae99  mov        dword ptr [esp + 0x1c], eax
0041ae9d  fstp       dword ptr [esp + 0x1c]
0041aea1  fld        dword ptr [ebp + 0x258]
0041aea7  fstp       dword ptr [esp + 0x3c]
0041aeab  call       0x46c6bc

// block 0041aeb0
0041aeb0  fld        dword ptr [esp + 0xc]
0041aeb4  fld        dword ptr [esp + 0x34]
0041aeb8  lea        edi, [esp + 0x18]
0041aebc  fld        st(0)
0041aebe  fmulp      st(2)
0041aec0  fxch       st(1)
0041aec2  fstp       dword ptr [esp + 0xc]
0041aec6  fld        dword ptr [esp + 0x10]
0041aeca  fmul       st(1)
0041aecc  fstp       dword ptr [esp + 0x10]
0041aed0  fmul       dword ptr [esp + 0x14]
0041aed4  fstp       dword ptr [esp + 0x14]
0041aed8  fld        dword ptr [esp + 0xc]
0041aedc  fadd       dword ptr [esp + 0x18]
0041aee0  fstp       dword ptr [esp + 0x18]
0041aee4  fld        dword ptr [esp + 0x10]
0041aee8  fadd       dword ptr [esp + 0x1c]
0041aeec  fstp       dword ptr [esp + 0x1c]
0041aef0  fld        dword ptr [esp + 0x20]
0041aef4  fadd       dword ptr [esp + 0x14]
0041aef8  fstp       dword ptr [esp + 0x20]
0041aefc  call       0x412570

// block 0041af01
0041af01  mov        esi, dword ptr [0x4b43c8]
0041af07  add        esi, 0x64
0041af0a  mov        edi, 0x7d0
0041af0f  nop        

// block 0041af10
0041af10  test       byte ptr [esi], 1
0041af13  je         0x41afca

// block 0041af19
0041af19  cmp        word ptr [esi + 0x532], 1
0041af21  jne        0x41afca

// block 0041af27
0041af27  movsx      eax, word ptr [esi + 0x9f4]
0041af2e  imul       eax, eax, 0xd0
0041af34  cmp        dword ptr [eax + 0x4af280], 0x26
0041af3b  jne        0x41afca

// block 0041af41
0041af41  fld        dword ptr [ebp + 0x258]
0041af47  sub        esp, 8
0041af4a  fchs       
0041af4c  lea        ecx, [esi + 0x4c8]
0041af52  fstp       dword ptr [esp + 0x3c]
0041af56  fld        dword ptr [esp + 0x3c]
0041af5a  fst        dword ptr [esi + 0x4d4]
0041af60  fstp       dword ptr [esp + 4]
0041af64  fld        dword ptr [esi + 0x4d8]
0041af6a  fstp       dword ptr [esp]
0041af6d  call       0x41c580

// block 0041af72
0041af72  fld        dword ptr [ebp + 0x34]
0041af75  fsub       dword ptr [esi + 0x4bc]
0041af7b  fstp       dword ptr [esp + 0x24]
0041af7f  fld        dword ptr [ebp + 0x38]
0041af82  fsub       dword ptr [esi + 0x4c0]
0041af88  fstp       dword ptr [esp + 0x28]
0041af8c  fld        dword ptr [esp + 0x28]
0041af90  fld        dword ptr [esp + 0x24]
0041af94  fld        dword ptr [ebp + 0x24c]
0041af9a  fld        st(1)
0041af9c  fmulp      st(2)
0041af9e  fld        st(2)
0041afa0  fmulp      st(3)
0041afa2  fxch       st(1)
0041afa4  faddp      st(2)
0041afa6  fxch       st(1)
0041afa8  fstp       dword ptr [esp + 0x34]
0041afac  fld        dword ptr [esp + 0x34]
0041afb0  fld        st(1)
0041afb2  fmulp      st(2)
0041afb4  fxch       st(1)
0041afb6  fmul       qword ptr [0x4a42b8]
0041afbc  fcompp     
0041afbe  fnstsw     ax
0041afc0  test       ah, 0x41
0041afc3  jne        0x41afca

// block 0041afc5
0041afc5  call       0x40c8b0

// block 0041afca
0041afca  add        esi, 0x9f8
0041afd0  sub        edi, 1
0041afd3  jne        0x41af10

// block 0041afd9
0041afd9  pop        edi
0041afda  pop        esi
0041afdb  xor        eax, eax
0041afdd  pop        ebp
0041afde  add        esp, 0x24
0041afe1  ret        4
