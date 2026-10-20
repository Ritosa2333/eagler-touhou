
// block 0046edf8
0046edf8  mov        edi, edi
0046edfa  push       ebp
0046edfb  mov        ebp, esp
0046edfd  sub        esp, 0x10
0046ee00  mov        ecx, dword ptr [ebp + 8]
0046ee03  mov        eax, dword ptr [ecx + 0x10]
0046ee06  push       esi
0046ee07  mov        esi, dword ptr [ebp + 0xc]
0046ee0a  push       edi
0046ee0b  mov        edi, esi
0046ee0d  sub        edi, dword ptr [ecx + 0xc]
0046ee10  add        esi, -4
0046ee13  shr        edi, 0xf
0046ee16  mov        ecx, edi
0046ee18  imul       ecx, ecx, 0x204
0046ee1e  lea        ecx, [ecx + eax + 0x144]
0046ee25  mov        dword ptr [ebp - 0x10], ecx
0046ee28  mov        ecx, dword ptr [esi]
0046ee2a  dec        ecx
0046ee2b  mov        dword ptr [ebp - 4], ecx
0046ee2e  test       cl, 1
0046ee31  jne        0x46f10a

// block 0046ee37
0046ee37  push       ebx
0046ee38  lea        ebx, [ecx + esi]
0046ee3b  mov        edx, dword ptr [ebx]
0046ee3d  mov        dword ptr [ebp - 0xc], edx
0046ee40  mov        edx, dword ptr [esi - 4]
0046ee43  mov        dword ptr [ebp - 8], edx
0046ee46  mov        edx, dword ptr [ebp - 0xc]
0046ee49  mov        dword ptr [ebp + 0xc], ebx
0046ee4c  test       dl, 1
0046ee4f  jne        0x46eec5

// block 0046ee51
0046ee51  sar        edx, 4
0046ee54  dec        edx
0046ee55  cmp        edx, 0x3f
0046ee58  jbe        0x46ee5d

// block 0046ee5a
0046ee5a  push       0x3f
0046ee5c  pop        edx

// block 0046ee5d
0046ee5d  mov        ecx, dword ptr [ebx + 4]
0046ee60  cmp        ecx, dword ptr [ebx + 8]
0046ee63  jne        0x46eea7

// block 0046ee65
0046ee65  mov        ebx, 0x80000000
0046ee6a  cmp        edx, 0x20
0046ee6d  jae        0x46ee88

// block 0046ee6f
0046ee6f  mov        ecx, edx
0046ee71  shr        ebx, cl
0046ee73  lea        ecx, [edx + eax + 4]
0046ee77  not        ebx
0046ee79  and        dword ptr [eax + edi*4 + 0x44], ebx
0046ee7d  dec        byte ptr [ecx]
0046ee7f  jne        0x46eea4

// block 0046ee81
0046ee81  mov        ecx, dword ptr [ebp + 8]
0046ee84  and        dword ptr [ecx], ebx
0046ee86  jmp        0x46eea4

// block 0046ee88
0046ee88  lea        ecx, [edx - 0x20]
0046ee8b  shr        ebx, cl
0046ee8d  lea        ecx, [edx + eax + 4]
0046ee91  not        ebx
0046ee93  and        dword ptr [eax + edi*4 + 0xc4], ebx
0046ee9a  dec        byte ptr [ecx]
0046ee9c  jne        0x46eea4

// block 0046ee9e
0046ee9e  mov        ecx, dword ptr [ebp + 8]
0046eea1  and        dword ptr [ecx + 4], ebx

// block 0046eea4
0046eea4  mov        ebx, dword ptr [ebp + 0xc]

// block 0046eea7
0046eea7  mov        edx, dword ptr [ebx + 8]
0046eeaa  mov        ebx, dword ptr [ebx + 4]
0046eead  mov        ecx, dword ptr [ebp - 4]
0046eeb0  add        ecx, dword ptr [ebp - 0xc]
0046eeb3  mov        dword ptr [edx + 4], ebx
0046eeb6  mov        edx, dword ptr [ebp + 0xc]
0046eeb9  mov        ebx, dword ptr [edx + 4]
0046eebc  mov        edx, dword ptr [edx + 8]
0046eebf  mov        dword ptr [ebx + 8], edx
0046eec2  mov        dword ptr [ebp - 4], ecx

// block 0046eec5
0046eec5  mov        edx, ecx
0046eec7  sar        edx, 4
0046eeca  dec        edx
0046eecb  cmp        edx, 0x3f
0046eece  jbe        0x46eed3

// block 0046eed0
0046eed0  push       0x3f
0046eed2  pop        edx

// block 0046eed3
0046eed3  mov        ebx, dword ptr [ebp - 8]
0046eed6  and        ebx, 1
0046eed9  mov        dword ptr [ebp - 0xc], ebx
0046eedc  jne        0x46ef71

// block 0046eee2
0046eee2  sub        esi, dword ptr [ebp - 8]
0046eee5  mov        ebx, dword ptr [ebp - 8]
0046eee8  sar        ebx, 4
0046eeeb  push       0x3f
0046eeed  mov        dword ptr [ebp + 0xc], esi
0046eef0  dec        ebx
0046eef1  pop        esi
0046eef2  cmp        ebx, esi
0046eef4  jbe        0x46eef8

// block 0046eef6
0046eef6  mov        ebx, esi

// block 0046eef8
0046eef8  add        ecx, dword ptr [ebp - 8]
0046eefb  mov        edx, ecx
0046eefd  sar        edx, 4
0046ef00  dec        edx
0046ef01  mov        dword ptr [ebp - 4], ecx
0046ef04  cmp        edx, esi
0046ef06  jbe        0x46ef0a

// block 0046ef08
0046ef08  mov        edx, esi

// block 0046ef0a
0046ef0a  cmp        ebx, edx
0046ef0c  je         0x46ef6c

// block 0046ef0e
0046ef0e  mov        ecx, dword ptr [ebp + 0xc]
0046ef11  mov        esi, dword ptr [ecx + 4]
0046ef14  cmp        esi, dword ptr [ecx + 8]
0046ef17  jne        0x46ef54

// block 0046ef19
0046ef19  mov        esi, 0x80000000
0046ef1e  cmp        ebx, 0x20
0046ef21  jae        0x46ef3a

// block 0046ef23
0046ef23  mov        ecx, ebx
0046ef25  shr        esi, cl
0046ef27  not        esi
0046ef29  and        dword ptr [eax + edi*4 + 0x44], esi
0046ef2d  dec        byte ptr [ebx + eax + 4]
0046ef31  jne        0x46ef54

// block 0046ef33
0046ef33  mov        ecx, dword ptr [ebp + 8]
0046ef36  and        dword ptr [ecx], esi
0046ef38  jmp        0x46ef54

// block 0046ef3a
0046ef3a  lea        ecx, [ebx - 0x20]
0046ef3d  shr        esi, cl
0046ef3f  not        esi
0046ef41  and        dword ptr [eax + edi*4 + 0xc4], esi
0046ef48  dec        byte ptr [ebx + eax + 4]
0046ef4c  jne        0x46ef54

// block 0046ef4e
0046ef4e  mov        ecx, dword ptr [ebp + 8]
0046ef51  and        dword ptr [ecx + 4], esi

// block 0046ef54
0046ef54  mov        ecx, dword ptr [ebp + 0xc]
0046ef57  mov        esi, dword ptr [ecx + 8]
0046ef5a  mov        ecx, dword ptr [ecx + 4]
0046ef5d  mov        dword ptr [esi + 4], ecx
0046ef60  mov        ecx, dword ptr [ebp + 0xc]
0046ef63  mov        esi, dword ptr [ecx + 4]
0046ef66  mov        ecx, dword ptr [ecx + 8]
0046ef69  mov        dword ptr [esi + 8], ecx

// block 0046ef6c
0046ef6c  mov        esi, dword ptr [ebp + 0xc]
0046ef6f  jmp        0x46ef74

// block 0046ef71
0046ef71  mov        ebx, dword ptr [ebp + 8]

// block 0046ef74
0046ef74  cmp        dword ptr [ebp - 0xc], 0
0046ef78  jne        0x46ef82

// block 0046ef7a
0046ef7a  cmp        ebx, edx
0046ef7c  je         0x46f002

// block 0046ef82
0046ef82  mov        ecx, dword ptr [ebp - 0x10]
0046ef85  lea        ecx, [ecx + edx*8]
0046ef88  mov        ebx, dword ptr [ecx + 4]
0046ef8b  mov        dword ptr [esi + 8], ecx
0046ef8e  mov        dword ptr [esi + 4], ebx
0046ef91  mov        dword ptr [ecx + 4], esi
0046ef94  mov        ecx, dword ptr [esi + 4]
0046ef97  mov        dword ptr [ecx + 8], esi
0046ef9a  mov        ecx, dword ptr [esi + 4]
0046ef9d  cmp        ecx, dword ptr [esi + 8]
0046efa0  jne        0x46f002

// block 0046efa2
0046efa2  mov        cl, byte ptr [edx + eax + 4]
0046efa6  mov        byte ptr [ebp + 0xf], cl
0046efa9  inc        cl
0046efab  mov        byte ptr [edx + eax + 4], cl
0046efaf  cmp        edx, 0x20
0046efb2  jae        0x46efd9

// block 0046efb4
0046efb4  cmp        byte ptr [ebp + 0xf], 0
0046efb8  jne        0x46efc8

// block 0046efba
0046efba  mov        ecx, edx
0046efbc  mov        ebx, 0x80000000
0046efc1  shr        ebx, cl
0046efc3  mov        ecx, dword ptr [ebp + 8]
0046efc6  or         dword ptr [ecx], ebx

// block 0046efc8
0046efc8  mov        ebx, 0x80000000
0046efcd  mov        ecx, edx
0046efcf  shr        ebx, cl
0046efd1  lea        eax, [eax + edi*4 + 0x44]
0046efd5  or         dword ptr [eax], ebx
0046efd7  jmp        0x46f002

// block 0046efd9
0046efd9  cmp        byte ptr [ebp + 0xf], 0
0046efdd  jne        0x46efef

// block 0046efdf
0046efdf  lea        ecx, [edx - 0x20]
0046efe2  mov        ebx, 0x80000000
0046efe7  shr        ebx, cl
0046efe9  mov        ecx, dword ptr [ebp + 8]
0046efec  or         dword ptr [ecx + 4], ebx

// block 0046efef
0046efef  lea        ecx, [edx - 0x20]
0046eff2  mov        edx, 0x80000000
0046eff7  shr        edx, cl
0046eff9  lea        eax, [eax + edi*4 + 0xc4]
0046f000  or         dword ptr [eax], edx

// block 0046f002
0046f002  mov        eax, dword ptr [ebp - 4]
0046f005  mov        dword ptr [esi], eax
0046f007  mov        dword ptr [eax + esi - 4], eax
0046f00b  mov        eax, dword ptr [ebp - 0x10]
0046f00e  dec        dword ptr [eax]
0046f010  jne        0x46f109

// block 0046f016
0046f016  mov        eax, dword ptr [0x4b3d58]
0046f01b  test       eax, eax
0046f01d  je         0x46f0fb

// block 0046f023
0046f023  mov        ecx, dword ptr [0x4d6454]
0046f029  mov        esi, dword ptr [0x49818c]
0046f02f  push       0x4000
0046f034  shl        ecx, 0xf
0046f037  add        ecx, dword ptr [eax + 0xc]
0046f03a  mov        ebx, 0x8000
0046f03f  push       ebx
0046f040  push       ecx
0046f041  call       esi

// block 0046f043
0046f043  mov        ecx, dword ptr [0x4d6454]
0046f049  mov        eax, dword ptr [0x4b3d58]
0046f04e  mov        edx, 0x80000000
0046f053  shr        edx, cl
0046f055  or         dword ptr [eax + 8], edx
0046f058  mov        eax, dword ptr [0x4b3d58]
0046f05d  mov        eax, dword ptr [eax + 0x10]
0046f060  mov        ecx, dword ptr [0x4d6454]
0046f066  and        dword ptr [eax + ecx*4 + 0xc4], 0
0046f06e  mov        eax, dword ptr [0x4b3d58]
0046f073  mov        eax, dword ptr [eax + 0x10]
0046f076  dec        byte ptr [eax + 0x43]
0046f079  mov        eax, dword ptr [0x4b3d58]
0046f07e  mov        ecx, dword ptr [eax + 0x10]
0046f081  cmp        byte ptr [ecx + 0x43], 0
0046f085  jne        0x46f090

// block 0046f087
0046f087  and        dword ptr [eax + 4], 0xfffffffe
0046f08b  mov        eax, dword ptr [0x4b3d58]

// block 0046f090
0046f090  cmp        dword ptr [eax + 8], -1
0046f094  jne        0x46f0fb

// block 0046f096
0046f096  push       ebx
0046f097  push       0
0046f099  push       dword ptr [eax + 0xc]
0046f09c  call       esi

// block 0046f09e
0046f09e  mov        eax, dword ptr [0x4b3d58]
0046f0a3  push       dword ptr [eax + 0x10]
0046f0a6  push       0
0046f0a8  push       dword ptr [0x4b3c04]
0046f0ae  call       dword ptr [0x4981d0]

// block 0046f0b4
0046f0b4  mov        ecx, dword ptr [0x4d6440]
0046f0ba  mov        eax, dword ptr [0x4b3d58]
0046f0bf  imul       ecx, ecx, 0x14
0046f0c2  mov        edx, dword ptr [0x4d6444]
0046f0c8  sub        ecx, eax
0046f0ca  lea        ecx, [ecx + edx - 0x14]
0046f0ce  push       ecx
0046f0cf  lea        ecx, [eax + 0x14]
0046f0d2  push       ecx
0046f0d3  push       eax
0046f0d4  call       0x47a6a0

// block 0046f0d9
0046f0d9  mov        eax, dword ptr [ebp + 8]
0046f0dc  add        esp, 0xc
0046f0df  dec        dword ptr [0x4d6440]
0046f0e5  cmp        eax, dword ptr [0x4b3d58]
0046f0eb  jbe        0x46f0f1

// block 0046f0ed
0046f0ed  sub        dword ptr [ebp + 8], 0x14

// block 0046f0f1
0046f0f1  mov        eax, dword ptr [0x4d6444]
0046f0f6  mov        dword ptr [0x4d644c], eax

// block 0046f0fb
0046f0fb  mov        eax, dword ptr [ebp + 8]
0046f0fe  mov        dword ptr [0x4b3d58], eax
0046f103  mov        dword ptr [0x4d6454], edi

// block 0046f109
0046f109  pop        ebx

// block 0046f10a
0046f10a  pop        edi
0046f10b  pop        esi
0046f10c  leave      
0046f10d  ret        
