
// block 00437d00
00437d00  push       ebp
00437d01  mov        ebp, esp
00437d03  and        esp, 0xfffffff8
00437d06  sub        esp, 0x34
00437d09  push       esi
00437d0a  mov        esi, dword ptr [0x4b4514]
00437d10  fld        dword ptr [esi + 0x97c]
00437d16  fsub       dword ptr [eax]
00437d18  fstp       dword ptr [esp + 0x14]
00437d1c  fld        dword ptr [esi + 0x980]
00437d22  fsub       dword ptr [eax + 4]
00437d25  fstp       dword ptr [esp + 0x18]
00437d29  fld        dword ptr [ebp + 8]
00437d2c  fchs       
00437d2e  fstp       dword ptr [esp + 8]
00437d32  fld        dword ptr [esp + 8]
00437d36  call       0x4938c0

// block 00437d3b
00437d3b  fstp       dword ptr [esp + 0xc]
00437d3f  fld        dword ptr [esp + 0xc]
00437d43  fstp       dword ptr [esp + 0x10]
00437d47  fld        dword ptr [esp + 8]
00437d4b  call       0x4939f0

// block 00437d50
00437d50  fstp       dword ptr [esp + 0xc]
00437d54  fld        dword ptr [esp + 0xc]
00437d58  fstp       dword ptr [esp + 0xc]
00437d5c  fld        dword ptr [esp + 0xc]
00437d60  fld        st(0)
00437d62  fld        dword ptr [esp + 0x14]
00437d66  fld        st(0)
00437d68  fmulp      st(2)
00437d6a  fld        dword ptr [esp + 0x18]
00437d6e  fld        st(0)
00437d70  fld        dword ptr [esp + 0x10]
00437d74  fld        st(0)
00437d76  fmulp      st(2)
00437d78  fxch       st(4)
00437d7a  fsubrp     st(1)
00437d7c  fstp       dword ptr [esp + 0x14]
00437d80  fmulp      st(3)
00437d82  fmulp      st(1)
00437d84  faddp      st(1)
00437d86  fstp       dword ptr [esp + 0x18]
00437d8a  fld        dword ptr [esi + 0x9e4]
00437d90  fld        qword ptr [0x4a3d68]
00437d96  fmul       st(1), st(0)
00437d98  fxch       st(1)
00437d9a  fstp       dword ptr [esp + 0x20]
00437d9e  fld        dword ptr [esi + 0x9e8]
00437da4  fmul       st(1)
00437da6  fstp       dword ptr [esp + 0x24]
00437daa  fld        dword ptr [esp + 0x14]
00437dae  fld        st(0)
00437db0  fsub       dword ptr [esp + 0x20]
00437db4  fstp       dword ptr [esp + 0x2c]
00437db8  fld        dword ptr [esp + 0x18]
00437dbc  fld        st(0)
00437dbe  fsub       dword ptr [esp + 0x24]
00437dc2  fstp       dword ptr [esp + 0x30]
00437dc6  fld        dword ptr [esi + 0x9e4]
00437dcc  fmul       st(3)
00437dce  fstp       dword ptr [esp + 0x20]
00437dd2  fld        dword ptr [esi + 0x9e8]
00437dd8  fmulp      st(3)
00437dda  fxch       st(2)
00437ddc  fstp       dword ptr [esp + 0x24]
00437de0  fld        dword ptr [esp + 0x20]
00437de4  fadd       st(1)
00437de6  fstp       dword ptr [esp + 0x14]
00437dea  fld        dword ptr [esp + 0x24]
00437dee  fadd       st(2)
00437df0  fstp       dword ptr [esp + 0x18]
00437df4  fld        dword ptr [esp + 0x2c]
00437df8  fld        dword ptr [ebp + 0x10]
00437dfb  fcom       st(1)
00437dfd  fnstsw     ax
00437dff  fstp       st(1)
00437e01  test       ah, 5
00437e04  jnp        0x437f1c

// block 00437e0a
00437e0a  fld        dword ptr [ebp + 0xc]
00437e0d  fld        st(0)
00437e0f  fld        qword ptr [0x4a3cf8]
00437e15  fmul       st(1), st(0)
00437e17  fld        dword ptr [esp + 0x30]
00437e1b  fcomp      st(2)
00437e1d  fnstsw     ax
00437e1f  test       ah, 0x41
00437e22  je         0x437eea

// block 00437e28
00437e28  fldz       
00437e2a  fcom       dword ptr [esp + 0x14]
00437e2e  fnstsw     ax
00437e30  test       ah, 0x41
00437e33  je         0x437eff

// block 00437e39
00437e39  fxch       st(3)
00437e3b  fchs       
00437e3d  fmulp      st(1)
00437e3f  fld        dword ptr [esp + 0x18]
00437e43  fcomp      st(1)
00437e45  fnstsw     ax
00437e47  test       ah, 5
00437e4a  jnp        0x437f16

// block 00437e50
00437e50  fld        st(4)
00437e52  fsub       dword ptr [esi + 0x9e4]
00437e58  fstp       dword ptr [esp + 0x2c]
00437e5c  fld        st(5)
00437e5e  fsub       dword ptr [esi + 0x9e8]
00437e64  fstp       dword ptr [esp + 0x30]
00437e68  fld        dword ptr [esi + 0x9e4]
00437e6e  faddp      st(5)
00437e70  fxch       st(4)
00437e72  fstp       dword ptr [esp + 0x20]
00437e76  fld        dword ptr [esi + 0x9e8]
00437e7c  faddp      st(5)
00437e7e  fxch       st(4)
00437e80  fstp       dword ptr [esp + 0x24]
00437e84  fld        dword ptr [esp + 0x2c]
00437e88  fcomp      st(2)
00437e8a  fnstsw     ax
00437e8c  fstp       st(1)
00437e8e  test       ah, 0x41
00437e91  je         0x437ec8

// block 00437e93
00437e93  fld        dword ptr [esp + 0x30]
00437e97  fcomp      st(3)
00437e99  fnstsw     ax
00437e9b  fstp       st(2)
00437e9d  test       ah, 0x41
00437ea0  je         0x437eda

// block 00437ea2
00437ea2  fxch       st(1)
00437ea4  fcomp      dword ptr [esp + 0x20]
00437ea8  fnstsw     ax
00437eaa  test       ah, 0x41
00437ead  je         0x437edc

// block 00437eaf
00437eaf  fld        dword ptr [esp + 0x24]
00437eb3  fcompp     
00437eb5  fnstsw     ax
00437eb7  test       ah, 5
00437eba  jnp        0x437ede

// block 00437ebc
00437ebc  mov        eax, 1
00437ec1  pop        esi
00437ec2  mov        esp, ebp
00437ec4  pop        ebp
00437ec5  ret        0xc

// block 00437ec8
00437ec8  fstp       st(1)
00437eca  mov        eax, 2
00437ecf  fstp       st(1)
00437ed1  fstp       st(0)
00437ed3  pop        esi
00437ed4  mov        esp, ebp
00437ed6  pop        ebp
00437ed7  ret        0xc

// block 00437eda
00437eda  fstp       st(0)

// block 00437edc
00437edc  fstp       st(0)

// block 00437ede
00437ede  mov        eax, 2
00437ee3  pop        esi
00437ee4  mov        esp, ebp
00437ee6  pop        ebp
00437ee7  ret        0xc

// block 00437eea
00437eea  fstp       st(3)
00437eec  xor        eax, eax
00437eee  fstp       st(3)
00437ef0  fstp       st(3)
00437ef2  fstp       st(2)
00437ef4  fstp       st(1)
00437ef6  fstp       st(0)
00437ef8  pop        esi
00437ef9  mov        esp, ebp
00437efb  pop        ebp
00437efc  ret        0xc

// block 00437eff
00437eff  fstp       st(4)
00437f01  xor        eax, eax
00437f03  fstp       st(4)
00437f05  fstp       st(4)
00437f07  fstp       st(0)
00437f09  fstp       st(1)
00437f0b  fstp       st(1)
00437f0d  fstp       st(0)
00437f0f  pop        esi
00437f10  mov        esp, ebp
00437f12  pop        ebp
00437f13  ret        0xc

// block 00437f16
00437f16  fstp       st(3)
00437f18  fstp       st(3)
00437f1a  fstp       st(3)

// block 00437f1c
00437f1c  fstp       st(0)
00437f1e  xor        eax, eax
00437f20  fstp       st(0)
00437f22  pop        esi
00437f23  fstp       st(0)
00437f25  mov        esp, ebp
00437f27  pop        ebp
00437f28  ret        0xc
