
// block 004273f0
004273f0  sub        esp, 0xc
004273f3  push       ebx
004273f4  mov        ebx, dword ptr [esp + 0x14]
004273f8  push       ebp
004273f9  mov        ebp, dword ptr [esp + 0x1c]
004273fd  push       esi
004273fe  lea        eax, [ebx - 0xa]
00427401  push       edi
00427402  mov        edi, dword ptr [0x4b44f0]
00427408  cmp        eax, 5
0042740b  ja         0x4275e1

// block 00427411
00427411  inc        dword ptr [edi + 0x666fe0]
00427417  fld        dword ptr [ebp]
0042741a  push       ecx
0042741b  mov        eax, 0x37
00427420  fstp       dword ptr [esp]
00427423  call       0x453e20

// block 00427428
00427428  xor        edx, edx
0042742a  lea        esi, [edi + 0x171254]
00427430  xor        eax, eax

// block 00427432
00427432  cmp        dword ptr [esi + 0x9b0], edx
00427438  je         0x427452

// block 0042743a
0042743a  inc        eax
0042743b  add        esi, 0x9d8
00427441  cmp        eax, 0x10
00427444  jl         0x427432

// block 00427446
00427446  mov        eax, esi
00427448  pop        edi
00427449  pop        esi
0042744a  pop        ebp
0042744b  pop        ebx
0042744c  add        esp, 0xc
0042744f  ret        0x10

// block 00427452
00427452  fld        dword ptr [0x4a3d98]
00427458  mov        dword ptr [esi + 0x9b0], 6
00427462  mov        ecx, dword ptr [ebp]
00427465  mov        dword ptr [esi + 0x96c], ecx
0042746b  mov        eax, dword ptr [ebp + 4]
0042746e  mov        dword ptr [esi + 0x970], eax
00427474  mov        ecx, dword ptr [ebp + 8]
00427477  mov        dword ptr [esi + 0x974], ecx
0042747d  fcom       dword ptr [esi + 0x96c]
00427483  fnstsw     ax
00427485  test       ah, 1
00427488  jne        0x427492

// block 0042748a
0042748a  fstp       dword ptr [esi + 0x96c]
00427490  jmp        0x4274b1

// block 00427492
00427492  fstp       st(0)
00427494  fld        dword ptr [0x4a3d88]
0042749a  fcom       dword ptr [esi + 0x96c]
004274a0  fnstsw     ax
004274a2  test       ah, 0x41
004274a5  jp         0x4274af

// block 004274a7
004274a7  fstp       dword ptr [esi + 0x96c]
004274ad  jmp        0x4274b1

// block 004274af
004274af  fstp       st(0)

// block 004274b1
004274b1  mov        edi, dword ptr [edi + 0x666fe0]
004274b7  and        edi, 1
004274ba  sub        edi, edx
004274bc  je         0x4274cb

// block 004274be
004274be  sub        edi, 1
004274c1  jne        0x4274d5

// block 004274c3
004274c3  fld        dword ptr [0x4a4278]
004274c9  jmp        0x4274d1

// block 004274cb
004274cb  fld        dword ptr [0x4a4274]

// block 004274d1
004274d1  fstp       dword ptr [esp + 0x28]

// block 004274d5
004274d5  fld        dword ptr [0x4a4270]
004274db  sub        esp, 8
004274de  fstp       dword ptr [esp + 4]
004274e2  lea        ecx, [esi + 0x978]
004274e8  fld        dword ptr [esp + 0x30]
004274ec  fstp       dword ptr [esp]
004274ef  call       0x427d00

// block 004274f4
004274f4  fldz       
004274f6  mov        edi, 0xfff0bdc1
004274fb  fst        dword ptr [esi + 0x980]
00427501  mov        ecx, 0x4b2ed0
00427506  mov        eax, dword ptr [esi + 0x998]
0042750c  test       al, 1
0042750e  jne        0x427531

// block 00427510
00427510  or         eax, 1
00427513  fst        dword ptr [esi + 0x990]
00427519  mov        dword ptr [esi + 0x98c], edx
0042751f  mov        dword ptr [esi + 0x988], edi
00427525  mov        dword ptr [esi + 0x994], ecx
0042752b  mov        dword ptr [esi + 0x998], eax

// block 00427531
00427531  or         ebp, 0xffffffff
00427534  fst        dword ptr [esi + 0x990]
0042753a  mov        dword ptr [esi + 0x98c], edx
00427540  mov        dword ptr [esi + 0x988], ebp
00427546  mov        eax, dword ptr [esi + 0x9ac]
0042754c  test       al, 1
0042754e  jne        0x427571

// block 00427550
00427550  or         eax, 1
00427553  fst        dword ptr [esi + 0x9a4]
00427559  mov        dword ptr [esi + 0x9a0], edx
0042755f  mov        dword ptr [esi + 0x99c], edi
00427565  mov        dword ptr [esi + 0x9a8], ecx
0042756b  mov        dword ptr [esi + 0x9ac], eax

// block 00427571
00427571  mov        dword ptr [esi + 0x9a0], edx
00427577  fst        dword ptr [esi + 0x9a4]
0042757d  mov        dword ptr [esi + 0x99c], ebp
00427583  fstp       dword ptr [esi + 0x984]
00427589  mov        dword ptr [esi + 0x968], edx
0042758f  mov        dword ptr [esi + 0x9b8], edx
00427595  mov        dword ptr [esi + 0x9c4], edx
0042759b  mov        edx, dword ptr [0x4b43c8]
004275a1  mov        dword ptr [esi + 0x9b4], ebx
004275a7  mov        edi, dword ptr [edx + 0x4debdc]
004275ad  call       0x402520

// block 004275b2
004275b2  add        ebx, 0xa5
004275b8  push       ebx
004275b9  push       esi
004275ba  mov        ecx, edi
004275bc  mov        byte ptr [esi + 0x49d], 0x10
004275c3  mov        byte ptr [esi + 0x49c], 0x10
004275ca  call       0x454d10

// block 004275cf
004275cf  mov        dword ptr [esi + 0x3bc], ebp
004275d5  mov        eax, esi
004275d7  pop        edi
004275d8  pop        esi
004275d9  pop        ebp
004275da  pop        ebx
004275db  add        esp, 0xc
004275de  ret        0x10

// block 004275e1
004275e1  lea        eax, [ebx - 0x10]
004275e4  cmp        eax, 2
004275e7  ja         0x427745

// block 004275ed
004275ed  xor        edx, edx
004275ef  lea        esi, [edi + 0x171254]
004275f5  xor        eax, eax

// block 004275f7
004275f7  cmp        dword ptr [esi + 0x9b0], edx
004275fd  je         0x427617

// block 004275ff
004275ff  inc        eax
00427600  add        esi, 0x9d8
00427606  cmp        eax, 0x10
00427609  jl         0x4275f7

// block 0042760b
0042760b  mov        eax, esi
0042760d  pop        edi
0042760e  pop        esi
0042760f  pop        ebp
00427610  pop        ebx
00427611  add        esp, 0xc
00427614  ret        0x10

// block 00427617
00427617  fld        dword ptr [0x4a3d98]
0042761d  mov        dword ptr [esi + 0x9b0], 8
00427627  mov        ecx, dword ptr [ebp]
0042762a  mov        dword ptr [esi + 0x96c], ecx
00427630  mov        eax, dword ptr [ebp + 4]
00427633  mov        dword ptr [esi + 0x970], eax
00427639  mov        ecx, dword ptr [ebp + 8]
0042763c  mov        dword ptr [esi + 0x974], ecx
00427642  fcom       dword ptr [esi + 0x96c]
00427648  fnstsw     ax
0042764a  test       ah, 1
0042764d  jne        0x427657

// block 0042764f
0042764f  fstp       dword ptr [esi + 0x96c]
00427655  jmp        0x427676

// block 00427657
00427657  fstp       st(0)
00427659  fld        dword ptr [0x4a3d88]
0042765f  fcom       dword ptr [esi + 0x96c]
00427665  fnstsw     ax
00427667  test       ah, 0x41
0042766a  jp         0x427674

// block 0042766c
0042766c  fstp       dword ptr [esi + 0x96c]
00427672  jmp        0x427676

// block 00427674
00427674  fstp       st(0)

// block 00427676
00427676  fldz       
00427678  fcom       dword ptr [esi + 0x970]
0042767e  fnstsw     ax
00427680  test       ah, 1
00427683  jne        0x42768b

// block 00427685
00427685  fst        dword ptr [esi + 0x970]

// block 0042768b
0042768b  sub        esp, 8
0042768e  fstp       dword ptr [esp + 4]
00427692  lea        ecx, [esi + 0x978]
00427698  fld        dword ptr [esp + 0x30]
0042769c  fstp       dword ptr [esp]
0042769f  call       0x427d00

// block 004276a4
004276a4  fldz       
004276a6  fst        dword ptr [esi + 0x980]
004276ac  mov        eax, dword ptr [esi + 0x998]
004276b2  test       al, 1
004276b4  jne        0x4276df

// block 004276b6
004276b6  or         eax, 1
004276b9  fst        dword ptr [esi + 0x990]
004276bf  mov        dword ptr [esi + 0x98c], edx
004276c5  mov        dword ptr [esi + 0x988], 0xfff0bdc1
004276cf  mov        dword ptr [esi + 0x994], 0x4b2ed0
004276d9  mov        dword ptr [esi + 0x998], eax

// block 004276df
004276df  mov        eax, dword ptr [0x4b43c8]
004276e4  fst        dword ptr [esi + 0x990]
004276ea  mov        dword ptr [esi + 0x98c], edx
004276f0  or         edi, 0xffffffff
004276f3  mov        dword ptr [esi + 0x988], edi
004276f9  fstp       dword ptr [esi + 0x984]
004276ff  push       edx
00427700  mov        dword ptr [esi + 0x9b4], ebx
00427706  mov        dword ptr [esi + 0x9b8], edx
0042770c  mov        ecx, dword ptr [eax + 0x4debdc]
00427712  add        ebx, 0xa5
00427718  push       ebx
00427719  lea        edx, [esp + 0x30]
0042771d  push       edx
0042771e  push       ecx
0042771f  lea        eax, [edi + 0x18]
00427722  xor        ecx, ecx
00427724  call       0x4615a0

// block 00427729
00427729  mov        edx, dword ptr [esp + 0x28]
0042772d  mov        dword ptr [esi + 0x968], edx
00427733  mov        dword ptr [esi + 0x3bc], edi
00427739  mov        eax, esi
0042773b  pop        edi
0042773c  pop        esi
0042773d  pop        ebp
0042773e  pop        ebx
0042773f  add        esp, 0xc
00427742  ret        0x10

// block 00427745
00427745  mov        edx, 9
0042774a  cmp        ebx, edx
0042774c  jne        0x42787c

// block 00427752
00427752  mov        eax, dword ptr [edi + 0x666fd8]
00427758  inc        dword ptr [edi + 0x666fdc]
0042775e  mov        ecx, eax
00427760  imul       ecx, ecx, 0x9d8
00427766  cmp        dword ptr [ecx + edi + 0x17b984], 0
0042776e  lea        esi, [ecx + edi + 0x17afd4]
00427775  mov        ecx, dword ptr [edi + 0x666fdc]
0042777b  jne        0x427853

// block 00427781
00427781  cmp        ecx, 0x400
00427787  jl         0x42779a

// block 00427789
00427789  and        eax, 0x8000001f
0042778e  jns        0x427795

// block 00427790
00427790  dec        eax
00427791  or         eax, 0xffffffe0
00427794  inc        eax

// block 00427795
00427795  add        eax, 0x10
00427798  jmp        0x4277d8

// block 0042779a
0042779a  cmp        ecx, 0x200
004277a0  jl         0x4277b3

// block 004277a2
004277a2  and        eax, 0x8000000f
004277a7  jns        0x4277ae

// block 004277a9
004277a9  dec        eax
004277aa  or         eax, 0xfffffff0
004277ad  inc        eax

// block 004277ae
004277ae  add        eax, 8
004277b1  jmp        0x4277d8

// block 004277b3
004277b3  cmp        ecx, 0x100
004277b9  jl         0x4277cc

// block 004277bb
004277bb  and        eax, 0x80000007
004277c0  jns        0x4277c7

// block 004277c2
004277c2  dec        eax
004277c3  or         eax, 0xfffffff8
004277c6  inc        eax

// block 004277c7
004277c7  add        eax, 4
004277ca  jmp        0x4277d8

// block 004277cc
004277cc  and        eax, 0x80000003
004277d1  jns        0x4277d8

// block 004277d3
004277d3  dec        eax
004277d4  or         eax, 0xfffffffc
004277d7  inc        eax

// block 004277d8
004277d8  fld        dword ptr [esp + 0x2c]
004277dc  mov        dword ptr [esi + 0x9c0], eax
004277e2  mov        dword ptr [esi + 0x9b0], 5
004277ec  mov        dword ptr [esi + 0x9b4], edx
004277f2  mov        dword ptr [esi + 0x9b8], edx
004277f8  mov        edx, dword ptr [ebp]
004277fb  mov        dword ptr [esi + 0x96c], edx
00427801  mov        eax, dword ptr [ebp + 4]
00427804  mov        dword ptr [esi + 0x970], eax
0042780a  mov        ecx, dword ptr [ebp + 8]
0042780d  sub        esp, 8
00427810  fstp       dword ptr [esp + 4]
00427814  mov        dword ptr [esi + 0x974], ecx
0042781a  fld        dword ptr [esp + 0x30]
0042781e  lea        ecx, [esi + 0x978]
00427824  fstp       dword ptr [esp]
00427827  call       0x427d00

// block 0042782c
0042782c  fldz       
0042782e  push       0
00427830  lea        eax, [esi + 0x988]
00427836  fstp       dword ptr [esi + 0x980]
0042783c  call       0x4067e0

// block 00427841
00427841  fldz       
00427843  fstp       dword ptr [esi + 0x984]
00427849  mov        dword ptr [esi + 0x968], 0

// block 00427853
00427853  mov        edx, dword ptr [edi + 0x666fd8]
00427859  inc        edx
0042785a  and        edx, 0x800007ff
00427860  jns        0x42786a

// block 00427862
00427862  dec        edx
00427863  or         edx, 0xfffff800
00427869  inc        edx

// block 0042786a
0042786a  mov        dword ptr [edi + 0x666fd8], edx
00427870  mov        eax, esi
00427872  pop        edi
00427873  pop        esi
00427874  pop        ebp
00427875  pop        ebx
00427876  add        esp, 0xc
00427879  ret        0x10

// block 0042787c
0042787c  add        edi, 0x14
0042787f  xor        edx, edx
00427881  xor        eax, eax

// block 00427883
00427883  cmp        dword ptr [edi + 0x9b0], edx
00427889  je         0x4278a5

// block 0042788b
0042788b  inc        eax
0042788c  add        edi, 0x9d8
00427892  cmp        eax, 0x258
00427897  jl         0x427883

// block 00427899
00427899  mov        eax, edi
0042789b  pop        edi
0042789c  pop        esi
0042789d  pop        ebp
0042789e  pop        ebx
0042789f  add        esp, 0xc
004278a2  ret        0x10

// block 004278a5
004278a5  fld        dword ptr [0x4a3d98]
004278ab  mov        esi, 1
004278b0  mov        dword ptr [edi + 0x9b0], esi
004278b6  mov        eax, dword ptr [ebp]
004278b9  lea        ebx, [edi + 0x96c]
004278bf  mov        dword ptr [ebx], eax
004278c1  mov        ecx, dword ptr [ebp + 4]
004278c4  mov        dword ptr [ebx + 4], ecx
004278c7  mov        eax, dword ptr [ebp + 8]
004278ca  mov        dword ptr [ebx + 8], eax
004278cd  fcom       dword ptr [ebx]
004278cf  fnstsw     ax
004278d1  test       ah, 1
004278d4  jne        0x4278da

// block 004278d6
004278d6  fstp       dword ptr [ebx]
004278d8  jmp        0x4278f1

// block 004278da
004278da  fstp       st(0)
004278dc  fld        dword ptr [0x4a3d88]
004278e2  fcom       dword ptr [ebx]
004278e4  fnstsw     ax
004278e6  test       ah, 0x41
004278e9  jp         0x4278ef

// block 004278eb
004278eb  fstp       dword ptr [ebx]
004278ed  jmp        0x4278f1

// block 004278ef
004278ef  fstp       st(0)

// block 004278f1
004278f1  fld        dword ptr [esp + 0x2c]
004278f5  sub        esp, 8
004278f8  fstp       dword ptr [esp + 4]
004278fc  lea        ecx, [edi + 0x978]
00427902  fld        dword ptr [esp + 0x30]
00427906  fstp       dword ptr [esp]
00427909  call       0x427d00

// block 0042790e
0042790e  fldz       
00427910  fst        dword ptr [edi + 0x980]
00427916  mov        eax, dword ptr [edi + 0x998]
0042791c  test       al, 1
0042791e  jne        0x427948

// block 00427920
00427920  or         eax, esi
00427922  fst        dword ptr [edi + 0x990]
00427928  mov        dword ptr [edi + 0x98c], edx
0042792e  mov        dword ptr [edi + 0x988], 0xfff0bdc1
00427938  mov        dword ptr [edi + 0x994], 0x4b2ed0
00427942  mov        dword ptr [edi + 0x998], eax

// block 00427948
00427948  mov        ebp, dword ptr [esp + 0x20]
0042794c  fst        dword ptr [edi + 0x990]
00427952  mov        dword ptr [edi + 0x98c], edx
00427958  mov        dword ptr [edi + 0x988], 0xffffffff
00427962  fst        dword ptr [edi + 0x984]
00427968  fstp       dword ptr [edi + 0x9bc]
0042796e  mov        dword ptr [edi + 0x968], edx
00427974  cmp        ebp, 4
00427977  je         0x42798c

// block 00427979
00427979  cmp        ebp, 5
0042797c  je         0x42798c

// block 0042797e
0042797e  cmp        ebp, 6
00427981  je         0x42798c

// block 00427983
00427983  cmp        ebp, 7
00427986  jne        0x427a70

// block 0042798c
0042798c  test       dword ptr [0x4cee78], 0x8000
00427996  mov        ecx, dword ptr [0x4b43c8]
0042799c  mov        ebp, dword ptr [ecx + 0x4debdc]
004279a2  je         0x4279b5

// block 004279a4
004279a4  push       0x4cf1d0
004279a9  call       dword ptr [0x498088]

// block 004279af
004279af  inc        byte ptr [0x4cf221]

// block 004279b5
004279b5  add        dword ptr [ebp + 0x130], esi
004279bb  call       0x4621c0

// block 004279c0
004279c0  mov        esi, eax
004279c2  or         dword ptr [esi + 0x480], 1
004279c9  mov        dword ptr [esi + 0x20], 0x17
004279d0  test       ebx, ebx
004279d2  jne        0x427a02

// block 004279d4
004279d4  fldz       
004279d6  fst        dword ptr [esp + 0x10]
004279da  mov        edx, dword ptr [esp + 0x10]
004279de  fst        dword ptr [esp + 0x14]
004279e2  mov        eax, dword ptr [esp + 0x14]
004279e6  fstp       dword ptr [esp + 0x18]
004279ea  mov        ecx, dword ptr [esp + 0x18]
004279ee  mov        dword ptr [esi + 0x430], edx
004279f4  mov        dword ptr [esi + 0x434], eax
004279fa  mov        dword ptr [esi + 0x438], ecx
00427a00  jmp        0x427a2e

// block 00427a02
00427a02  fld        dword ptr [ebx]
00427a04  fadd       qword ptr [0x4a3d70]
00427a0a  fadd       qword ptr [0x4a3d28]
00427a10  fstp       dword ptr [esi + 0x430]
00427a16  fld        dword ptr [ebx + 4]
00427a19  fadd       qword ptr [0x4a3d68]
00427a1f  fstp       dword ptr [esi + 0x434]
00427a25  fld        dword ptr [ebx + 8]
00427a28  fstp       dword ptr [esi + 0x438]

// block 00427a2e
00427a2e  push       0x64
00427a30  push       esi
00427a31  mov        ecx, ebp
00427a33  call       0x454d10

// block 00427a38
00427a38  mov        ebx, esi
00427a3a  lea        eax, [esp + 0x28]
00427a3e  call       0x461250

// block 00427a43
00427a43  test       dword ptr [0x4cee78], 0x8000
00427a4d  je         0x427a60

// block 00427a4f
00427a4f  push       0x4cf1d0
00427a54  call       dword ptr [0x49808c]

// block 00427a5a
00427a5a  dec        byte ptr [0x4cf221]

// block 00427a60
00427a60  mov        edx, 0x32
00427a65  call       0x453d90

// block 00427a6a
00427a6a  mov        ebp, dword ptr [esp + 0x20]
00427a6e  xor        edx, edx

// block 00427a70
00427a70  mov        dword ptr [edi + 0x9b8], edx
00427a76  mov        edx, dword ptr [0x4b43c8]
00427a7c  mov        dword ptr [edi + 0x9b4], ebp
00427a82  mov        ebx, dword ptr [edx + 0x4debdc]
00427a88  mov        esi, edi
00427a8a  call       0x402520

// block 00427a8f
00427a8f  lea        eax, [ebp + 0xa5]
00427a95  push       eax
00427a96  push       edi
00427a97  mov        ecx, ebx
00427a99  mov        byte ptr [edi + 0x49d], 0x10
00427aa0  mov        byte ptr [edi + 0x49c], 0x10
00427aa7  call       0x454d10

// block 00427aac
00427aac  mov        ecx, dword ptr [0x4b43c8]
00427ab2  mov        ebx, dword ptr [ecx + 0x4debdc]
00427ab8  lea        esi, [edi + 0x4b4]
00427abe  call       0x402520

// block 00427ac3
00427ac3  add        ebp, 0xb9
00427ac9  push       ebp
00427aca  push       esi
00427acb  mov        ecx, ebx
00427acd  mov        byte ptr [esi + 0x49d], 0x10
00427ad4  mov        byte ptr [esi + 0x49c], 0x10
00427adb  call       0x454d10

// block 00427ae0
00427ae0  mov        dword ptr [edi + 0x3bc], 0xffffffff
00427aea  mov        eax, edi
00427aec  pop        edi
00427aed  pop        esi
00427aee  pop        ebp
00427aef  pop        ebx
00427af0  add        esp, 0xc
00427af3  ret        0x10
