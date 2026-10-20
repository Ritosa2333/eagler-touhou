
// block 00429610
00429610  push       ebp
00429611  mov        ebp, esp
00429613  and        esp, 0xfffffff8
00429616  sub        esp, 0x20
00429619  push       esi
0042961a  push       edi
0042961b  mov        edi, ecx
0042961d  lea        ecx, [ecx]

// block 00429620
00429620  mov        eax, dword ptr [edi]
00429622  mov        edx, dword ptr [eax]
00429624  mov        ecx, edi
00429626  call       edx

// block 00429628
00429628  mov        eax, dword ptr [edi + 0x430]
0042962e  test       eax, eax
00429630  je         0x4297b3

// block 00429636
00429636  xor        esi, esi
00429638  test       al, 1
0042963a  je         0x429647

// block 0042963c
0042963c  mov        eax, dword ptr [edi]
0042963e  mov        edx, dword ptr [eax + 0x28]
00429641  mov        ecx, edi
00429643  call       edx

// block 00429645
00429645  mov        esi, eax

// block 00429647
00429647  test       byte ptr [edi + 0x430], 4
0042964e  je         0x42965b

// block 00429650
00429650  mov        eax, dword ptr [edi]
00429652  mov        edx, dword ptr [eax + 0x2c]
00429655  mov        ecx, edi
00429657  call       edx

// block 00429659
00429659  add        esi, eax

// block 0042965b
0042965b  test       byte ptr [edi + 0x430], 8
00429662  je         0x42966f

// block 00429664
00429664  mov        eax, dword ptr [edi]
00429666  mov        edx, dword ptr [eax + 0x30]
00429669  mov        ecx, edi
0042966b  call       edx

// block 0042966d
0042966d  add        esi, eax

// block 0042966f
0042966f  test       byte ptr [edi + 0x430], 0x10
00429676  je         0x429683

// block 00429678
00429678  mov        eax, dword ptr [edi]
0042967a  mov        edx, dword ptr [eax + 0x34]
0042967d  mov        ecx, edi
0042967f  call       edx

// block 00429681
00429681  add        esi, eax

// block 00429683
00429683  test       byte ptr [edi + 0x430], 0x40
0042968a  je         0x429697

// block 0042968c
0042968c  mov        eax, dword ptr [edi]
0042968e  mov        edx, dword ptr [eax + 0x38]
00429691  mov        ecx, edi
00429693  call       edx

// block 00429695
00429695  add        esi, eax

// block 00429697
00429697  test       byte ptr [edi + 0x430], 0x20
0042969e  je         0x4296ab

// block 004296a0
004296a0  mov        eax, dword ptr [edi]
004296a2  mov        edx, dword ptr [eax + 0x3c]
004296a5  mov        ecx, edi
004296a7  call       edx

// block 004296a9
004296a9  add        esi, eax

// block 004296ab
004296ab  test       dword ptr [edi + 0x430], 0x100
004296b5  je         0x4296c2

// block 004296b7
004296b7  mov        eax, dword ptr [edi]
004296b9  mov        edx, dword ptr [eax + 0x40]
004296bc  mov        ecx, edi
004296be  call       edx

// block 004296c0
004296c0  add        esi, eax

// block 004296c2
004296c2  test       dword ptr [edi + 0x430], 0x20000
004296cc  je         0x4296d9

// block 004296ce
004296ce  mov        eax, dword ptr [edi]
004296d0  mov        edx, dword ptr [eax + 0x44]
004296d3  mov        ecx, edi
004296d5  call       edx

// block 004296d7
004296d7  add        esi, eax

// block 004296d9
004296d9  test       dword ptr [edi + 0x430], 0x40000
004296e3  je         0x4296f0

// block 004296e5
004296e5  mov        eax, dword ptr [edi]
004296e7  mov        edx, dword ptr [eax + 0x48]
004296ea  mov        ecx, edi
004296ec  call       edx

// block 004296ee
004296ee  add        esi, eax

// block 004296f0
004296f0  test       dword ptr [edi + 0x430], 0x800000
004296fa  je         0x429707

// block 004296fc
004296fc  mov        eax, dword ptr [edi]
004296fe  mov        edx, dword ptr [eax + 0x4c]
00429701  mov        ecx, edi
00429703  call       edx

// block 00429705
00429705  add        esi, eax

// block 00429707
00429707  mov        eax, dword ptr [edi + 0x430]
0042970d  test       eax, 0x1000
00429712  je         0x42979a

// block 00429718
00429718  cmp        dword ptr [edi + 0x18c], 0
0042971f  jg         0x42972f

// block 00429721
00429721  xor        eax, 0x1000
00429726  mov        dword ptr [edi + 0x430], eax
0042972c  inc        esi
0042972d  jmp        0x42979a

// block 0042972f
0042972f  mov        eax, dword ptr [edi + 0x18c]
00429735  mov        ecx, dword ptr [edi + 0x194]
0042973b  mov        dword ptr [edi + 0x188], eax
00429741  fld        dword ptr [ecx]
00429743  fcomp      qword ptr [0x4a3db0]
00429749  fnstsw     ax
0042974b  test       ah, 0x41
0042974e  jne        0x42976d

// block 00429750
00429750  fld        dword ptr [0x4a3dac]
00429756  fcomp      dword ptr [ecx]
00429758  fnstsw     ax
0042975a  test       ah, 0x41
0042975d  jne        0x42976d

// block 0042975f
0042975f  fld        dword ptr [edi + 0x190]
00429765  fsub       qword ptr [0x4a3ce0]
0042976b  jmp        0x429783

// block 0042976d
0042976d  fld        dword ptr [edi + 0x190]
00429773  fld        dword ptr [ecx]
00429775  fmul       qword ptr [0x4a3ce0]
0042977b  fstp       dword ptr [esp + 0xc]
0042977f  fsub       dword ptr [esp + 0xc]

// block 00429783
00429783  fstp       dword ptr [edi + 0x190]
00429789  fld        dword ptr [edi + 0x190]
0042978f  call       0x4931e0

// block 00429794
00429794  mov        dword ptr [edi + 0x18c], eax

// block 0042979a
0042979a  mov        eax, dword ptr [edi + 0x44c]
004297a0  test       eax, eax
004297a2  je         0x4297ab

// block 004297a4
004297a4  dec        eax
004297a5  mov        dword ptr [edi + 0x44c], eax

// block 004297ab
004297ab  test       esi, esi
004297ad  jne        0x429620

// block 004297b3
004297b3  fld        dword ptr [edi + 0x6c]
004297b6  fld        dword ptr [edi + 0x464]
004297bc  fcompp     
004297be  fnstsw     ax
004297c0  fldz       
004297c2  test       ah, 0x41
004297c5  jne        0x429801

// block 004297c7
004297c7  fstp       st(0)
004297c9  fld        dword ptr [edi + 0x74]
004297cc  fmul       dword ptr [0x4b2ed0]
004297d2  fadd       dword ptr [edi + 0x6c]
004297d5  fstp       dword ptr [esp + 0xc]
004297d9  fld        dword ptr [esp + 0xc]
004297dd  fst        dword ptr [edi + 0x6c]
004297e0  fld        dword ptr [edi + 0x464]
004297e6  fcompp     
004297e8  fnstsw     ax
004297ea  test       ah, 5
004297ed  jp         0x4298a4

// block 004297f3
004297f3  fld        dword ptr [edi + 0x464]
004297f9  fstp       dword ptr [edi + 0x6c]
004297fc  jmp        0x4298a4

// block 00429801
00429801  fld        dword ptr [edi + 0x74]
00429804  fmul       dword ptr [0x4b2ed0]
0042980a  fadd       dword ptr [edi + 0x78]
0042980d  fstp       dword ptr [edi + 0x78]
00429810  fld        dword ptr [edi + 0x5c]
00429813  fld        dword ptr [0x4b2ed0]
00429819  fld        st(0)
0042981b  fmulp      st(2)
0042981d  fxch       st(1)
0042981f  fstp       dword ptr [esp + 0x10]
00429823  fld        dword ptr [edi + 0x60]
00429826  fmul       st(1)
00429828  fstp       dword ptr [esp + 0x14]
0042982c  fmul       dword ptr [edi + 0x64]
0042982f  fstp       dword ptr [esp + 0x18]
00429833  fld        dword ptr [edi + 0x50]
00429836  fadd       dword ptr [esp + 0x10]
0042983a  fstp       dword ptr [edi + 0x50]
0042983d  fld        dword ptr [esp + 0x14]
00429841  fadd       dword ptr [edi + 0x54]
00429844  fstp       dword ptr [edi + 0x54]
00429847  fld        dword ptr [edi + 0x58]
0042984a  fadd       dword ptr [esp + 0x18]
0042984e  fstp       dword ptr [edi + 0x58]
00429851  fcom       dword ptr [edi + 0x46c]
00429857  fnstsw     ax
00429859  test       ah, 5
0042985c  jp         0x4298a2

// block 0042985e
0042985e  fld        dword ptr [edi + 0x6c]
00429861  fadd       dword ptr [edi + 0x78]
00429864  fld        dword ptr [edi + 0x46c]
0042986a  fcompp     
0042986c  fnstsw     ax
0042986e  test       ah, 5
00429871  jp         0x4298a2

// block 00429873
00429873  fld        dword ptr [edi + 0x46c]
00429879  fsub       dword ptr [edi + 0x78]
0042987c  fstp       dword ptr [esp + 0xc]
00429880  fld        dword ptr [esp + 0xc]
00429884  fst        dword ptr [edi + 0x6c]
00429887  fstp       dword ptr [edi + 0x464]
0042988d  fcomp      dword ptr [edi + 0x6c]
00429890  fnstsw     ax
00429892  test       ah, 1
00429895  jne        0x4298a4

// block 00429897
00429897  mov        eax, 1
0042989c  pop        edi
0042989d  pop        esi
0042989e  mov        esp, ebp
004298a0  pop        ebp
004298a1  ret        

// block 004298a2
004298a2  fstp       st(0)

// block 004298a4
004298a4  cmp        dword ptr [edi + 0x43c], 0
004298ab  jg         0x42992b

// block 004298ad
004298ad  fld        dword ptr [edi + 0x6c]
004298b0  sub        esp, 8
004298b3  fstp       dword ptr [esp + 4]
004298b7  lea        ecx, [esp + 0x18]
004298bb  fld        dword ptr [edi + 0x68]
004298be  fstp       dword ptr [esp]
004298c1  call       0x42e880

// block 004298c6
004298c6  fld        dword ptr [edi + 0x50]
004298c9  lea        ecx, [edi + 0x50]
004298cc  fadd       dword ptr [esp + 0x10]
004298d0  sub        esp, 8
004298d3  fstp       dword ptr [esp + 0x18]
004298d7  fld        dword ptr [ecx + 4]
004298da  fadd       dword ptr [esp + 0x1c]
004298de  fstp       dword ptr [esp + 0x1c]
004298e2  fld        dword ptr [ecx + 8]
004298e5  fadd       dword ptr [esp + 0x20]
004298e9  fstp       dword ptr [esp + 0x20]
004298ed  fld        dword ptr [edi + 0x70]
004298f0  fstp       dword ptr [esp + 4]
004298f4  fld        dword ptr [edi + 0x70]
004298f7  fstp       dword ptr [esp]
004298fa  call       0x42e820

// block 004298ff
004298ff  test       eax, eax
00429901  je         0x429940

// block 00429903
00429903  fld        dword ptr [edi + 0x70]
00429906  sub        esp, 8
00429909  fstp       dword ptr [esp + 4]
0042990d  lea        ecx, [esp + 0x18]
00429911  fld        dword ptr [edi + 0x70]
00429914  fstp       dword ptr [esp]
00429917  call       0x42e820

// block 0042991c
0042991c  test       eax, eax
0042991e  je         0x429940

// block 00429920
00429920  mov        eax, 1
00429925  pop        edi
00429926  pop        esi
00429927  mov        esp, ebp
00429929  pop        ebp
0042992a  ret        

// block 0042992b
0042992b  fld        dword ptr [0x4a3da8]
00429931  push       ecx
00429932  lea        esi, [edi + 0x438]
00429938  fstp       dword ptr [esp]
0042993b  call       0x464a20

// block 00429940
00429940  fld        dword ptr [0x4a3e68]
00429946  fcomp      dword ptr [edi + 0x6c]
00429949  fnstsw     ax
0042994b  test       ah, 5
0042994e  jp         0x429a7e

// block 00429954
00429954  fld        dword ptr [0x4a4020]
0042995a  fcomp      dword ptr [edi + 0x70]
0042995d  fnstsw     ax
0042995f  test       ah, 5
00429962  jp         0x429a7e

// block 00429968
00429968  fld        dword ptr [edi + 0x6c]
0042996b  sub        esp, 8
0042996e  fdiv       qword ptr [0x4a3e40]
00429974  lea        ecx, [esp + 0x18]
00429978  fstp       dword ptr [esp + 0x14]
0042997c  fld        dword ptr [esp + 0x14]
00429980  fstp       dword ptr [esp + 4]
00429984  fld        dword ptr [edi + 0x68]
00429987  fstp       dword ptr [esp]
0042998a  call       0x42e880

// block 0042998f
0042998f  fld        dword ptr [esp + 0x10]
00429993  fadd       dword ptr [edi + 0x50]
00429996  fstp       dword ptr [esp + 0x10]
0042999a  fld        dword ptr [edi + 0x54]
0042999d  fadd       dword ptr [esp + 0x14]
004299a1  fstp       dword ptr [esp + 0x14]
004299a5  fld        dword ptr [edi + 0x58]
004299a8  fadd       dword ptr [esp + 0x18]
004299ac  fstp       dword ptr [esp + 0x18]
004299b0  fld        dword ptr [0x4a3d78]
004299b6  fcomp      dword ptr [edi + 0x70]
004299b9  fnstsw     ax
004299bb  test       ah, 0x41
004299be  jne        0x4299cb

// block 004299c0
004299c0  fld        dword ptr [edi + 0x70]
004299c3  fmul       qword ptr [0x4a3cf8]
004299c9  jmp        0x4299de

// block 004299cb
004299cb  fld        dword ptr [edi + 0x70]
004299ce  fld        qword ptr [0x4a3d68]
004299d4  fadd       st(1)
004299d6  fmul       qword ptr [0x4a3cf8]
004299dc  fsubp      st(1)

// block 004299de
004299de  fld        dword ptr [edi + 0x6c]
004299e1  sub        esp, 0xc
004299e4  fmul       qword ptr [0x4a4108]
004299ea  lea        eax, [esp + 0x1c]
004299ee  fdiv       qword ptr [0x4a3fd0]
004299f4  fstp       dword ptr [esp + 0x18]
004299f8  fld        dword ptr [esp + 0x18]
004299fc  fstp       dword ptr [esp + 8]
00429a00  fstp       dword ptr [esp + 0x18]
00429a04  fld        dword ptr [esp + 0x18]
00429a08  fstp       dword ptr [esp + 4]
00429a0c  fld        dword ptr [edi + 0x68]
00429a0f  fstp       dword ptr [esp]
00429a12  call       0x437a80

// block 00429a17
00429a17  cmp        eax, 1
00429a1a  jne        0x429a50

// block 00429a1c
00429a1c  fld        dword ptr [0x4a3d78]
00429a22  mov        ecx, dword ptr [0x4b4514]
00429a28  mov        edx, dword ptr [edi]
00429a2a  fst        dword ptr [esp + 0x1c]
00429a2e  mov        edx, dword ptr [edx + 0x18]
00429a31  fstp       dword ptr [esp + 0x20]
00429a35  fldz       
00429a37  push       eax
00429a38  push       0
00429a3a  fstp       dword ptr [esp + 0x2c]
00429a3e  lea        eax, [esp + 0x24]
00429a42  add        ecx, 0x97c
00429a48  push       eax
00429a49  push       ecx
00429a4a  mov        ecx, edi
00429a4c  call       edx

// block 00429a4e
00429a4e  jmp        0x429a7e

// block 00429a50
00429a50  cmp        eax, 2
00429a53  jne        0x429a7e

// block 00429a55
00429a55  mov        eax, dword ptr [edi + 0x2c]
00429a58  cdq        
00429a59  mov        ecx, 3
00429a5e  idiv       ecx
00429a60  test       edx, edx
00429a62  jne        0x429a76

// block 00429a64
00429a64  mov        edx, dword ptr [0x4b4514]
00429a6a  add        edx, 0x97c
00429a70  push       edx
00429a71  call       0x4391c0

// block 00429a76
00429a76  lea        esi, [edi + 0x28]
00429a79  call       0x464a80

// block 00429a7e
00429a7e  mov        ecx, dword ptr [edi + 0xa30]
00429a84  fld        dword ptr [edi + 0x70]
00429a87  fdiv       dword ptr [ecx + 0x38]
00429a8a  lea        eax, [edi + 0x63c]
00429a90  mov        ecx, 8
00429a95  or         dword ptr [eax + 0x47c], ecx
00429a9b  push       eax
00429a9c  fstp       dword ptr [eax + 0x40]
00429a9f  fld        dword ptr [edi + 0x6c]
00429aa2  mov        edx, dword ptr [edi + 0xa30]
00429aa8  fdiv       dword ptr [edx + 0x34]
00429aab  or         dword ptr [eax + 0x47c], ecx
00429ab1  fstp       dword ptr [eax + 0x44]
00429ab4  call       0x455630

// block 00429ab9
00429ab9  fldz       
00429abb  fcomp      dword ptr [edi + 0x78]
00429abe  fnstsw     ax
00429ac0  test       ah, 0x44
00429ac3  jp         0x429ad1

// block 00429ac5
00429ac5  lea        eax, [edi + 0xaf0]
00429acb  push       eax
00429acc  call       0x455630

// block 00429ad1
00429ad1  lea        esi, [edi + 0x3c]
00429ad4  call       0x464a80

// block 00429ad9
00429ad9  pop        edi
00429ada  xor        eax, eax
00429adc  pop        esi
00429add  mov        esp, ebp
00429adf  pop        ebp
00429ae0  ret        
