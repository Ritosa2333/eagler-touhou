
// block 0041f900
0041f900  sub        esp, 0xc
0041f903  push       ebx
0041f904  push       ebp
0041f905  push       edi
0041f906  mov        eax, 0xfffffffe
0041f90b  and        dword ptr [esi + 0x14], eax
0041f90e  and        dword ptr [esi + 0x28], eax
0041f911  and        dword ptr [esi + 0x3c], eax
0041f914  push       0xac
0041f919  xor        edi, edi
0041f91b  push       edi
0041f91c  push       esi
0041f91d  call       0x477420

// block 0041f922
0041f922  fldz       
0041f924  mov        eax, dword ptr [esi + 0x14]
0041f927  add        esp, 0xc
0041f92a  mov        edx, 0xfff0bdc1
0041f92f  mov        ecx, 0x4b2ed0
0041f934  test       al, 1
0041f936  jne        0x41f94a

// block 0041f938
0041f938  or         eax, 1
0041f93b  fst        dword ptr [esi + 0xc]
0041f93e  mov        dword ptr [esi + 8], edi
0041f941  mov        dword ptr [esi + 4], edx
0041f944  mov        dword ptr [esi + 0x10], ecx
0041f947  mov        dword ptr [esi + 0x14], eax

// block 0041f94a
0041f94a  or         ebx, 0xffffffff
0041f94d  fst        dword ptr [esi + 0xc]
0041f950  mov        dword ptr [esi + 8], edi
0041f953  mov        dword ptr [esi + 4], ebx
0041f956  mov        eax, dword ptr [esi + 0x28]
0041f959  test       al, 1
0041f95b  jne        0x41f96f

// block 0041f95d
0041f95d  or         eax, 1

// block 0041f96f
0041f96f  fst        dword ptr [esi + 0x20]
0041f972  mov        dword ptr [esi + 0x1c], edi
0041f975  mov        dword ptr [esi + 0x18], ebx
0041f978  mov        dword ptr [esi + 0x60], edi
0041f97b  mov        eax, dword ptr [esi + 0x3c]
0041f97e  test       al, 1
0041f980  jne        0x41f994

// block 0041f982
0041f982  or         eax, 1
0041f985  fst        dword ptr [esi + 0x34]
0041f988  mov        dword ptr [esi + 0x30], edi
0041f98b  mov        dword ptr [esi + 0x2c], edx
0041f98e  mov        dword ptr [esi + 0x38], ecx
0041f991  mov        dword ptr [esi + 0x3c], eax

// block 0041f994
0041f994  mov        eax, dword ptr [esp + 0x1c]
0041f998  fstp       dword ptr [esi + 0x34]
0041f99b  push       edi
0041f99c  mov        dword ptr [esi + 0x30], edi
0041f99f  mov        dword ptr [esi + 0x2c], ebx
0041f9a2  push       edi
0041f9a3  mov        dword ptr [esi + 0x64], eax
0041f9a6  mov        edx, dword ptr [0x4cee70]
0041f9ac  lea        ecx, [esp + 0x24]
0041f9b0  push       ecx
0041f9b1  push       edx
0041f9b2  mov        eax, 0x17
0041f9b7  xor        ecx, ecx
0041f9b9  call       0x4615a0

// block 0041f9be
0041f9be  mov        eax, dword ptr [esp + 0x1c]
0041f9c2  push       edi
0041f9c3  push       1
0041f9c5  mov        dword ptr [esi + 0x4c], eax
0041f9c8  mov        edx, dword ptr [0x4cee70]
0041f9ce  lea        ecx, [esp + 0x24]
0041f9d2  push       ecx
0041f9d3  push       edx
0041f9d4  mov        eax, 0x17
0041f9d9  xor        ecx, ecx
0041f9db  call       0x4615a0

// block 0041f9e0
0041f9e0  mov        eax, dword ptr [esp + 0x1c]
0041f9e4  mov        ebp, dword ptr [0x4ce8cc]
0041f9ea  mov        dword ptr [esi + 0x50], eax
0041f9ed  mov        ecx, dword ptr [esi + 0x4c]
0041f9f0  push       ecx
0041f9f1  mov        edx, ebp
0041f9f3  call       0x461920

// block 0041f9f8
0041f9f8  cmp        eax, edi
0041f9fa  jne        0x41f9ff

// block 0041f9fc
0041f9fc  mov        dword ptr [esi + 0x4c], edi

// block 0041f9ff
0041f9ff  mov        bl, 0x10
0041fa01  mov        byte ptr [eax + 0x49c], bl
0041fa07  mov        edx, dword ptr [esi + 0x4c]
0041fa0a  push       edx
0041fa0b  mov        edx, ebp
0041fa0d  call       0x461920

// block 0041fa12
0041fa12  cmp        eax, edi
0041fa14  jne        0x41fa19

// block 0041fa16
0041fa16  mov        dword ptr [esi + 0x4c], edi

// block 0041fa19
0041fa19  mov        byte ptr [eax + 0x49d], bl
0041fa1f  mov        eax, dword ptr [esi + 0x50]
0041fa22  push       eax
0041fa23  mov        edx, ebp
0041fa25  call       0x461920

// block 0041fa2a
0041fa2a  cmp        eax, edi
0041fa2c  jne        0x41fa31

// block 0041fa2e
0041fa2e  mov        dword ptr [esi + 0x50], edi

// block 0041fa31
0041fa31  mov        byte ptr [eax + 0x49c], bl
0041fa37  mov        ecx, dword ptr [esi + 0x50]
0041fa3a  push       ecx
0041fa3b  mov        edx, ebp
0041fa3d  call       0x461920

// block 0041fa42
0041fa42  cmp        eax, edi
0041fa44  jne        0x41fa49

// block 0041fa46
0041fa46  mov        dword ptr [esi + 0x50], edi

// block 0041fa49
0041fa49  push       edi
0041fa4a  push       2
0041fa4c  mov        byte ptr [eax + 0x49d], bl
0041fa52  mov        eax, dword ptr [0x4cee70]
0041fa57  lea        edx, [esp + 0x24]
0041fa5b  push       edx
0041fa5c  push       eax
0041fa5d  mov        eax, 0x17
0041fa62  xor        ecx, ecx
0041fa64  call       0x4615a0

// block 0041fa69
0041fa69  mov        ecx, dword ptr [esp + 0x1c]
0041fa6d  push       edi
0041fa6e  push       3
0041fa70  mov        dword ptr [esi + 0x54], ecx
0041fa73  mov        eax, dword ptr [0x4cee70]
0041fa78  lea        edx, [esp + 0x24]
0041fa7c  push       edx
0041fa7d  push       eax
0041fa7e  mov        eax, 0x17
0041fa83  xor        ecx, ecx
0041fa85  call       0x4615a0

// block 0041fa8a
0041fa8a  mov        ecx, dword ptr [esp + 0x1c]
0041fa8e  mov        ebp, dword ptr [0x4ce8cc]
0041fa94  mov        dword ptr [esi + 0x58], ecx
0041fa97  mov        edx, dword ptr [esi + 0x54]
0041fa9a  push       edx
0041fa9b  mov        edx, ebp
0041fa9d  call       0x461920

// block 0041faa2
0041faa2  cmp        eax, edi
0041faa4  jne        0x41faa9

// block 0041faa6
0041faa6  mov        dword ptr [esi + 0x54], edi

// block 0041faa9
0041faa9  mov        byte ptr [eax + 0x49c], bl
0041faaf  mov        eax, dword ptr [esi + 0x54]
0041fab2  push       eax
0041fab3  mov        edx, ebp
0041fab5  call       0x461920

// block 0041faba
0041faba  cmp        eax, edi
0041fabc  jne        0x41fac1

// block 0041fabe
0041fabe  mov        dword ptr [esi + 0x54], edi

// block 0041fac1
0041fac1  mov        byte ptr [eax + 0x49d], bl
0041fac7  mov        ecx, dword ptr [esi + 0x58]
0041faca  push       ecx
0041facb  mov        edx, ebp
0041facd  call       0x461920

// block 0041fad2
0041fad2  cmp        eax, edi
0041fad4  jne        0x41fad9

// block 0041fad6
0041fad6  mov        dword ptr [esi + 0x58], edi

// block 0041fad9
0041fad9  mov        byte ptr [eax + 0x49c], bl
0041fadf  mov        edx, dword ptr [esi + 0x58]
0041fae2  push       edx
0041fae3  mov        edx, ebp
0041fae5  call       0x461920

// block 0041faea
0041faea  cmp        eax, edi
0041faec  jne        0x41faf1

// block 0041faee
0041faee  mov        dword ptr [esi + 0x58], edi

// block 0041faf1
0041faf1  fld        dword ptr [0x4a3e54]
0041faf7  mov        byte ptr [eax + 0x49d], bl
0041fafd  fst        dword ptr [esp + 0xc]
0041fb01  mov        eax, dword ptr [esp + 0xc]
0041fb05  fldz       
0041fb07  mov        dword ptr [esi + 0x68], eax
0041fb0a  fst        dword ptr [esp + 0x10]
0041fb0e  xor        ebx, ebx
0041fb10  mov        ecx, dword ptr [esp + 0x10]
0041fb14  fst        dword ptr [esp + 0x14]
0041fb18  mov        edx, dword ptr [esp + 0x14]
0041fb1c  fxch       st(1)
0041fb1e  fst        dword ptr [esp + 0xc]
0041fb22  mov        dword ptr [esi + 0x6c], ecx
0041fb25  mov        eax, dword ptr [esp + 0xc]
0041fb29  fxch       st(1)
0041fb2b  fst        dword ptr [esp + 0x10]
0041fb2f  mov        dword ptr [esi + 0x70], edx
0041fb32  mov        ecx, dword ptr [esp + 0x10]
0041fb36  fst        dword ptr [esp + 0x14]
0041fb3a  mov        edx, dword ptr [esp + 0x14]
0041fb3e  fxch       st(1)
0041fb40  fstp       dword ptr [esp + 0xc]
0041fb44  mov        dword ptr [esi + 0x74], eax
0041fb47  mov        eax, dword ptr [esp + 0xc]
0041fb4b  mov        dword ptr [esi + 0x78], ecx
0041fb4e  fst        dword ptr [esp + 0x10]
0041fb52  mov        ecx, dword ptr [esp + 0x10]
0041fb56  mov        dword ptr [esi + 0x7c], edx
0041fb59  fstp       dword ptr [esp + 0x14]
0041fb5d  mov        edx, dword ptr [esp + 0x14]
0041fb61  mov        dword ptr [esi + 0x80], eax
0041fb67  mov        dword ptr [esi + 0x84], ecx
0041fb6d  mov        dword ptr [esi + 0x94], edi
0041fb73  mov        dword ptr [esi + 0x98], edi
0041fb79  mov        dword ptr [esi + 0xa0], 0xf8f08f
0041fb83  mov        dword ptr [esi + 0xa4], 0x8088ff
0041fb8d  mov        dword ptr [esi + 0xa8], 0xd8d8d8
0041fb97  mov        dword ptr [esi + 0x9c], edi
0041fb9d  mov        dword ptr [esi + 0x88], edx
0041fba3  call       0x40d230

// block 0041fba8
0041fba8  mov        eax, dword ptr [0x4b44f4]
0041fbad  mov        ecx, dword ptr [eax + 0x18]
0041fbb0  cmp        ecx, edi
0041fbb2  je         0x41fbcc

// block 0041fbb4
0041fbb4  cmp        dword ptr [ecx + 0xc], 1
0041fbb8  mov        ebx, dword ptr [ecx + 8]
0041fbbb  je         0x41fbc6

// block 0041fbbd
0041fbbd  mov        edx, dword ptr [ecx]
0041fbbf  mov        eax, dword ptr [edx + 0x14]
0041fbc2  push       edi
0041fbc3  push       edi
0041fbc4  call       eax

// block 0041fbc6
0041fbc6  mov        ecx, ebx
0041fbc8  cmp        ebx, edi
0041fbca  jne        0x41fbb4

// block 0041fbcc
0041fbcc  call       0x414c60

// block 0041fbd1
0041fbd1  pop        edi
0041fbd2  pop        ebp
0041fbd3  mov        eax, esi
0041fbd5  pop        ebx
0041fbd6  add        esp, 0xc
0041fbd9  ret        4
