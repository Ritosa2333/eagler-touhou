
// block 00413700
00413700  sub        esp, 0x18
00413703  fld        dword ptr [esi + 0x9c]
00413709  push       ebx
0041370a  fadd       dword ptr [esi + 0x68]
0041370d  lea        ebx, [esi + 0x34]
00413710  fstp       dword ptr [esp + 4]
00413714  fld        dword ptr [esi + 0xa0]
0041371a  fadd       dword ptr [esi + 0x6c]
0041371d  fstp       dword ptr [esp + 8]
00413721  fld        dword ptr [esi + 0xa4]
00413727  fadd       dword ptr [esi + 0x70]
0041372a  fstp       dword ptr [esp + 0xc]
0041372e  fld        dword ptr [esp + 4]
00413732  fsub       dword ptr [ebx]
00413734  fstp       dword ptr [esp + 0x10]
00413738  mov        eax, dword ptr [esp + 0x10]
0041373c  fld        dword ptr [esp + 8]
00413740  fsub       dword ptr [ebx + 4]
00413743  fstp       dword ptr [esp + 0x14]
00413747  mov        ecx, dword ptr [esp + 0x14]
0041374b  fld        dword ptr [esp + 0xc]
0041374f  fsub       dword ptr [ebx + 8]
00413752  mov        dword ptr [esi + 0x40], eax
00413755  mov        dword ptr [esi + 0x44], ecx
00413758  fstp       dword ptr [esp + 0x18]
0041375c  mov        edx, dword ptr [esp + 0x18]
00413760  mov        dword ptr [esi + 0x48], edx
00413763  call       0x464db0

// block 00413768
00413768  test       byte ptr [esi + 0x16ba], 1
0041376f  je         0x41382d

// block 00413775
00413775  fld        dword ptr [esi + 0x15fc]
0041377b  fld        qword ptr [0x4a3cf8]
00413781  fmul       st(1), st(0)
00413783  fld        dword ptr [esi + 0x15f4]
00413789  fsub       st(2)
0041378b  fld        dword ptr [ebx]
0041378d  fcomp      st(1)
0041378f  fnstsw     ax
00413791  test       ah, 5
00413794  jp         0x41379e

// block 00413796
00413796  fstp       st(2)
00413798  fxch       st(1)
0041379a  fstp       dword ptr [ebx]
0041379c  jmp        0x4137bb

// block 0041379e
0041379e  fstp       st(0)
004137a0  fld        dword ptr [esi + 0x15f4]
004137a6  faddp      st(2)
004137a8  fld        dword ptr [ebx]
004137aa  fcomp      st(2)
004137ac  fnstsw     ax
004137ae  test       ah, 0x41
004137b1  jne        0x4137b9

// block 004137b3
004137b3  fxch       st(1)
004137b5  fstp       dword ptr [ebx]
004137b7  jmp        0x4137bb

// block 004137b9
004137b9  fstp       st(1)

// block 004137bb
004137bb  fmul       dword ptr [esi + 0x1600]
004137c1  fld        dword ptr [esi + 0x15f8]
004137c7  fsub       st(1)
004137c9  fld        dword ptr [esi + 0x38]
004137cc  fcomp      st(1)
004137ce  fnstsw     ax
004137d0  test       ah, 5
004137d3  jp         0x4137dc

// block 004137d5
004137d5  fstp       st(1)

// block 004137d7
004137d7  fstp       dword ptr [esi + 0x38]
004137da  jmp        0x4137f2

// block 004137dc
004137dc  fstp       st(0)
004137de  fadd       dword ptr [esi + 0x15f8]
004137e4  fld        dword ptr [esi + 0x38]
004137e7  fcomp      st(1)
004137e9  fnstsw     ax
004137eb  test       ah, 0x41
004137ee  je         0x4137d7

// block 004137f0
004137f0  fstp       st(0)

// block 004137f2
004137f2  fld        dword ptr [ebx]
004137f4  fsub       dword ptr [esi + 0x9c]
004137fa  fstp       dword ptr [esp + 0x10]
004137fe  mov        eax, dword ptr [esp + 0x10]
00413802  fld        dword ptr [ebx + 4]
00413805  fsub       dword ptr [esi + 0xa0]
0041380b  fstp       dword ptr [esp + 0x14]
0041380f  mov        ecx, dword ptr [esp + 0x14]
00413813  fld        dword ptr [ebx + 8]
00413816  fsub       dword ptr [esi + 0xa4]
0041381c  mov        dword ptr [esi + 0x68], eax
0041381f  mov        dword ptr [esi + 0x6c], ecx
00413822  fstp       dword ptr [esp + 0x18]
00413826  mov        edx, dword ptr [esp + 0x18]
0041382a  mov        dword ptr [esi + 0x70], edx

// block 0041382d
0041382d  pop        ebx
0041382e  add        esp, 0x18
00413831  ret        
