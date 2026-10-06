
// block 0045c900
0045c900  push       ebp
0045c901  mov        ebp, esp
0045c903  and        esp, 0xfffffff8
0045c906  sub        esp, 0x24
0045c909  push       ebx
0045c90a  mov        ebx, eax
0045c90c  mov        eax, dword ptr [ebx + 0x47c]
0045c912  push       esi
0045c913  push       edi
0045c914  test       al, 1
0045c916  jne        0x45c924

// block 0045c918
0045c918  or         eax, 0xffffffff
0045c91b  pop        edi
0045c91c  pop        esi
0045c91d  pop        ebx
0045c91e  mov        esp, ebp
0045c920  pop        ebp
0045c921  ret        4

// block 0045c924
0045c924  test       al, 2
0045c926  je         0x45c918

// block 0045c928
0045c928  shr        eax, 0x17
0045c92b  and        eax, 0x1f
0045c92e  cmp        eax, 0x14
0045c931  ja         0x45cd74

// block 0045c937
0045c937  jmp        dword ptr [eax*4 + 0x45cd80]

// block 0045c93e
0045c93e  cmp        byte ptr [ebx + 0x3bf], 0
0045c945  je         0x45c918

// block 0045c947
0045c947  mov        eax, dword ptr [ebp + 8]
0045c94a  push       eax
0045c94b  mov        eax, ebx
0045c94d  call       0x45a9f0

// block 0045c952
0045c952  pop        edi
0045c953  pop        esi
0045c954  pop        ebx
0045c955  mov        esp, ebp
0045c957  pop        ebp
0045c958  ret        4

// block 0045c95b
0045c95b  cmp        byte ptr [ebx + 0x3bf], 0
0045c962  je         0x45c918

// block 0045c964
0045c964  mov        ecx, dword ptr [ebp + 8]
0045c967  push       ecx
0045c968  mov        eax, ebx
0045c96a  call       0x45b1b0

// block 0045c96f
0045c96f  pop        edi
0045c970  pop        esi
0045c971  pop        ebx
0045c972  mov        esp, ebp
0045c974  pop        ebp
0045c975  ret        4

// block 0045c978
0045c978  cmp        byte ptr [ebx + 0x3bf], 0
0045c97f  je         0x45c918

// block 0045c981
0045c981  mov        edx, dword ptr [ebp + 8]
0045c984  push       edx
0045c985  mov        esi, ebx
0045c987  call       0x45b5e0

// block 0045c98c
0045c98c  pop        edi
0045c98d  pop        esi
0045c98e  pop        ebx
0045c98f  mov        esp, ebp
0045c991  pop        ebp
0045c992  ret        4

// block 0045c995
0045c995  cmp        byte ptr [ebx + 0x3bf], 0
0045c99c  je         0x45c918

// block 0045c9a2
0045c9a2  mov        esi, dword ptr [ebp + 8]
0045c9a5  mov        eax, ebx
0045c9a7  call       0x45ba90

// block 0045c9ac
0045c9ac  pop        edi
0045c9ad  pop        esi
0045c9ae  pop        ebx
0045c9af  mov        esp, ebp
0045c9b1  pop        ebp
0045c9b2  ret        4

// block 0045c9b5
0045c9b5  cmp        byte ptr [ebx + 0x3bf], 0
0045c9bc  je         0x45c918

// block 0045c9c2
0045c9c2  mov        edi, dword ptr [ebp + 8]
0045c9c5  mov        esi, ebx
0045c9c7  call       0x45b610

// block 0045c9cc
0045c9cc  pop        edi
0045c9cd  pop        esi
0045c9ce  pop        ebx
0045c9cf  mov        esp, ebp
0045c9d1  pop        ebp
0045c9d2  ret        4

// block 0045c9d5
0045c9d5  cmp        byte ptr [ebx + 0x3bf], 0
0045c9dc  je         0x45c918

// block 0045c9e2
0045c9e2  mov        eax, dword ptr [ebp + 8]
0045c9e5  push       ebx
0045c9e6  push       eax
0045c9e7  call       0x45bac0

// block 0045c9ec
0045c9ec  pop        edi
0045c9ed  pop        esi
0045c9ee  pop        ebx
0045c9ef  mov        esp, ebp
0045c9f1  pop        ebp
0045c9f2  ret        4

// block 0045c9f5
0045c9f5  cmp        byte ptr [ebx + 0x3bf], 0
0045c9fc  je         0x45c918

// block 0045ca02
0045ca02  mov        ecx, dword ptr [ebp + 8]
0045ca05  push       ebx
0045ca06  push       ecx
0045ca07  call       0x45bce0

// block 0045ca0c
0045ca0c  pop        edi
0045ca0d  pop        esi
0045ca0e  pop        ebx
0045ca0f  mov        esp, ebp
0045ca11  pop        ebp
0045ca12  ret        4

// block 0045ca15
0045ca15  cmp        byte ptr [ebx + 0x3bf], 0
0045ca1c  je         0x45c918

// block 0045ca22
0045ca22  mov        edi, 0x4ce8e8
0045ca27  call       0x4302c0

// block 0045ca2c
0045ca2c  mov        edx, dword ptr [ebp + 8]
0045ca2f  push       ebx
0045ca30  push       edx
0045ca31  call       0x45bce0

// block 0045ca36
0045ca36  call       0x430300

// block 0045ca3b
0045ca3b  xor        eax, eax
0045ca3d  pop        edi
0045ca3e  pop        esi
0045ca3f  pop        ebx
0045ca40  mov        esp, ebp
0045ca42  pop        ebp
0045ca43  ret        4

// block 0045ca46
0045ca46  mov        eax, dword ptr [ebx + 0x3fc]
0045ca4c  mov        ecx, dword ptr [ebx + 0x478]
0045ca52  add        eax, eax
0045ca54  push       eax
0045ca55  push       ecx
0045ca56  mov        ecx, dword ptr [ebp + 8]
0045ca59  mov        eax, ebx
0045ca5b  call       0x45c3a0

// block 0045ca60
0045ca60  pop        edi
0045ca61  pop        esi
0045ca62  pop        ebx
0045ca63  mov        esp, ebp
0045ca65  pop        ebp
0045ca66  ret        4

// block 0045ca69
0045ca69  mov        edx, dword ptr [ebx + 0x3fc]
0045ca6f  mov        eax, dword ptr [ebx + 0x478]
0045ca75  mov        ecx, dword ptr [ebp + 8]
0045ca78  add        edx, edx
0045ca7a  push       edx
0045ca7b  push       eax
0045ca7c  push       ecx
0045ca7d  mov        eax, ebx
0045ca7f  call       0x45c800

// block 0045ca84
0045ca84  pop        edi
0045ca85  pop        esi
0045ca86  pop        ebx
0045ca87  mov        esp, ebp
0045ca89  pop        ebp
0045ca8a  ret        4

// block 0045ca8d
0045ca8d  cmp        byte ptr [ebx + 0x3bf], 0
0045ca94  je         0x45c918

// block 0045ca9a
0045ca9a  mov        edx, dword ptr [ebp + 8]
0045ca9d  push       ebx
0045ca9e  push       edx
0045ca9f  call       0x45ae50

// block 0045caa4
0045caa4  pop        edi
0045caa5  pop        esi
0045caa6  pop        ebx
0045caa7  mov        esp, ebp
0045caa9  pop        ebp
0045caaa  ret        4

// block 0045caad
0045caad  cmp        byte ptr [ebx + 0x3bf], 0
0045cab4  je         0x45c918

// block 0045caba
0045caba  mov        eax, dword ptr [ebp + 8]
0045cabd  push       eax
0045cabe  mov        eax, ebx
0045cac0  call       0x45b1e0

// block 0045cac5
0045cac5  pop        edi
0045cac6  pop        esi
0045cac7  pop        ebx
0045cac8  mov        esp, ebp
0045caca  pop        ebp
0045cacb  ret        4

// block 0045cace
0045cace  fld        dword ptr [ebx + 0x2c]
0045cad1  lea        eax, [esp + 0x18]
0045cad5  fstp       dword ptr [esp + 0xc]
0045cad9  mov        ecx, ebx
0045cadb  fld        dword ptr [ebx + 0x58]
0045cade  fmul       dword ptr [ebx + 0x40]
0045cae1  fstp       dword ptr [esp + 0x14]
0045cae5  fld        dword ptr [ebx + 0x5c]
0045cae8  fmul       dword ptr [ebx + 0x44]
0045caeb  fstp       dword ptr [esp + 0x10]
0045caef  call       0x45d8c0

// block 0045caf4
0045caf4  mov        ecx, dword ptr [ebx + 0x3c]
0045caf7  test       ecx, ecx
0045caf9  je         0x45cb40

// block 0045cafb
0045cafb  lea        eax, [esp + 0x24]
0045caff  call       0x45d8c0

// block 0045cb04
0045cb04  fld        dword ptr [esp + 0x24]
0045cb08  fadd       dword ptr [esp + 0x18]
0045cb0c  mov        eax, dword ptr [ebx + 0x3c]
0045cb0f  fstp       dword ptr [esp + 0x18]
0045cb13  fld        dword ptr [esp + 0x28]
0045cb17  fadd       dword ptr [esp + 0x1c]
0045cb1b  fstp       dword ptr [esp + 0x1c]
0045cb1f  fld        dword ptr [eax + 0x40]
0045cb22  fmul       dword ptr [esp + 0x14]
0045cb26  fstp       dword ptr [esp + 0x14]
0045cb2a  fld        dword ptr [eax + 0x44]
0045cb2d  fmul       dword ptr [esp + 0x10]
0045cb31  fstp       dword ptr [esp + 0x10]
0045cb35  fld        dword ptr [eax + 0x2c]
0045cb38  fadd       dword ptr [esp + 0xc]
0045cb3c  fstp       dword ptr [esp + 0xc]

// block 0045cb40
0045cb40  mov        edi, dword ptr [ebp + 8]
0045cb43  push       ebx
0045cb44  mov        eax, edi
0045cb46  call       0x459a60

// block 0045cb4b
0045cb4b  mov        eax, dword ptr [ebx + 0x47c]
0045cb51  shr        eax, 0x17
0045cb54  and        eax, 0x1f
0045cb57  add        eax, -0xe
0045cb5a  cmp        eax, 6
0045cb5d  ja         0x45cd74

// block 0045cb63
0045cb63  jmp        dword ptr [eax*4 + 0x45cdd4]

// block 0045cb6a
0045cb6a  fld        dword ptr [esp + 0xc]
0045cb6e  mov        edx, dword ptr [ebx + 0x3bc]
0045cb74  sub        esp, 0x14
0045cb77  fstp       dword ptr [esp + 0x10]
0045cb7b  fld        dword ptr [esp + 0x24]
0045cb7f  fstp       dword ptr [esp + 0xc]
0045cb83  fld        dword ptr [esp + 0x28]
0045cb87  fstp       dword ptr [esp + 8]
0045cb8b  fld        dword ptr [esp + 0x30]
0045cb8f  fstp       dword ptr [esp + 4]
0045cb93  fld        dword ptr [esp + 0x2c]
0045cb97  fstp       dword ptr [esp]
0045cb9a  call       0x45ce60

// block 0045cb9f
0045cb9f  xor        eax, eax
0045cba1  pop        edi
0045cba2  pop        esi
0045cba3  pop        ebx
0045cba4  mov        esp, ebp
0045cba6  pop        ebp
0045cba7  ret        4

// block 0045cbaa
0045cbaa  fld        dword ptr [esp + 0xc]
0045cbae  mov        edx, dword ptr [ebx + 0x3c0]
0045cbb4  mov        eax, dword ptr [ebx + 0x3bc]
0045cbba  sub        esp, 0x14
0045cbbd  fstp       dword ptr [esp + 0x10]
0045cbc1  fld        dword ptr [esp + 0x24]
0045cbc5  fstp       dword ptr [esp + 0xc]
0045cbc9  fld        dword ptr [esp + 0x28]
0045cbcd  fstp       dword ptr [esp + 8]
0045cbd1  fld        dword ptr [esp + 0x30]
0045cbd5  fstp       dword ptr [esp + 4]
0045cbd9  fld        dword ptr [esp + 0x2c]
0045cbdd  fstp       dword ptr [esp]
0045cbe0  call       0x45d0a0

// block 0045cbe5
0045cbe5  xor        eax, eax
0045cbe7  pop        edi
0045cbe8  pop        esi
0045cbe9  pop        ebx
0045cbea  mov        esp, ebp
0045cbec  pop        ebp
0045cbed  ret        4

// block 0045cbf0
0045cbf0  fld        dword ptr [esp + 0xc]
0045cbf4  mov        esi, dword ptr [ebx + 0x3bc]
0045cbfa  sub        esp, 0x14
0045cbfd  fstp       dword ptr [esp + 0x10]
0045cc01  mov        eax, edi
0045cc03  fld        dword ptr [esp + 0x24]
0045cc07  fstp       dword ptr [esp + 0xc]
0045cc0b  fld        dword ptr [esp + 0x28]
0045cc0f  fstp       dword ptr [esp + 8]
0045cc13  fld        dword ptr [esp + 0x30]
0045cc17  fstp       dword ptr [esp + 4]
0045cc1b  fld        dword ptr [esp + 0x2c]
0045cc1f  fstp       dword ptr [esp]
0045cc22  call       0x45d2e0

// block 0045cc27
0045cc27  xor        eax, eax
0045cc29  pop        edi
0045cc2a  pop        esi
0045cc2b  pop        ebx
0045cc2c  mov        esp, ebp
0045cc2e  pop        ebp
0045cc2f  ret        4

// block 0045cc32
0045cc32  fld        dword ptr [esp + 0xc]
0045cc36  mov        esi, dword ptr [ebx + 0x3c0]
0045cc3c  mov        ebx, dword ptr [ebx + 0x3bc]
0045cc42  sub        esp, 0x14
0045cc45  fstp       dword ptr [esp + 0x10]
0045cc49  mov        eax, edi
0045cc4b  fld        dword ptr [esp + 0x24]
0045cc4f  fstp       dword ptr [esp + 0xc]
0045cc53  fld        dword ptr [esp + 0x28]
0045cc57  fstp       dword ptr [esp + 8]
0045cc5b  fld        dword ptr [esp + 0x30]
0045cc5f  fstp       dword ptr [esp + 4]
0045cc63  fld        dword ptr [esp + 0x2c]
0045cc67  fstp       dword ptr [esp]
0045cc6a  call       0x45d380

// block 0045cc6f
0045cc6f  xor        eax, eax
0045cc71  pop        edi
0045cc72  pop        esi
0045cc73  pop        ebx
0045cc74  mov        esp, ebp
0045cc76  pop        ebp
0045cc77  ret        4

// block 0045cc7a
0045cc7a  fld        dword ptr [ebx + 0x2c]
0045cc7d  lea        eax, [esp + 0x18]
0045cc81  fstp       dword ptr [esp + 0x14]
0045cc85  mov        ecx, ebx
0045cc87  fld        dword ptr [ebx + 0x58]
0045cc8a  fmul       dword ptr [ebx + 0x40]
0045cc8d  fstp       dword ptr [esp + 0x10]
0045cc91  call       0x45d8c0

// block 0045cc96
0045cc96  mov        ecx, dword ptr [ebx + 0x3c]
0045cc99  test       ecx, ecx
0045cc9b  je         0x45ccd7

// block 0045cc9d
0045cc9d  lea        eax, [esp + 0x24]
0045cca1  call       0x45d8c0

// block 0045cca6
0045cca6  fld        dword ptr [esp + 0x24]
0045ccaa  fadd       dword ptr [esp + 0x18]
0045ccae  mov        eax, dword ptr [ebx + 0x3c]
0045ccb1  fstp       dword ptr [esp + 0x18]
0045ccb5  fld        dword ptr [esp + 0x28]
0045ccb9  fadd       dword ptr [esp + 0x1c]
0045ccbd  fstp       dword ptr [esp + 0x1c]
0045ccc1  fld        dword ptr [eax + 0x40]
0045ccc4  fmul       dword ptr [esp + 0x10]
0045ccc8  fstp       dword ptr [esp + 0x10]
0045cccc  fld        dword ptr [eax + 0x2c]
0045cccf  fadd       dword ptr [esp + 0x14]
0045ccd3  fstp       dword ptr [esp + 0x14]

// block 0045ccd7
0045ccd7  mov        edi, dword ptr [ebp + 8]
0045ccda  push       ebx
0045ccdb  mov        eax, edi
0045ccdd  call       0x459a60

// block 0045cce2
0045cce2  mov        eax, dword ptr [ebx + 0x47c]
0045cce8  shr        eax, 0x17
0045cceb  and        eax, 0x1f
0045ccee  sub        eax, 0xf
0045ccf1  je         0x45cd39

// block 0045ccf3
0045ccf3  sub        eax, 1
0045ccf6  jne        0x45cd74

// block 0045ccf8
0045ccf8  mov        ecx, dword ptr [ebx + 0x3bc]
0045ccfe  fld        dword ptr [esp + 0x14]
0045cd02  mov        edx, dword ptr [ebx + 0x3fc]
0045cd08  push       ecx
0045cd09  push       edx
0045cd0a  sub        esp, 0x10
0045cd0d  fstp       dword ptr [esp + 0xc]
0045cd11  fld        dword ptr [esp + 0x28]
0045cd15  fstp       dword ptr [esp + 8]
0045cd19  fld        dword ptr [esp + 0x34]
0045cd1d  fstp       dword ptr [esp + 4]
0045cd21  fld        dword ptr [esp + 0x30]
0045cd25  fstp       dword ptr [esp]
0045cd28  push       edi
0045cd29  call       0x45d710

// block 0045cd2e
0045cd2e  xor        eax, eax
0045cd30  pop        edi
0045cd31  pop        esi
0045cd32  pop        ebx
0045cd33  mov        esp, ebp
0045cd35  pop        ebp
0045cd36  ret        4

// block 0045cd39
0045cd39  mov        eax, dword ptr [ebx + 0x3c0]
0045cd3f  fld        dword ptr [esp + 0x14]
0045cd43  mov        ecx, dword ptr [ebx + 0x3fc]
0045cd49  push       eax
0045cd4a  mov        eax, dword ptr [ebx + 0x3bc]
0045cd50  push       ecx
0045cd51  sub        esp, 0x10
0045cd54  fstp       dword ptr [esp + 0xc]
0045cd58  fld        dword ptr [esp + 0x28]
0045cd5c  fstp       dword ptr [esp + 8]
0045cd60  fld        dword ptr [esp + 0x34]
0045cd64  fstp       dword ptr [esp + 4]
0045cd68  fld        dword ptr [esp + 0x30]
0045cd6c  fstp       dword ptr [esp]
0045cd6f  call       0x45d430

// block 0045cd74
0045cd74  pop        edi
0045cd75  pop        esi
0045cd76  xor        eax, eax
0045cd78  pop        ebx
0045cd79  mov        esp, ebp
0045cd7b  pop        ebp
0045cd7c  ret        4
