
// block 00437a80
00437a80  push       ebp
00437a81  mov        ebp, esp
00437a83  and        esp, 0xfffffff8
00437a86  sub        esp, 0x34
00437a89  push       esi
00437a8a  mov        esi, dword ptr [0x4b4514]
00437a90  fld        dword ptr [esi + 0x97c]
00437a96  fsub       dword ptr [eax]
00437a98  fstp       dword ptr [esp + 0x14]
00437a9c  fld        dword ptr [esi + 0x980]
00437aa2  fsub       dword ptr [eax + 4]
00437aa5  fstp       dword ptr [esp + 0x18]
00437aa9  fld        dword ptr [ebp + 8]
00437aac  fchs       
00437aae  fstp       dword ptr [esp + 8]
00437ab2  fld        dword ptr [esp + 8]
00437ab6  call       0x4938c0

// block 00437abb
00437abb  fstp       dword ptr [esp + 0xc]
00437abf  fld        dword ptr [esp + 0xc]
00437ac3  fstp       dword ptr [esp + 0x10]
00437ac7  fld        dword ptr [esp + 8]
00437acb  call       0x4939f0

// block 00437ad0
00437ad0  fstp       dword ptr [esp + 0xc]
00437ad4  fld        dword ptr [esp + 0xc]
00437ad8  fstp       dword ptr [esp + 0xc]
00437adc  fld        dword ptr [esp + 0xc]
00437ae0  fld        st(0)
00437ae2  fld        dword ptr [esp + 0x14]
00437ae6  fld        st(0)
00437ae8  fmulp      st(2)
00437aea  fld        dword ptr [esp + 0x18]
00437aee  fld        st(0)
00437af0  fld        dword ptr [esp + 0x10]
00437af4  fld        st(0)
00437af6  fmulp      st(2)
00437af8  fxch       st(4)
00437afa  fsubrp     st(1)
00437afc  fstp       dword ptr [esp + 0x14]
00437b00  fmulp      st(3)
00437b02  fmulp      st(1)
00437b04  faddp      st(1)
00437b06  fstp       dword ptr [esp + 0x18]
00437b0a  fld        dword ptr [esi + 0x9e4]
00437b10  fld        qword ptr [0x4a3d68]
00437b16  fmul       st(1), st(0)
00437b18  fxch       st(1)
00437b1a  fstp       dword ptr [esp + 0x20]
00437b1e  fld        dword ptr [esi + 0x9e8]
00437b24  fmul       st(1)
00437b26  fstp       dword ptr [esp + 0x24]
00437b2a  fld        dword ptr [esp + 0x14]
00437b2e  fld        st(0)
00437b30  fsub       dword ptr [esp + 0x20]
00437b34  fstp       dword ptr [esp + 0x2c]
00437b38  fld        dword ptr [esp + 0x18]
00437b3c  fld        st(0)
00437b3e  fsub       dword ptr [esp + 0x24]
00437b42  fstp       dword ptr [esp + 0x30]
00437b46  fld        dword ptr [esi + 0x9e4]
00437b4c  fmul       st(3)
00437b4e  fstp       dword ptr [esp + 0x20]
00437b52  fld        dword ptr [esi + 0x9e8]
00437b58  fmulp      st(3)
00437b5a  fxch       st(2)
00437b5c  fstp       dword ptr [esp + 0x24]
00437b60  fld        dword ptr [esp + 0x20]
00437b64  fadd       st(1)
00437b66  fstp       dword ptr [esp + 0x14]
00437b6a  fld        dword ptr [esp + 0x24]
00437b6e  fadd       st(2)
00437b70  fstp       dword ptr [esp + 0x18]
00437b74  fld        dword ptr [esp + 0x2c]
00437b78  fld        dword ptr [ebp + 0x10]
00437b7b  fcom       st(1)
00437b7d  fnstsw     ax
00437b7f  fstp       st(1)
00437b81  test       ah, 5
00437b84  jnp        0x437cee

// block 00437b8a
00437b8a  fld        dword ptr [ebp + 0xc]
00437b8d  fld        st(0)
00437b8f  fld        qword ptr [0x4a3cf8]
00437b95  fmul       st(1), st(0)
00437b97  fld        dword ptr [esp + 0x30]
00437b9b  fcomp      st(2)
00437b9d  fnstsw     ax
00437b9f  test       ah, 0x41
00437ba2  je         0x437cbc

// block 00437ba8
00437ba8  fldz       
00437baa  fcom       dword ptr [esp + 0x14]
00437bae  fnstsw     ax
00437bb0  test       ah, 0x41
00437bb3  je         0x437cd1

// block 00437bb9
00437bb9  fxch       st(3)
00437bbb  fchs       
00437bbd  fmulp      st(1)
00437bbf  fld        dword ptr [esp + 0x18]
00437bc3  fcomp      st(1)
00437bc5  fnstsw     ax
00437bc7  test       ah, 5
00437bca  jnp        0x437ce8

// block 00437bd0
00437bd0  fld        st(4)
00437bd2  fsub       dword ptr [esi + 0x9e4]
00437bd8  fstp       dword ptr [esp + 0x2c]
00437bdc  fld        st(5)
00437bde  fsub       dword ptr [esi + 0x9e8]
00437be4  fstp       dword ptr [esp + 0x30]
00437be8  fld        dword ptr [esi + 0x9e4]
00437bee  faddp      st(5)
00437bf0  fxch       st(4)
00437bf2  fstp       dword ptr [esp + 0x20]
00437bf6  fld        dword ptr [esi + 0x9e8]
00437bfc  faddp      st(5)
00437bfe  fxch       st(4)
00437c00  fstp       dword ptr [esp + 0x24]
00437c04  fld        dword ptr [esp + 0x2c]
00437c08  fcomp      st(2)
00437c0a  fnstsw     ax
00437c0c  fstp       st(1)
00437c0e  test       ah, 0x41
00437c11  je         0x437c9a

// block 00437c17
00437c17  fld        dword ptr [esp + 0x30]
00437c1b  fcomp      st(3)
00437c1d  fnstsw     ax
00437c1f  fstp       st(2)
00437c21  test       ah, 0x41
00437c24  je         0x437cac

// block 00437c2a
00437c2a  fxch       st(1)
00437c2c  fcomp      dword ptr [esp + 0x20]
00437c30  fnstsw     ax
00437c32  test       ah, 0x41
00437c35  je         0x437cae

// block 00437c37
00437c37  fld        dword ptr [esp + 0x24]
00437c3b  fcompp     
00437c3d  fnstsw     ax
00437c3f  test       ah, 5
00437c42  jnp        0x437cb0

// block 00437c44
00437c44  mov        eax, dword ptr [0x4b43e4]
00437c49  test       eax, eax
00437c4b  je         0x437c5a

// block 00437c4d
00437c4d  cmp        dword ptr [eax + 0x6d30], 0
00437c54  jne        0x437cf4

// block 00437c5a
00437c5a  mov        eax, dword ptr [esi + 0xa28]
00437c60  cmp        eax, 2
00437c63  je         0x437cf4

// block 00437c69
00437c69  cmp        eax, 4
00437c6c  je         0x437cf4

// block 00437c72
00437c72  cmp        eax, 3
00437c75  je         0x437cf4

// block 00437c77
00437c77  test       byte ptr [esi + 0xc414], 2
00437c7e  jne        0x437cf4

// block 00437c80
00437c80  cmp        dword ptr [esi + 0xc404], 0
00437c87  jg         0x437cf4

// block 00437c89
00437c89  call       0x438370

// block 00437c8e
00437c8e  mov        eax, 1
00437c93  pop        esi
00437c94  mov        esp, ebp
00437c96  pop        ebp
00437c97  ret        0xc

// block 00437c9a
00437c9a  fstp       st(1)
00437c9c  mov        eax, 2
00437ca1  fstp       st(1)
00437ca3  fstp       st(0)
00437ca5  pop        esi
00437ca6  mov        esp, ebp
00437ca8  pop        ebp
00437ca9  ret        0xc

// block 00437cac
00437cac  fstp       st(0)

// block 00437cae
00437cae  fstp       st(0)

// block 00437cb0
00437cb0  mov        eax, 2
00437cb5  pop        esi
00437cb6  mov        esp, ebp
00437cb8  pop        ebp
00437cb9  ret        0xc

// block 00437cbc
00437cbc  fstp       st(3)
00437cbe  xor        eax, eax
00437cc0  fstp       st(3)
00437cc2  fstp       st(3)
00437cc4  fstp       st(2)
00437cc6  fstp       st(1)
00437cc8  fstp       st(0)
00437cca  pop        esi
00437ccb  mov        esp, ebp
00437ccd  pop        ebp
00437cce  ret        0xc

// block 00437cd1
00437cd1  fstp       st(4)
00437cd3  xor        eax, eax
00437cd5  fstp       st(4)
00437cd7  fstp       st(4)
00437cd9  fstp       st(0)
00437cdb  fstp       st(1)
00437cdd  fstp       st(1)
00437cdf  fstp       st(0)
00437ce1  pop        esi
00437ce2  mov        esp, ebp
00437ce4  pop        ebp
00437ce5  ret        0xc

// block 00437ce8
00437ce8  fstp       st(3)
00437cea  fstp       st(3)
00437cec  fstp       st(3)

// block 00437cee
00437cee  fstp       st(0)
00437cf0  fstp       st(0)
00437cf2  fstp       st(0)

// block 00437cf4
00437cf4  xor        eax, eax
00437cf6  pop        esi
00437cf7  mov        esp, ebp
00437cf9  pop        ebp
00437cfa  ret        0xc
