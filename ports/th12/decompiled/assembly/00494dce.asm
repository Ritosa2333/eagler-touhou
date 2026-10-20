
// block 00494dce
00494dce  push       eax
00494dcf  push       ebx
00494dd0  push       ecx
00494dd1  mov        eax, dword ptr [esp + 0x16]
00494dd5  xor        eax, 0x700
00494dda  test       eax, 0x700
00494ddf  jne        0x494f68

// block 00494de5
00494de5  shr        eax, 0xb
00494de8  and        eax, 0xf
00494deb  cmp        byte ptr [eax + 0x4b360c], 0
00494df2  je         0x494f68

// block 00494df8
00494df8  mov        eax, dword ptr [esp + 0x16]
00494dfc  and        eax, 0x7fff0000
00494e01  cmp        eax, 0x7fff0000
00494e06  je         0x494f68

// block 00494e0c
00494e0c  mov        eax, dword ptr [esp + 0x2e]
00494e10  and        eax, 0x7fff0000
00494e15  je         0x494f68

// block 00494e1b
00494e1b  cmp        eax, 0x7fff0000
00494e20  je         0x494f68

// block 00494e26
00494e26  mov        eax, dword ptr [esp + 0x2c]
00494e2a  add        eax, eax
00494e2c  jne        0x494f68

// block 00494e32
00494e32  mov        eax, dword ptr [esp + 0x14]
00494e36  add        eax, eax
00494e38  jne        0x494f68

// block 00494e3e
00494e3e  mov        eax, dword ptr [esp + 0x18]
00494e42  and        eax, 0x7fff
00494e47  add        eax, 0x3f
00494e4a  mov        ebx, dword ptr [esp + 0x30]
00494e4e  and        ebx, 0x7fff
00494e54  sub        ebx, eax
00494e56  ja         0x494eb6

// block 00494e58
00494e58  mov        eax, dword ptr [esp + 0x18]
00494e5c  and        eax, 0x7fff
00494e61  add        eax, 0xa
00494e64  mov        ebx, dword ptr [esp + 0x30]
00494e68  and        ebx, 0x7fff
00494e6e  sub        ebx, eax
00494e70  js         0x494f68

// block 00494e76
00494e76  fld        xword ptr [esp + 0x28]
00494e7a  mov        eax, dword ptr [esp + 0x18]
00494e7e  mov        ebx, dword ptr [esp + 0x30]
00494e82  and        ebx, 0x7fff
00494e88  mov        ecx, ebx
00494e8a  sub        ebx, eax
00494e8c  and        ebx, 7
00494e8f  or         ebx, 4
00494e92  sub        ecx, ebx
00494e94  mov        ebx, eax
00494e96  and        ebx, 0x8000
00494e9c  or         ecx, ebx
00494e9e  mov        dword ptr [esp + 0x18], ecx
00494ea2  fld        xword ptr [esp + 0x10]
00494ea6  mov        dword ptr [esp + 0x18], eax
00494eaa  fxch       st(1)
00494eac  fprem      
00494eae  fstp       xword ptr [esp + 0x28]
00494eb2  fstp       st(0)
00494eb4  jmp        0x494e58

// block 00494eb6
00494eb6  test       ebx, 2
00494ebc  jne        0x494ec6

// block 00494ebe
00494ebe  fld        xword ptr [esp + 0x10]
00494ec2  fstp       xword ptr [esp + 0x1c]

// block 00494ec6
00494ec6  fnstcw     word ptr [esp + 0x34]
00494eca  mov        eax, dword ptr [esp + 0x34]
00494ece  or         eax, 0x33f
00494ed3  mov        dword ptr [esp + 0x38], eax
00494ed7  fldcw      word ptr [esp + 0x38]
00494edb  mov        eax, dword ptr [esp + 0x18]
00494edf  and        eax, 0x7fff
00494ee4  mov        ebx, dword ptr [esp + 0x30]
00494ee8  and        ebx, 0x7fff
00494eee  sub        ebx, eax
00494ef0  and        ebx, 0x3f
00494ef3  or         ebx, 0x20
00494ef6  add        ebx, 1
00494ef9  mov        ecx, ebx
00494efb  mov        eax, dword ptr [esp + 0x18]
00494eff  mov        ebx, dword ptr [esp + 0x30]
00494f03  and        ebx, 0x7fff
00494f09  and        eax, 0x8000
00494f0e  or         ebx, eax
00494f10  mov        dword ptr [esp + 0x18], ebx
00494f14  fld        xword ptr [esp + 0x10]
00494f18  fabs       
00494f1a  fld        xword ptr [esp + 0x28]
00494f1e  fabs       

// block 00494f20
00494f20  fcom       st(1)
00494f22  fnstsw     ax
00494f24  and        eax, 0x100
00494f29  jne        0x494f2d

// block 00494f2b
00494f2b  fsub       st(1)

// block 00494f2d
00494f2d  fxch       st(1)
00494f2f  fmul       qword ptr [0x4b363c]
00494f35  fxch       st(1)
00494f37  sub        ecx, 1
00494f3a  jne        0x494f20

// block 00494f3c
00494f3c  mov        ebx, dword ptr [esp + 0x30]
00494f40  fstp       xword ptr [esp + 0x28]
00494f44  fstp       st(0)
00494f46  fld        xword ptr [esp + 0x1c]
00494f4a  fld        xword ptr [0x4b3644]
00494f50  fprem1     
00494f52  fstp       st(0)
00494f54  fld        xword ptr [esp + 0x28]
00494f58  fldcw      word ptr [esp + 0x34]
00494f5c  and        ebx, 0x8000
00494f62  je         0x494f72

// block 00494f64
00494f64  fchs       
00494f66  jmp        0x494f72

// block 00494f68
00494f68  fld        xword ptr [esp + 0x10]
00494f6c  fld        xword ptr [esp + 0x28]
00494f70  fprem1     

// block 00494f72
00494f72  test       edx, 3
00494f78  je         0x494fd0

// block 00494f7a
00494f7a  fnstsw     word ptr [esp + 0x3c]
00494f7e  test       edx, 1
00494f84  je         0x494fa5

// block 00494f86
00494f86  fnstcw     word ptr [esp + 0x34]
00494f8a  mov        eax, dword ptr [esp + 0x34]
00494f8e  or         eax, 0x300
00494f93  mov        dword ptr [esp + 0x38], eax
00494f97  fldcw      word ptr [esp + 0x38]
00494f9b  fmul       qword ptr [0x4b362c]
00494fa1  fldcw      word ptr [esp + 0x34]

// block 00494fa5
00494fa5  mov        eax, dword ptr [esp + 0x3c]
00494fa9  fxch       st(1)
00494fab  fstp       st(0)
00494fad  fld        xword ptr [esp + 0x1c]
00494fb1  fxch       st(1)
00494fb3  and        eax, 0x4300
00494fb8  sub        esp, 0x1c
00494fbb  fnstenv    [esp]
00494fbe  and        dword ptr [esp + 4], 0xbcff
00494fc6  or         dword ptr [esp + 4], eax
00494fca  fldenv     [esp]
00494fcd  add        esp, 0x1c

// block 00494fd0
00494fd0  pop        ecx
00494fd1  pop        ebx
00494fd2  pop        eax
00494fd3  ret        
