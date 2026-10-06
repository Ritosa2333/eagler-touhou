
// block 00430be0
00430be0  sub        esp, 0xc
00430be3  fldz       
00430be5  push       edi
00430be6  fst        dword ptr [esp + 4]
00430bea  mov        dword ptr [0x4cecdc], 0x1e0
00430bf4  mov        eax, dword ptr [esp + 4]
00430bf8  fst        dword ptr [esp + 8]
00430bfc  mov        edx, dword ptr [esp + 8]
00430c00  fst        dword ptr [esp + 0xc]
00430c04  fst        dword ptr [esp + 4]
00430c08  mov        dword ptr [0x4cec04], eax
00430c0d  mov        eax, dword ptr [esp + 0xc]
00430c11  fst        dword ptr [esp + 8]
00430c15  fst        dword ptr [esp + 0xc]
00430c19  mov        dword ptr [0x4cec08], edx
00430c1f  mov        edx, dword ptr [esp + 4]
00430c23  fst        dword ptr [esp + 4]
00430c27  fld1       
00430c29  mov        dword ptr [0x4cec0c], eax
00430c2e  mov        eax, dword ptr [esp + 8]
00430c32  fst        dword ptr [esp + 8]
00430c36  mov        dword ptr [0x4cec10], edx
00430c3c  fxch       st(1)
00430c3e  mov        edx, dword ptr [esp + 0xc]
00430c42  fst        dword ptr [esp + 0xc]
00430c46  fld        dword ptr [0x4a3e0c]
00430c4c  mov        dword ptr [0x4cec14], eax
00430c51  mov        eax, dword ptr [esp + 4]
00430c55  fst        dword ptr [0x4cec4c]
00430c5b  fxch       st(1)
00430c5d  mov        dword ptr [0x4cec18], edx
00430c63  mov        edx, dword ptr [esp + 8]
00430c67  fst        dword ptr [0x4cece0]
00430c6d  fxch       st(2)
00430c6f  mov        dword ptr [0x4cec1c], eax
00430c74  mov        eax, dword ptr [esp + 0xc]
00430c78  fst        dword ptr [0x4cece4]
00430c7e  fxch       st(2)
00430c80  mov        dword ptr [0x4cec20], edx
00430c86  fst        dword ptr [esp + 4]
00430c8a  mov        dword ptr [0x4cec24], eax
00430c8f  fst        dword ptr [esp + 8]
00430c93  xor        eax, eax
00430c95  mov        edi, dword ptr [esp + 4]
00430c99  fst        dword ptr [esp + 0xc]
00430c9d  mov        dword ptr [0x4cec40], edi
00430ca3  fst        dword ptr [esp + 4]
00430ca7  mov        edi, dword ptr [esp + 8]
00430cab  fst        dword ptr [esp + 8]
00430caf  fld        dword ptr [0x4a3de4]
00430cb5  mov        dword ptr [0x4cec44], edi
00430cbb  mov        edi, dword ptr [esp + 0xc]
00430cbf  fst        dword ptr [esp + 0xc]
00430cc3  mov        dword ptr [0x4cec48], edi
00430cc9  fxch       st(1)
00430ccb  mov        edi, dword ptr [esp + 4]
00430ccf  fst        dword ptr [esp + 4]
00430cd3  mov        dword ptr [0x4ceaec], edi
00430cd9  mov        edi, dword ptr [esp + 8]
00430cdd  fst        dword ptr [esp + 8]
00430ce1  mov        dword ptr [0x4ceaf0], edi
00430ce7  mov        edi, dword ptr [esp + 0xc]
00430ceb  fst        dword ptr [esp + 0xc]
00430cef  mov        dword ptr [0x4ceaf4], edi
00430cf5  mov        edi, dword ptr [esp + 4]
00430cf9  fst        dword ptr [esp + 4]
00430cfd  mov        dword ptr [0x4ceaf8], edi
00430d03  mov        edi, dword ptr [esp + 8]
00430d07  mov        dword ptr [0x4ceafc], edi
00430d0d  mov        edi, dword ptr [esp + 0xc]
00430d11  fst        dword ptr [esp + 0xc]
00430d15  mov        dword ptr [0x4ceb00], edi
00430d1b  fxch       st(3)
00430d1d  mov        edi, dword ptr [esp + 4]
00430d21  fst        dword ptr [esp + 8]
00430d25  mov        edx, 0x280
00430d2a  mov        dword ptr [0x4ceb04], edi
00430d30  mov        edi, dword ptr [esp + 8]
00430d34  mov        dword ptr [0x4cecd0], eax
00430d39  mov        dword ptr [0x4cecd4], eax
00430d3e  mov        dword ptr [0x4cecd8], edx
00430d44  mov        dword ptr [0x4cece8], 1
00430d4e  fxch       st(2)
00430d50  mov        dword ptr [0x4ceb08], edi
00430d56  mov        edi, dword ptr [esp + 0xc]
00430d5a  fst        dword ptr [0x4ceb34]
00430d60  fxch       st(3)
00430d62  mov        dword ptr [0x4ceb0c], edi
00430d68  fst        dword ptr [0x4cebc8]

// block 00430d6e
00430d6e  mov        dword ptr [0x4cebd0], eax
00430d73  fxch       st(2)
00430d75  fst        dword ptr [0x4cebcc]
00430d7b  fxch       st(2)
00430d7d  fst        dword ptr [esp + 4]
00430d81  mov        edi, dword ptr [esp + 4]
00430d85  mov        dword ptr [0x4ceb28], edi
00430d8b  fst        dword ptr [esp + 8]
00430d8f  mov        edi, dword ptr [esp + 8]
00430d93  fst        dword ptr [esp + 0xc]
00430d97  mov        dword ptr [0x4ceb2c], edi
00430d9d  mov        edi, dword ptr [esp + 0xc]
00430da1  mov        dword ptr [0x4ceb30], edi
00430da7  mov        edi, 0x4000
00430dac  test       dword ptr [ecx + 0x590], edi
00430db2  je         0x430dde

// block 00430db4
00430db4  mov        dword ptr [0x4cebb8], 0x20
00430dbe  mov        dword ptr [0x4cebbc], 0x10
00430dc8  mov        dword ptr [0x4cebc0], 0x180
00430dd2  mov        dword ptr [0x4cebc4], 0x1c0
00430ddc  jmp        0x430df8

// block 00430dde
00430dde  mov        dword ptr [0x4cebb8], eax
00430de3  mov        dword ptr [0x4cebbc], eax
00430de8  mov        dword ptr [0x4cebc0], edx
00430dee  mov        dword ptr [0x4cebc4], 0x1e0

// block 00430df8
00430df8  fst        dword ptr [esp + 4]
00430dfc  mov        edx, dword ptr [esp + 4]
00430e00  mov        dword ptr [0x4ced1c], edx
00430e06  fst        dword ptr [esp + 8]
00430e0a  mov        edx, dword ptr [esp + 8]
00430e0e  fxch       st(1)
00430e10  fstp       dword ptr [esp + 0xc]
00430e14  mov        dword ptr [0x4ced20], edx
00430e1a  mov        edx, dword ptr [esp + 0xc]
00430e1e  mov        dword ptr [0x4ced24], edx
00430e24  fst        dword ptr [esp + 4]
00430e28  mov        edx, dword ptr [esp + 4]
00430e2c  fst        dword ptr [esp + 8]
00430e30  mov        dword ptr [0x4ced28], edx
00430e36  fst        dword ptr [esp + 0xc]
00430e3a  mov        edx, dword ptr [esp + 8]
00430e3e  fst        dword ptr [esp + 4]
00430e42  mov        dword ptr [0x4ced2c], edx
00430e48  mov        edx, dword ptr [esp + 0xc]
00430e4c  fst        dword ptr [esp + 0xc]
00430e50  mov        dword ptr [0x4ced30], edx
00430e56  fxch       st(1)
00430e58  mov        edx, dword ptr [esp + 4]
00430e5c  fst        dword ptr [esp + 8]
00430e60  mov        dword ptr [0x4ced34], edx
00430e66  fxch       st(2)
00430e68  mov        edx, dword ptr [esp + 8]
00430e6c  fstp       dword ptr [0x4ced64]
00430e72  mov        dword ptr [0x4ced38], edx
00430e78  mov        edx, dword ptr [esp + 0xc]
00430e7c  fst        dword ptr [0x4cedf8]
00430e82  fxch       st(1)
00430e84  mov        dword ptr [0x4ced3c], edx
00430e8a  fstp       dword ptr [0x4cedfc]
00430e90  mov        dword ptr [0x4cee00], eax
00430e95  fst        dword ptr [esp + 4]
00430e99  mov        edx, dword ptr [esp + 4]
00430e9d  mov        dword ptr [0x4ced58], edx
00430ea3  fst        dword ptr [esp + 8]
00430ea7  mov        edx, dword ptr [esp + 8]
00430eab  fstp       dword ptr [esp + 0xc]
00430eaf  mov        dword ptr [0x4ced5c], edx
00430eb5  mov        edx, dword ptr [esp + 0xc]
00430eb9  mov        dword ptr [0x4ced60], edx
00430ebf  test       dword ptr [ecx + 0x590], edi
00430ec5  pop        edi
00430ec6  mov        dword ptr [0x4cedec], eax
00430ecb  je         0x430eef

// block 00430ecd
00430ecd  mov        dword ptr [0x4cede8], 0xd
00430ed7  mov        dword ptr [0x4cedf0], 0x1a6
00430ee1  mov        dword ptr [0x4cedf4], 0x1e0
00430eeb  add        esp, 0xc
00430eee  ret        

// block 00430eef
00430eef  mov        dword ptr [0x4cede8], eax
00430ef4  mov        eax, 0x200
00430ef9  mov        dword ptr [0x4cedf0], eax
00430efe  mov        dword ptr [0x4cedf4], eax
00430f03  add        esp, 0xc
00430f06  ret        
