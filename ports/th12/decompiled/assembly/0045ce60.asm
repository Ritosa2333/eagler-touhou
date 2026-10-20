
// block 0045ce60
0045ce60  fldz       
0045ce62  mov        ecx, dword ptr [edi + 0x8856b0]
0045ce68  fld        st(0)
0045ce6a  sub        esp, 8
0045ce6d  fld        dword ptr [esp + 0x1c]
0045ce71  fucom      st(1)
0045ce73  fnstsw     ax
0045ce75  fstp       st(1)
0045ce77  fld1       
0045ce79  test       ah, 0x44
0045ce7c  jnp        0x45ce99

// block 0045ce7e
0045ce7e  fstp       st(2)
0045ce80  fstp       st(1)
0045ce82  fstp       dword ptr [esp + 4]
0045ce86  fld        dword ptr [esp + 4]
0045ce8a  fsincos    
0045ce8c  fstp       dword ptr [esp]
0045ce8f  fstp       dword ptr [esp + 0x1c]
0045ce93  fld1       
0045ce95  fldz       
0045ce97  jmp        0x45cea8

// block 0045ce99
0045ce99  fstp       st(1)
0045ce9b  fxch       st(1)
0045ce9d  fst        dword ptr [esp + 0x1c]
0045cea1  fxch       st(1)
0045cea3  fst        dword ptr [esp]
0045cea6  fxch       st(1)

// block 0045cea8
0045cea8  fld        dword ptr [esp + 0x14]
0045ceac  push       esi
0045cead  fld        qword ptr [0x4a3cf8]
0045ceb3  fmul       st(1), st(0)
0045ceb5  fxch       st(1)
0045ceb7  fstp       dword ptr [esp + 0x18]
0045cebb  fmul       dword ptr [esp + 0x1c]
0045cebf  fstp       dword ptr [esp + 0x1c]
0045cec3  fld        dword ptr [esp + 4]
0045cec7  fld        dword ptr [esp + 0x18]
0045cecb  fld        st(0)
0045cecd  fmulp      st(2)
0045cecf  fld        dword ptr [esp + 0x20]
0045ced3  fld        dword ptr [esp + 0x1c]
0045ced7  fld        st(0)
0045ced9  fmulp      st(2)
0045cedb  fxch       st(3)
0045cedd  fsubrp     st(1)
0045cedf  fld        dword ptr [esp + 0x10]
0045cee3  fld        st(0)
0045cee5  faddp      st(2)
0045cee7  fxch       st(1)
0045cee9  fstp       dword ptr [ecx]
0045ceeb  fld        dword ptr [esp + 0x20]
0045ceef  fmul       st(2)
0045cef1  fld        dword ptr [esp + 4]
0045cef5  fmul       st(4)
0045cef7  faddp      st(1)
0045cef9  fadd       dword ptr [esp + 0x14]
0045cefd  fstp       dword ptr [ecx + 4]
0045cf00  fxch       st(3)
0045cf02  fst        dword ptr [ecx + 8]
0045cf05  fld        dword ptr [esp + 4]
0045cf09  fmul       st(2)
0045cf0b  fld        dword ptr [esp + 0x20]
0045cf0f  fmul       st(4)
0045cf11  faddp      st(1)
0045cf13  fsubp      st(4)
0045cf15  fxch       st(3)
0045cf17  fstp       dword ptr [ecx + 0x14]
0045cf1a  fld        dword ptr [esp + 4]
0045cf1e  fmul       st(2)
0045cf20  fld        dword ptr [esp + 0x20]
0045cf24  fmul       st(2)
0045cf26  fsubp      st(1)
0045cf28  fld        dword ptr [esp + 0x14]
0045cf2c  fld        st(0)
0045cf2e  faddp      st(2)
0045cf30  fxch       st(1)
0045cf32  fstp       dword ptr [ecx + 0x18]
0045cf35  fxch       st(3)
0045cf37  fst        dword ptr [ecx + 0x1c]
0045cf3a  fld        dword ptr [esp + 4]
0045cf3e  fmul       st(2)
0045cf40  fld        dword ptr [esp + 0x20]
0045cf44  fmul       st(4)
0045cf46  faddp      st(1)
0045cf48  fadd       dword ptr [esp + 0x10]
0045cf4c  fstp       dword ptr [ecx + 0x28]
0045cf4f  fld        dword ptr [esp + 0x20]
0045cf53  fmul       st(2)
0045cf55  fld        dword ptr [esp + 4]
0045cf59  fmul       st(4)
0045cf5b  fsubp      st(1)
0045cf5d  fadd       st(4)
0045cf5f  fstp       dword ptr [ecx + 0x2c]
0045cf62  fst        dword ptr [ecx + 0x30]
0045cf65  fld        dword ptr [esp + 0x20]
0045cf69  fmul       st(3)
0045cf6b  fld        dword ptr [esp + 4]
0045cf6f  fmul       st(3)
0045cf71  fsubp      st(1)
0045cf73  fadd       dword ptr [esp + 0x10]
0045cf77  fstp       dword ptr [ecx + 0x3c]
0045cf7a  fld        dword ptr [esp + 0x20]
0045cf7e  fmulp      st(2)
0045cf80  fld        dword ptr [esp + 4]
0045cf84  fmulp      st(3)
0045cf86  fxch       st(1)
0045cf88  faddp      st(2)
0045cf8a  fxch       st(2)
0045cf8c  mov        esi, dword ptr [0x4ce8cc]
0045cf92  fsubrp     st(1)
0045cf94  mov        dword ptr [ecx + 0x4c], edx
0045cf97  mov        dword ptr [ecx + 0x38], edx
0045cf9a  fstp       dword ptr [ecx + 0x40]
0045cf9d  mov        dword ptr [ecx + 0x24], edx
0045cfa0  mov        dword ptr [ecx + 0x10], edx
0045cfa3  fstp       dword ptr [ecx + 0x44]
0045cfa6  fst        dword ptr [ecx + 0x48]
0045cfa9  fst        dword ptr [ecx + 0x34]
0045cfac  fst        dword ptr [ecx + 0x20]
0045cfaf  fstp       dword ptr [ecx + 0xc]

// block 0045cfb2
0045cfb2  call       0x45a3c0

// block 0045cfb7
0045cfb7  mov        eax, dword ptr [0x4ce8f0]
0045cfbc  mov        ecx, dword ptr [eax]
0045cfbe  mov        edx, dword ptr [ecx + 0xe4]
0045cfc4  push       0
0045cfc6  push       0xe
0045cfc8  push       eax
0045cfc9  call       edx

// block 0045cfcb
0045cfcb  mov        eax, dword ptr [0x4ce8cc]
0045cfd0  cmp        byte ptr [eax + 0x4b5647], 0
0045cfd7  pop        esi
0045cfd8  je         0x45d012

// block 0045cfda
0045cfda  mov        eax, dword ptr [0x4ce8f0]
0045cfdf  mov        ecx, dword ptr [eax]
0045cfe1  mov        edx, dword ptr [ecx + 0x10c]
0045cfe7  push       3
0045cfe9  push       4
0045cfeb  push       0
0045cfed  push       eax
0045cfee  call       edx

// block 0045cff0
0045cff0  mov        eax, dword ptr [0x4ce8f0]
0045cff5  mov        ecx, dword ptr [eax]
0045cff7  mov        edx, dword ptr [ecx + 0x10c]
0045cffd  push       3
0045cfff  push       1
0045d001  push       0
0045d003  push       eax
0045d004  call       edx

// block 0045d006
0045d006  mov        eax, dword ptr [0x4ce8cc]
0045d00b  mov        byte ptr [eax + 0x4b5647], 0

// block 0045d012
0045d012  cmp        byte ptr [edi + 0x4b5642], 1
0045d019  je         0x45d04e

// block 0045d01b
0045d01b  mov        eax, dword ptr [0x4ce8f0]
0045d020  mov        ecx, dword ptr [eax]
0045d022  mov        edx, dword ptr [ecx + 0x10c]
0045d028  push       0
0045d02a  push       6
0045d02c  push       0
0045d02e  push       eax
0045d02f  call       edx

// block 0045d031
0045d031  mov        eax, dword ptr [0x4ce8f0]
0045d036  mov        ecx, dword ptr [eax]
0045d038  mov        edx, dword ptr [ecx + 0x10c]
0045d03e  push       0
0045d040  push       3
0045d042  push       0
0045d044  push       eax
0045d045  call       edx

// block 0045d047
0045d047  mov        byte ptr [edi + 0x4b5642], 1

// block 0045d04e
0045d04e  mov        eax, dword ptr [0x4ce8f0]
0045d053  mov        ecx, dword ptr [eax]
0045d055  mov        edx, dword ptr [ecx + 0x164]
0045d05b  push       0x44
0045d05d  push       eax
0045d05e  call       edx

// block 0045d060
0045d060  mov        edx, dword ptr [edi + 0x8856b0]
0045d066  mov        eax, dword ptr [0x4ce8f0]
0045d06b  mov        ecx, dword ptr [eax]
0045d06d  push       0x14
0045d06f  push       edx
0045d070  push       2
0045d072  push       5
0045d074  push       eax
0045d075  mov        eax, dword ptr [ecx + 0x14c]
0045d07b  call       eax

// block 0045d07d
0045d07d  add        dword ptr [edi + 0x8856b0], 0x50
0045d084  inc        dword ptr [edi + 0xac]
0045d08a  xor        eax, eax
0045d08c  add        esp, 8
0045d08f  ret        0x14
