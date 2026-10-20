
// block 00469750
00469750  sub        esp, 0x14
00469753  push       ebx
00469754  push       ebp
00469755  push       esi
00469756  push       edi
00469757  push       0x418
0046975c  mov        esi, edx
0046975e  mov        ebp, ecx
00469760  call       0x46d04a

// block 00469765
00469765  fldz       
00469767  mov        ebx, eax
00469769  mov        dword ptr [ebp + 0x478], ebx
0046976f  mov        ecx, dword ptr [esi + 8]
00469772  mov        eax, dword ptr [esi + 4]
00469775  mov        edi, dword ptr [esi]
00469777  push       0x418
0046977c  push       0
0046977e  mov        dword ptr [esp + 0x2c], ecx
00469782  fstp       dword ptr [esp + 0x2c]
00469786  push       ebx
00469787  mov        dword ptr [esp + 0x2c], eax
0046978b  call       0x477420

// block 00469790
00469790  fld        dword ptr [0x4a3eb8]
00469796  mov        edx, dword ptr [esi + 0xc]
00469799  fstp       dword ptr [esp + 0x2c]
0046979d  mov        dword ptr [ebx + 0x408], edx
004697a3  mov        eax, dword ptr [esi + 0x10]
004697a6  mov        dword ptr [ebx + 0x40c], eax
004697ac  mov        ecx, dword ptr [esi + 0x14]
004697af  mov        eax, dword ptr [esp + 0x2c]
004697b3  mov        dword ptr [ebx + 0x410], ecx
004697b9  mov        ecx, dword ptr [esp + 0x30]
004697bd  mov        dword ptr [ebp + 0x430], edi
004697c3  mov        dword ptr [ebp + 0x434], eax
004697c9  mov        dword ptr [ebp + 0x438], ecx
004697cf  mov        dword ptr [ebx + 0x330], edi
004697d5  mov        dword ptr [ebx + 0x334], eax
004697db  mov        dword ptr [ebx + 0x338], ecx
004697e1  fld        dword ptr [esi + 8]
004697e4  add        esp, 0xc
004697e7  fstp       dword ptr [esp]
004697ea  call       0x4646e0

// block 004697ef
004697ef  push       ecx
004697f0  fstp       dword ptr [esp]
004697f3  call       0x4646e0

// block 004697f8
004697f8  mov        edx, dword ptr [esp + 0x1c]
004697fc  fstp       dword ptr [ebx + 0x34c]
00469802  fld        dword ptr [0x4a3d78]
00469808  mov        eax, dword ptr [esp + 0x20]
0046980c  lea        esi, [ebx + 0x2a0]
00469812  fstp       dword ptr [ebx + 0x348]
00469818  mov        dword ptr [esi], edi
0046981a  mov        dword ptr [esi + 4], edx
0046981d  mov        dword ptr [esi + 8], eax
00469820  mov        eax, dword ptr [0x4b4514]
00469825  mov        ecx, 0x21
0046982a  lea        edi, [esi + 0xc]

// block 0046982d
0046982d  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0046982f
0046982f  test       eax, eax
00469831  je         0x469884

// block 00469833
00469833  fld        dword ptr [0x4a4134]
00469839  push       0xa
0046983b  push       0x3e7
00469840  sub        esp, 8
00469843  fstp       dword ptr [esp + 4]
00469847  lea        ecx, [ebx + 0x330]
0046984d  fld        dword ptr [0x4a4130]
00469853  fstp       dword ptr [esp]
00469856  push       ecx
00469857  call       0x4390f0

// block 0046985c
0046985c  fld        dword ptr [0x4a3db8]
00469862  mov        dword ptr [ebx + 0x414], eax
00469868  and        dword ptr [eax + 0x70], 0xfffffffd
0046986c  mov        ecx, dword ptr [ebx + 0x414]
00469872  fstp       dword ptr [ecx + 0x10]
00469875  mov        edx, dword ptr [ebx + 0x414]
0046987b  fld        dword ptr [0x4a3e00]
00469881  fstp       dword ptr [edx + 0x14]

// block 00469884
00469884  fldz       
00469886  mov        ecx, ebx
00469888  fst        dword ptr [esp + 0x10]
0046988c  mov        dword ptr [ebp + 0x3fc], 0xc
00469896  fld1       
00469898  mov        dword ptr [esp + 0x14], 0x180
004698a0  fld        qword ptr [0x4a3d48]
004698a6  mov        edi, 0x1fe
004698ab  lea        esi, [ebx + 0x2ac]
004698b1  jmp        0x4698b5

// block 004698b3
004698b3  fxch       st(1)

// block 004698b5
004698b5  mov        dword ptr [ecx + 0x10], 0xffffffff
004698bc  fxch       st(1)
004698be  fst        dword ptr [ecx + 0xc]
004698c1  lea        edx, [edi - 0x1fe]
004698c7  mov        eax, 0x2aaaaaab
004698cc  imul       edx
004698ce  sar        edx, 1
004698d0  mov        eax, edx
004698d2  shr        eax, 0x1f
004698d5  add        eax, edx
004698d7  or         dl, 0xff
004698da  sub        dl, al
004698dc  mov        byte ptr [ecx + 0x13], dl
004698df  add        ecx, 0x1c
004698e2  fld        dword ptr [ebp + 0x60]
004698e5  add        ecx, 0x1c
004698e8  fadd       dword ptr [ebp + 0x7c]
004698eb  fstp       dword ptr [ecx - 0x24]
004698ee  fld        dword ptr [esp + 0x10]
004698f2  fst        dword ptr [ecx - 0x20]
004698f5  mov        eax, dword ptr [esi - 0xc]
004698f8  mov        dword ptr [ecx - 0x38], eax
004698fb  fxch       st(1)
004698fd  mov        edx, dword ptr [esi - 8]
00469900  mov        dword ptr [ecx - 0x34], edx
00469903  mov        eax, dword ptr [esi - 4]
00469906  mov        dword ptr [ecx - 0x30], eax
00469909  fst        dword ptr [ecx - 0x10]
0046990c  mov        edx, dword ptr [esp + 0x14]
00469910  add        edx, 0xfffffe80
00469916  mov        eax, 0x2aaaaaab
0046991b  imul       edx
0046991d  mov        dword ptr [ecx - 0xc], 0xffffffff
00469924  sar        edx, 1
00469926  mov        eax, edx
00469928  shr        eax, 0x1f
0046992b  add        eax, edx
0046992d  mov        dl, 0xc0
0046992f  sub        dl, al
00469931  mov        byte ptr [ecx - 9], dl
00469934  fld        dword ptr [ebp + 0x64]
00469937  fadd       dword ptr [ebp + 0x84]
0046993d  fstp       dword ptr [ecx - 8]
00469940  fxch       st(1)
00469942  fst        dword ptr [ecx - 4]
00469945  mov        eax, dword ptr [esi - 0xc]
00469948  mov        dword ptr [ecx - 0x1c], eax
0046994b  mov        edx, dword ptr [esi - 8]
0046994e  mov        dword ptr [ecx - 0x18], edx
00469951  mov        eax, dword ptr [esi - 4]
00469954  mov        dword ptr [ecx - 0x14], eax
00469957  fld        dword ptr [ebx + 0x348]
0046995d  fdivr      st(3)
0046995f  lea        edx, [edi - 0xff]
00469965  mov        eax, 0x2aaaaaab
0046996a  imul       edx
0046996c  faddp      st(1)
0046996e  fstp       dword ptr [esp + 0x10]
00469972  fst        dword ptr [ecx + 0xc]
00469975  sar        edx, 1
00469977  mov        dword ptr [ecx + 0x10], 0xffffffff
0046997e  mov        eax, edx
00469980  shr        eax, 0x1f
00469983  add        eax, edx
00469985  or         dl, 0xff
00469988  sub        dl, al
0046998a  mov        byte ptr [ecx + 0x13], dl
0046998d  add        ecx, 0x1c
00469990  fld        dword ptr [ebp + 0x60]
00469993  fadd       dword ptr [ebp + 0x7c]
00469996  fstp       dword ptr [ecx - 8]
00469999  fld        dword ptr [esp + 0x10]
0046999d  fst        dword ptr [ecx - 4]
004699a0  mov        eax, dword ptr [esi]
004699a2  mov        dword ptr [ecx - 0x1c], eax
004699a5  fxch       st(1)
004699a7  mov        edx, dword ptr [esi + 4]
004699aa  mov        dword ptr [ecx - 0x18], edx
004699ad  mov        eax, dword ptr [esi + 8]
004699b0  mov        dword ptr [ecx - 0x14], eax
004699b3  fst        dword ptr [ecx + 0xc]
004699b6  mov        edx, dword ptr [esp + 0x14]
004699ba  mov        dword ptr [ecx + 0x10], 0xffffffff
004699c1  add        edx, 0xffffff40
004699c7  mov        eax, 0x2aaaaaab
004699cc  imul       edx
004699ce  sar        edx, 1
004699d0  mov        eax, edx
004699d2  shr        eax, 0x1f
004699d5  add        eax, edx
004699d7  mov        dl, 0xc0
004699d9  sub        dl, al
004699db  mov        byte ptr [ecx + 0x13], dl
004699de  add        ecx, 0x1c
004699e1  fld        dword ptr [ebp + 0x64]
004699e4  add        ecx, 0x1c
004699e7  fadd       dword ptr [ebp + 0x84]
004699ed  fstp       dword ptr [ecx - 0x24]

// block 004699f0
004699f0  fxch       st(1)

// block 004699f2
004699f2  fst        dword ptr [ecx - 0x20]
004699f5  mov        eax, dword ptr [esi]
004699f7  mov        dword ptr [ecx - 0x38], eax
004699fa  mov        edx, dword ptr [esi + 4]
004699fd  mov        dword ptr [ecx - 0x34], edx
00469a00  mov        eax, dword ptr [esi + 8]
00469a03  mov        dword ptr [ecx - 0x30], eax
00469a06  fld        dword ptr [ebx + 0x348]
00469a0c  fdivr      st(3)
00469a0e  mov        eax, 0x2aaaaaab
00469a13  imul       edi
00469a15  faddp      st(1)
00469a17  fstp       dword ptr [esp + 0x10]
00469a1b  fst        dword ptr [ecx - 0x10]
00469a1e  mov        dword ptr [ecx - 0xc], 0xffffffff
00469a25  sar        edx, 1
00469a27  mov        eax, edx
00469a29  shr        eax, 0x1f
00469a2c  add        eax, edx
00469a2e  or         dl, 0xff
00469a31  sub        dl, al
00469a33  mov        byte ptr [ecx - 9], dl
00469a36  fld        dword ptr [ebp + 0x60]
00469a39  fadd       dword ptr [ebp + 0x7c]
00469a3c  fstp       dword ptr [ecx - 8]
00469a3f  fld        dword ptr [esp + 0x10]
00469a43  fst        dword ptr [ecx - 4]
00469a46  mov        eax, dword ptr [esi + 0xc]
00469a49  mov        dword ptr [ecx - 0x1c], eax
00469a4c  fxch       st(1)
00469a4e  mov        edx, dword ptr [esi + 0x10]
00469a51  mov        dword ptr [ecx - 0x18], edx
00469a54  mov        eax, dword ptr [esi + 0x14]
00469a57  mov        dword ptr [ecx - 0x14], eax
00469a5a  fst        dword ptr [ecx + 0xc]
00469a5d  mov        eax, 0x2aaaaaab
00469a62  imul       dword ptr [esp + 0x14]
00469a66  sar        edx, 1
00469a68  mov        dword ptr [ecx + 0x10], 0xffffffff
00469a6f  mov        eax, edx
00469a71  shr        eax, 0x1f
00469a74  add        eax, edx
00469a76  mov        dl, 0xc0
00469a78  sub        dl, al
00469a7a  mov        byte ptr [ecx + 0x13], dl
00469a7d  add        ecx, 0x1c
00469a80  fld        dword ptr [ebp + 0x64]
00469a83  fadd       dword ptr [ebp + 0x84]
00469a89  fstp       dword ptr [ecx - 8]
00469a8c  fxch       st(1)
00469a8e  fst        dword ptr [ecx - 4]
00469a91  mov        eax, dword ptr [esi + 0xc]
00469a94  mov        dword ptr [ecx - 0x1c], eax
00469a97  mov        edx, dword ptr [esi + 0x10]
00469a9a  mov        dword ptr [ecx - 0x18], edx
00469a9d  mov        eax, dword ptr [esi + 0x14]
00469aa0  mov        dword ptr [ecx - 0x14], eax
00469aa3  fld        dword ptr [ebx + 0x348]
00469aa9  fdivr      st(3)
00469aab  faddp      st(1)
00469aad  fstp       dword ptr [esp + 0x10]
00469ab1  fst        dword ptr [ecx + 0xc]
00469ab4  mov        dword ptr [ecx + 0x10], 0xffffffff
00469abb  lea        edx, [edi + 0xff]
00469ac1  mov        eax, 0x2aaaaaab
00469ac6  imul       edx
00469ac8  sar        edx, 1
00469aca  mov        eax, edx
00469acc  shr        eax, 0x1f
00469acf  add        eax, edx
00469ad1  or         dl, 0xff
00469ad4  sub        dl, al
00469ad6  mov        byte ptr [ecx + 0x13], dl
00469ad9  add        ecx, 0x1c
00469adc  fld        dword ptr [ebp + 0x60]
00469adf  add        ecx, 0x1c
00469ae2  fadd       dword ptr [ebp + 0x7c]
00469ae5  add        ecx, 0x1c
00469ae8  fstp       dword ptr [ecx - 0x40]
00469aeb  fld        dword ptr [esp + 0x10]
00469aef  fst        dword ptr [ecx - 0x3c]
00469af2  mov        eax, dword ptr [esi + 0x18]
00469af5  mov        dword ptr [ecx - 0x54], eax
00469af8  fxch       st(1)
00469afa  mov        edx, dword ptr [esi + 0x1c]
00469afd  mov        dword ptr [ecx - 0x50], edx
00469b00  mov        eax, dword ptr [esi + 0x20]
00469b03  mov        dword ptr [ecx - 0x4c], eax
00469b06  fst        dword ptr [ecx - 0x2c]
00469b09  mov        edx, dword ptr [esp + 0x14]
00469b0d  add        edx, 0xc0
00469b13  mov        dword ptr [ecx - 0x28], 0xffffffff
00469b1a  mov        eax, 0x2aaaaaab
00469b1f  imul       edx
00469b21  sar        edx, 1
00469b23  mov        eax, edx
00469b25  shr        eax, 0x1f
00469b28  add        eax, edx

// block 00469b2a
00469b2a  mov        dl, 0xc0

// block 00469b2c
00469b2c  sub        dl, al
00469b2e  mov        byte ptr [ecx - 0x25], dl
00469b31  fld        dword ptr [ebp + 0x64]
00469b34  fadd       dword ptr [ebp + 0x84]
00469b3a  fstp       dword ptr [ecx - 0x24]
00469b3d  fxch       st(1)
00469b3f  fst        dword ptr [ecx - 0x20]
00469b42  mov        eax, dword ptr [esi + 0x18]
00469b45  mov        dword ptr [ecx - 0x38], eax
00469b48  mov        edx, dword ptr [esi + 0x1c]
00469b4b  mov        dword ptr [ecx - 0x34], edx
00469b4e  mov        eax, dword ptr [esi + 0x20]
00469b51  mov        dword ptr [ecx - 0x30], eax
00469b54  fld        dword ptr [ebx + 0x348]
00469b5a  fdivr      st(3)
00469b5c  lea        edx, [edi + 0x1fe]
00469b62  mov        eax, 0x2aaaaaab
00469b67  imul       edx
00469b69  faddp      st(1)
00469b6b  fstp       dword ptr [esp + 0x10]
00469b6f  fst        dword ptr [ecx - 0x10]
00469b72  sar        edx, 1
00469b74  mov        dword ptr [ecx - 0xc], 0xffffffff
00469b7b  mov        eax, edx
00469b7d  shr        eax, 0x1f
00469b80  add        eax, edx
00469b82  or         dl, 0xff
00469b85  sub        dl, al
00469b87  mov        byte ptr [ecx - 9], dl
00469b8a  fld        dword ptr [ebp + 0x60]
00469b8d  fadd       dword ptr [ebp + 0x7c]
00469b90  fstp       dword ptr [ecx - 8]
00469b93  fld        dword ptr [esp + 0x10]
00469b97  fst        dword ptr [ecx - 4]
00469b9a  mov        eax, dword ptr [esi + 0x24]
00469b9d  mov        dword ptr [ecx - 0x1c], eax
00469ba0  fxch       st(1)
00469ba2  mov        edx, dword ptr [esi + 0x28]
00469ba5  mov        dword ptr [ecx - 0x18], edx
00469ba8  mov        eax, dword ptr [esi + 0x2c]
00469bab  mov        edx, dword ptr [esp + 0x14]
00469baf  mov        dword ptr [ecx - 0x14], eax
00469bb2  fst        dword ptr [ecx + 0xc]
00469bb5  mov        dword ptr [ecx + 0x10], 0xffffffff
00469bbc  add        edx, 0x180
00469bc2  mov        eax, 0x2aaaaaab
00469bc7  imul       edx
00469bc9  sar        edx, 1
00469bcb  mov        eax, edx
00469bcd  shr        eax, 0x1f
00469bd0  add        eax, edx
00469bd2  mov        dl, 0xc0
00469bd4  sub        dl, al
00469bd6  mov        byte ptr [ecx + 0x13], dl
00469bd9  add        ecx, 0x1c
00469bdc  fld        dword ptr [ebp + 0x64]
00469bdf  add        ecx, 0x1c
00469be2  fadd       dword ptr [ebp + 0x84]
00469be8  add        ecx, 0x1c
00469beb  add        esi, 0x48
00469bee  fstp       dword ptr [ecx - 0x40]
00469bf1  fxch       st(1)
00469bf3  fst        dword ptr [ecx - 0x3c]
00469bf6  mov        eax, dword ptr [esi - 0x24]
00469bf9  mov        dword ptr [ecx - 0x54], eax
00469bfc  mov        edx, dword ptr [esi - 0x20]
00469bff  mov        dword ptr [ecx - 0x50], edx
00469c02  mov        eax, dword ptr [esi - 0x1c]
00469c05  mov        dword ptr [ecx - 0x4c], eax
00469c08  fld        dword ptr [ebx + 0x348]
00469c0e  fdivr      st(3)
00469c10  lea        edx, [edi + 0x2fd]
00469c16  mov        eax, 0x2aaaaaab
00469c1b  imul       edx
00469c1d  faddp      st(1)
00469c1f  fstp       dword ptr [esp + 0x10]
00469c23  fst        dword ptr [ecx - 0x2c]
00469c26  mov        dword ptr [ecx - 0x28], 0xffffffff
00469c2d  sar        edx, 1
00469c2f  mov        eax, edx
00469c31  shr        eax, 0x1f
00469c34  add        eax, edx
00469c36  or         dl, 0xff
00469c39  sub        dl, al
00469c3b  mov        byte ptr [ecx - 0x25], dl
00469c3e  fld        dword ptr [ebp + 0x60]
00469c41  fadd       dword ptr [ebp + 0x7c]
00469c44  fstp       dword ptr [ecx - 0x24]
00469c47  fld        dword ptr [esp + 0x10]
00469c4b  fst        dword ptr [ecx - 0x20]
00469c4e  mov        eax, dword ptr [esi - 0x18]
00469c51  mov        dword ptr [ecx - 0x38], eax
00469c54  fxch       st(1)
00469c56  mov        edx, dword ptr [esi - 0x14]
00469c59  mov        dword ptr [ecx - 0x34], edx
00469c5c  mov        eax, dword ptr [esi - 0x10]
00469c5f  mov        edx, dword ptr [esp + 0x14]
00469c63  mov        dword ptr [ecx - 0x30], eax

// block 00469c66
00469c66  fst        dword ptr [ecx - 0x10]

// block 00469c69
00469c69  add        edx, 0x240
00469c6f  mov        eax, 0x2aaaaaab
00469c74  imul       edx
00469c76  sar        edx, 1
00469c78  mov        dword ptr [ecx - 0xc], 0xffffffff
00469c7f  mov        eax, edx
00469c81  shr        eax, 0x1f
00469c84  add        eax, edx
00469c86  mov        dl, 0xc0
00469c88  sub        dl, al
00469c8a  mov        byte ptr [ecx - 9], dl
00469c8d  add        edi, 0x5fa
00469c93  fld        dword ptr [ebp + 0x64]
00469c96  fadd       dword ptr [ebp + 0x84]
00469c9c  fstp       dword ptr [ecx - 8]
00469c9f  fxch       st(1)
00469ca1  fst        dword ptr [ecx - 4]
00469ca4  mov        eax, dword ptr [esi - 0x18]
00469ca7  mov        dword ptr [ecx - 0x1c], eax
00469caa  mov        edx, dword ptr [esi - 0x14]
00469cad  mov        dword ptr [ecx - 0x18], edx
00469cb0  mov        eax, dword ptr [esi - 0x10]
00469cb3  mov        dword ptr [ecx - 0x14], eax
00469cb6  fld        dword ptr [ebx + 0x348]
00469cbc  fdivr      st(3)
00469cbe  faddp      st(1)
00469cc0  fstp       dword ptr [esp + 0x10]
00469cc4  add        dword ptr [esp + 0x14], 0x480
00469ccc  cmp        edi, 0xdf2
00469cd2  jl         0x4698b3

// block 00469cd8
00469cd8  mov        eax, dword ptr [ebx + 0x404]
00469cde  fstp       st(0)
00469ce0  fstp       st(0)
00469ce2  test       al, 1
00469ce4  jne        0x469d13

// block 00469ce6
00469ce6  or         eax, 1
00469ce9  fst        dword ptr [ebx + 0x3fc]
00469cef  mov        dword ptr [ebx + 0x3f8], 0
00469cf9  mov        dword ptr [ebx + 0x3f4], 0xfff0bdc1
00469d03  mov        dword ptr [ebx + 0x400], 0x4b2ed0
00469d0d  mov        dword ptr [ebx + 0x404], eax

// block 00469d13
00469d13  pop        edi
00469d14  fstp       dword ptr [ebx + 0x3fc]
00469d1a  pop        esi
00469d1b  pop        ebp
00469d1c  mov        dword ptr [ebx + 0x3f8], 0
00469d26  mov        dword ptr [ebx + 0x3f4], 0xffffffff
00469d30  xor        eax, eax
00469d32  pop        ebx
00469d33  add        esp, 0x14
00469d36  ret        
