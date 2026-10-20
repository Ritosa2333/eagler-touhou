
// block 00435ae0
00435ae0  mov        eax, dword ptr [0x4b0c90]
00435ae5  sub        esp, 0x10
00435ae8  push       ebx
00435ae9  mov        ebx, dword ptr [eax*4 + 0x4b3184]
00435af0  push       ebp
00435af1  push       esi
00435af2  mov        ecx, 7
00435af7  call       0x45fe60

// block 00435afc
00435afc  xor        ebp, ebp
00435afe  mov        dword ptr [edi + 0x10], eax
00435b01  cmp        eax, ebp
00435b03  jne        0x435b21

// block 00435b05
00435b05  push       0x4a10d8
00435b0a  mov        ecx, 0x4b0ec8
00435b0f  call       0x464220

// block 00435b14
00435b14  add        esp, 4
00435b17  or         eax, 0xffffffff
00435b1a  pop        esi
00435b1b  pop        ebp
00435b1c  pop        ebx
00435b1d  add        esp, 0x10
00435b20  ret        

// block 00435b21
00435b21  mov        eax, dword ptr [0x4ce8a8]
00435b26  cmp        eax, ebp
00435b28  je         0x435b38

// block 00435b2a
00435b2a  mov        dword ptr [edi + 0xa2c], eax
00435b30  mov        dword ptr [0x4ce8a8], ebp
00435b36  jmp        0x435b59

// block 00435b38
00435b38  mov        ecx, dword ptr [0x4b0c90]
00435b3e  mov        edx, dword ptr [0x4b0c94]
00435b44  lea        eax, [edx + ecx*2]
00435b47  mov        eax, dword ptr [eax*4 + 0x4b3190]
00435b4e  mov        esi, edi
00435b50  call       0x437680

// block 00435b55
00435b55  test       eax, eax
00435b57  jne        0x435b05

// block 00435b59
00435b59  push       0x24
00435b5b  call       0x46c9ea

// block 00435b60
00435b60  add        esp, 4
00435b63  cmp        eax, ebp
00435b65  je         0x435b83

// block 00435b67
00435b67  and        dword ptr [eax + 4], 0xfffffffe
00435b6b  mov        dword ptr [eax + 8], ebp
00435b6e  mov        dword ptr [eax + 0xc], ebp
00435b71  mov        dword ptr [eax + 0x10], ebp
00435b74  mov        dword ptr [eax], ebp
00435b76  mov        dword ptr [eax + 0x14], eax
00435b79  mov        dword ptr [eax + 0x18], ebp
00435b7c  mov        dword ptr [eax + 0x1c], ebp
00435b7f  mov        esi, eax
00435b81  jmp        0x435b85

// block 00435b83
00435b83  xor        esi, esi

// block 00435b85
00435b85  mov        ecx, dword ptr [esi + 4]
00435b88  and        ecx, 0xfffffffd
00435b8b  or         ecx, 1
00435b8e  mov        ebx, 0x10
00435b93  mov        dword ptr [esi + 8], 0x437660
00435b9a  mov        dword ptr [esi + 0xc], ebp
00435b9d  mov        dword ptr [esi + 0x10], ebp
00435ba0  mov        dword ptr [esi + 0x20], edi
00435ba3  mov        dword ptr [esi + 4], ecx
00435ba6  call       0x462380

// block 00435bab
00435bab  push       0x24
00435bad  mov        dword ptr [edi + 8], esi
00435bb0  call       0x46c9ea

// block 00435bb5
00435bb5  add        esp, 4
00435bb8  cmp        eax, ebp
00435bba  je         0x435bd8

// block 00435bbc
00435bbc  and        dword ptr [eax + 4], 0xfffffffe
00435bc0  mov        dword ptr [eax + 8], ebp
00435bc3  mov        dword ptr [eax + 0xc], ebp
00435bc6  mov        dword ptr [eax + 0x10], ebp
00435bc9  mov        dword ptr [eax], ebp
00435bcb  mov        dword ptr [eax + 0x14], eax
00435bce  mov        dword ptr [eax + 0x18], ebp
00435bd1  mov        dword ptr [eax + 0x1c], ebp
00435bd4  mov        esi, eax
00435bd6  jmp        0x435bda

// block 00435bd8
00435bd8  xor        esi, esi

// block 00435bda
00435bda  mov        edx, dword ptr [esi + 4]
00435bdd  and        edx, 0xfffffffd
00435be0  or         edx, 1
00435be3  mov        ebx, 0x18
00435be8  mov        dword ptr [esi + 8], 0x437670
00435bef  mov        dword ptr [esi + 0xc], ebp
00435bf2  mov        dword ptr [esi + 0x10], ebp
00435bf5  mov        dword ptr [esi + 0x20], edi
00435bf8  mov        dword ptr [esi + 4], edx
00435bfb  call       0x462420

// block 00435c00
00435c00  mov        ebx, dword ptr [edi + 0x10]
00435c03  mov        dword ptr [edi + 0xc], esi
00435c06  lea        esi, [edi + 0x14]
00435c09  call       0x402520

// block 00435c0e
00435c0e  mov        al, 0x10
00435c10  push       ebp
00435c11  push       esi
00435c12  mov        ecx, ebx
00435c14  mov        byte ptr [esi + 0x49d], al
00435c1a  mov        byte ptr [esi + 0x49c], al
00435c20  call       0x454d10

// block 00435c25
00435c25  fldz       
00435c27  mov        esi, dword ptr [edi + 0xa2c]
00435c2d  fst        dword ptr [edi + 0x97c]
00435c33  fld        dword ptr [0x4a43dc]
00435c39  mov        dword ptr [edi + 0x988], ebp
00435c3f  fstp       dword ptr [edi + 0x980]
00435c45  mov        dword ptr [edi + 0x98c], 0xc800
00435c4f  fld        dword ptr [esi + 0x10]
00435c52  fld        qword ptr [0x4a3d48]
00435c58  fmul       st(1), st(0)
00435c5a  fxch       st(1)
00435c5c  call       0x4931e0

// block 00435c61
00435c61  mov        dword ptr [edi + 0x990], eax
00435c67  fld        dword ptr [esi + 0x14]
00435c6a  fmul       st(1)
00435c6c  call       0x4931e0

// block 00435c71
00435c71  mov        dword ptr [edi + 0x994], eax
00435c77  fld        dword ptr [esi + 0x18]
00435c7a  fmul       st(1)
00435c7c  call       0x4931e0

// block 00435c81
00435c81  mov        dword ptr [edi + 0x998], eax
00435c87  fmul       dword ptr [esi + 0x1c]
00435c8a  call       0x4931e0

// block 00435c8f
00435c8f  mov        dword ptr [edi + 0x99c], eax
00435c95  mov        dword ptr [esi + 0x24], 0x64
00435c9c  mov        eax, dword ptr [edi + 0xa2c]
00435ca2  mov        ecx, dword ptr [eax + 0x24]
00435ca5  imul       ecx, dword ptr [eax + 0x20]
00435ca9  mov        dword ptr [0x4b0cd0], ecx
00435caf  mov        edx, dword ptr [edi + 0xa2c]
00435cb5  mov        eax, dword ptr [edx + 0x24]
00435cb8  mov        dword ptr [0x4b0cd4], eax
00435cbd  mov        eax, dword ptr [edi + 0xc430]
00435cc3  mov        edx, 0xfff0bdc1
00435cc8  mov        ecx, 0x4b2ed0
00435ccd  test       al, 1
00435ccf  jne        0x435cf2

// block 00435cd1
00435cd1  or         eax, 1
00435cd4  fst        dword ptr [edi + 0xc428]
00435cda  mov        dword ptr [edi + 0xc424], ebp
00435ce0  mov        dword ptr [edi + 0xc420], edx
00435ce6  mov        dword ptr [edi + 0xc42c], ecx
00435cec  mov        dword ptr [edi + 0xc430], eax

// block 00435cf2
00435cf2  mov        dword ptr [edi + 0xc420], 0xfffffffe
00435cfc  fld        dword ptr [0x4a3da8]
00435d02  fstp       dword ptr [edi + 0xc428]
00435d08  or         esi, 0xffffffff
00435d0b  mov        dword ptr [edi + 0xc424], esi
00435d11  mov        ebx, dword ptr [0x4b0c90]
00435d17  fld        dword ptr [ebx*4 + 0x4b31a8]
00435d1e  mov        eax, dword ptr [edi + 0xa2c]
00435d24  fstp       dword ptr [eax + 4]
00435d27  mov        ebx, dword ptr [0x4b0c90]
00435d2d  fld        dword ptr [ebx*4 + 0x4b31b4]
00435d34  mov        eax, dword ptr [edi + 0xa2c]
00435d3a  fstp       dword ptr [eax + 0xc]
00435d3d  mov        ebx, dword ptr [0x4b0c90]
00435d43  fld        dword ptr [ebx*4 + 0x4b31cc]
00435d4a  mov        eax, dword ptr [edi + 0xa2c]
00435d50  fstp       dword ptr [eax + 8]
00435d53  mov        eax, dword ptr [edi + 0xa2c]
00435d59  fld        dword ptr [eax + 4]
00435d5c  fld        qword ptr [0x4a3cf8]
00435d62  fmul       st(1), st(0)
00435d64  fxch       st(1)
00435d66  fstp       dword ptr [esp + 0xc]
00435d6a  fld        dword ptr [esp + 0xc]
00435d6e  fst        dword ptr [edi + 0x9e8]
00435d74  fstp       dword ptr [edi + 0x9e4]
00435d7a  fld        dword ptr [0x4a3e38]
00435d80  fst        dword ptr [edi + 0x9ec]
00435d86  mov        eax, dword ptr [0x4b0c90]
00435d8b  fld        dword ptr [eax*4 + 0x4b31b4]
00435d92  fmul       st(2)
00435d94  fstp       dword ptr [esp + 0xc]
00435d98  fld        dword ptr [esp + 0xc]
00435d9c  fst        dword ptr [edi + 0x9f4]
00435da2  fstp       dword ptr [edi + 0x9f0]
00435da8  fst        dword ptr [edi + 0x9f8]
00435dae  mov        eax, dword ptr [0x4b0c90]
00435db3  fld        dword ptr [eax*4 + 0x4b31c0]
00435dba  fmulp      st(2)
00435dbc  fxch       st(1)
00435dbe  fstp       dword ptr [esp + 0xc]
00435dc2  fld        dword ptr [esp + 0xc]
00435dc6  fst        dword ptr [edi + 0xa00]
00435dcc  fstp       dword ptr [edi + 0x9fc]
00435dd2  fstp       dword ptr [edi + 0xa04]
00435dd8  fld        dword ptr [edi + 0x97c]
00435dde  fsub       dword ptr [edi + 0x9e4]
00435de4  fstp       dword ptr [esp + 0x10]
00435de8  mov        eax, dword ptr [esp + 0x10]
00435dec  fld        dword ptr [edi + 0x980]
00435df2  fsub       dword ptr [edi + 0x9e8]
00435df8  fstp       dword ptr [esp + 0x14]
00435dfc  fld        dword ptr [edi + 0x984]
00435e02  fsub       dword ptr [edi + 0x9ec]
00435e08  mov        dword ptr [edi + 0x9cc], eax
00435e0e  mov        eax, dword ptr [esp + 0x14]
00435e12  mov        dword ptr [edi + 0x9d0], eax
00435e18  fstp       dword ptr [esp + 0x18]
00435e1c  mov        eax, dword ptr [esp + 0x18]
00435e20  mov        dword ptr [edi + 0x9d4], eax
00435e26  fld        dword ptr [edi + 0x9e4]
00435e2c  fadd       dword ptr [edi + 0x97c]
00435e32  fstp       dword ptr [esp + 0x10]
00435e36  mov        eax, dword ptr [esp + 0x10]
00435e3a  fld        dword ptr [edi + 0x9e8]
00435e40  fadd       dword ptr [edi + 0x980]
00435e46  fstp       dword ptr [esp + 0x14]
00435e4a  fld        dword ptr [edi + 0x9ec]
00435e50  fadd       dword ptr [edi + 0x984]
00435e56  mov        dword ptr [edi + 0x9d8], eax
00435e5c  mov        eax, dword ptr [esp + 0x14]

// block 00435e60
00435e60  mov        dword ptr [edi + 0x9dc], eax
00435e66  fstp       dword ptr [esp + 0x18]
00435e6a  mov        eax, dword ptr [esp + 0x18]
00435e6e  mov        dword ptr [edi + 0x9e0], eax
00435e74  fld        dword ptr [edi + 0x97c]
00435e7a  fsub       dword ptr [edi + 0x9f0]

// block 00435e80
00435e80  fstp       dword ptr [esp + 0x10]
00435e84  fld        dword ptr [edi + 0x980]
00435e8a  fsub       dword ptr [edi + 0x9f4]
00435e90  fstp       dword ptr [esp + 0x14]
00435e94  fld        dword ptr [edi + 0x984]
00435e9a  mov        eax, dword ptr [esp + 0x10]
00435e9e  fsub       dword ptr [edi + 0x9f8]
00435ea4  mov        dword ptr [edi + 0xc444], eax
00435eaa  mov        eax, dword ptr [esp + 0x14]
00435eae  mov        dword ptr [edi + 0xc448], eax
00435eb4  fstp       dword ptr [esp + 0x18]
00435eb8  mov        eax, dword ptr [esp + 0x18]
00435ebc  mov        dword ptr [edi + 0xc44c], eax
00435ec2  fld        dword ptr [edi + 0x9f0]
00435ec8  fadd       dword ptr [edi + 0x97c]
00435ece  fstp       dword ptr [esp + 0x10]
00435ed2  mov        eax, dword ptr [esp + 0x10]
00435ed6  fld        dword ptr [edi + 0x9f4]
00435edc  fadd       dword ptr [edi + 0x980]
00435ee2  fstp       dword ptr [esp + 0x14]
00435ee6  fld        dword ptr [edi + 0x9f8]
00435eec  fadd       dword ptr [edi + 0x984]
00435ef2  mov        dword ptr [edi + 0xc450], eax
00435ef8  mov        eax, dword ptr [esp + 0x14]
00435efc  mov        dword ptr [edi + 0xc454], eax
00435f02  fstp       dword ptr [esp + 0x18]
00435f06  mov        eax, dword ptr [esp + 0x18]
00435f0a  mov        dword ptr [edi + 0xc458], eax
00435f10  fld        dword ptr [edi + 0x97c]
00435f16  fsub       dword ptr [edi + 0x9fc]
00435f1c  fstp       dword ptr [esp + 0x10]
00435f20  mov        eax, dword ptr [esp + 0x10]
00435f24  fld        dword ptr [edi + 0x980]
00435f2a  fsub       dword ptr [edi + 0xa00]
00435f30  fstp       dword ptr [esp + 0x14]
00435f34  fld        dword ptr [edi + 0x984]
00435f3a  fsub       dword ptr [edi + 0xa04]
00435f40  mov        dword ptr [edi + 0xc45c], eax
00435f46  mov        eax, dword ptr [esp + 0x14]
00435f4a  mov        dword ptr [edi + 0xc460], eax
00435f50  fstp       dword ptr [esp + 0x18]
00435f54  mov        eax, dword ptr [esp + 0x18]
00435f58  mov        dword ptr [edi + 0xc464], eax
00435f5e  fld        dword ptr [edi + 0x9fc]
00435f64  fadd       dword ptr [edi + 0x97c]
00435f6a  fstp       dword ptr [esp + 0x10]
00435f6e  mov        eax, dword ptr [esp + 0x10]
00435f72  fld        dword ptr [edi + 0xa00]
00435f78  fadd       dword ptr [edi + 0x980]
00435f7e  fstp       dword ptr [esp + 0x14]
00435f82  fld        dword ptr [edi + 0xa04]
00435f88  fadd       dword ptr [edi + 0x984]
00435f8e  mov        dword ptr [edi + 0xc468], eax
00435f94  mov        eax, dword ptr [esp + 0x14]
00435f98  mov        dword ptr [edi + 0xc46c], eax
00435f9e  fstp       dword ptr [esp + 0x18]
00435fa2  mov        eax, dword ptr [esp + 0x18]
00435fa6  mov        dword ptr [edi + 0xc470], eax
00435fac  fld        dword ptr [edi + 0x97c]
00435fb2  fsub       dword ptr [edi + 0x9fc]
00435fb8  fstp       dword ptr [esp + 0x10]
00435fbc  mov        eax, dword ptr [esp + 0x10]
00435fc0  fld        dword ptr [edi + 0x980]
00435fc6  fsub       dword ptr [edi + 0xa00]
00435fcc  fstp       dword ptr [esp + 0x14]
00435fd0  fld        dword ptr [edi + 0x984]
00435fd6  fsub       dword ptr [edi + 0xa04]
00435fdc  mov        dword ptr [edi + 0xc474], eax
00435fe2  mov        eax, dword ptr [esp + 0x14]
00435fe6  mov        dword ptr [edi + 0xc478], eax
00435fec  fstp       dword ptr [esp + 0x18]

// block 00435ff0
00435ff0  mov        eax, dword ptr [esp + 0x18]
00435ff4  mov        dword ptr [edi + 0xc47c], eax
00435ffa  fld        dword ptr [edi + 0x9fc]
00436000  fadd       dword ptr [edi + 0x97c]
00436006  fstp       dword ptr [esp + 0x10]
0043600a  mov        eax, dword ptr [esp + 0x10]

// block 0043600e
0043600e  fld        dword ptr [edi + 0xa00]
00436014  fadd       dword ptr [edi + 0x980]
0043601a  fstp       dword ptr [esp + 0x14]
0043601e  fld        dword ptr [edi + 0xa04]
00436024  fadd       dword ptr [edi + 0x984]
0043602a  mov        dword ptr [edi + 0xc480], eax
00436030  mov        eax, dword ptr [esp + 0x14]
00436034  fstp       dword ptr [esp + 0x18]
00436038  mov        dword ptr [edi + 0xc484], eax
0043603e  mov        eax, dword ptr [esp + 0x18]
00436042  mov        dword ptr [edi + 0xc488], eax
00436048  mov        eax, dword ptr [edi + 0xa40]
0043604e  test       al, 1
00436050  jne        0x436073

// block 00436052
00436052  or         eax, 1
00436055  fst        dword ptr [edi + 0xa38]
0043605b  mov        dword ptr [edi + 0xa34], ebp
00436061  mov        dword ptr [edi + 0xa30], edx
00436067  mov        dword ptr [edi + 0xa3c], ecx
0043606d  mov        dword ptr [edi + 0xa40], eax

// block 00436073
00436073  fst        dword ptr [edi + 0xa38]
00436079  mov        dword ptr [edi + 0xa34], ebp
0043607f  mov        dword ptr [edi + 0xa30], esi
00436085  mov        eax, dword ptr [edi + 0xc410]
0043608b  test       al, 1
0043608d  jne        0x4360b2

// block 0043608f
0043608f  or         eax, 1
00436092  fstp       dword ptr [edi + 0xc408]
00436098  mov        dword ptr [edi + 0xc404], ebp
0043609e  mov        dword ptr [edi + 0xc400], edx
004360a4  mov        dword ptr [edi + 0xc40c], ecx
004360aa  mov        dword ptr [edi + 0xc410], eax
004360b0  jmp        0x4360b4

// block 004360b2
004360b2  fstp       st(0)

// block 004360b4
004360b4  fld        dword ptr [0x4a43d8]
004360ba  mov        dword ptr [edi + 0xc404], 0x78
004360c4  fstp       dword ptr [edi + 0xc408]
004360ca  mov        dword ptr [edi + 0xc400], 0x77
004360d4  fld1       
004360d6  pop        esi
004360d7  mov        dword ptr [edi + 0xc41c], ebp
004360dd  fstp       dword ptr [edi + 0x825c]
004360e3  pop        ebp
004360e4  mov        dword ptr [edi + 0xc3fc], 0x1e
004360ee  xor        eax, eax
004360f0  pop        ebx
004360f1  add        esp, 0x10
004360f4  ret        
