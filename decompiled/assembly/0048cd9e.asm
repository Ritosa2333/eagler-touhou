
// block 0048cd9e
0048cd9e  push       0xc
0048cda0  push       0x4ab0a0
0048cda5  call       0x46fcec

// block 0048cdaa
0048cdaa  xor        edi, edi
0048cdac  mov        dword ptr [ebp - 0x1c], edi
0048cdaf  xor        bl, bl
0048cdb1  test       byte ptr [ebp + 0xc], 8
0048cdb5  je         0x48cdba

// block 0048cdb7
0048cdb7  add        bl, 0x20

// block 0048cdba
0048cdba  test       dword ptr [ebp + 0xc], 0x4000
0048cdc1  je         0x48cdc6

// block 0048cdc3
0048cdc3  or         bl, 0x80

// block 0048cdc6
0048cdc6  test       byte ptr [ebp + 0xc], 0x80
0048cdca  je         0x48cdcf

// block 0048cdcc
0048cdcc  or         bl, 0x10

// block 0048cdcf
0048cdcf  push       dword ptr [ebp + 8]
0048cdd2  call       dword ptr [0x498114]

// block 0048cdd8
0048cdd8  cmp        eax, edi
0048cdda  jne        0x48cdf2

// block 0048cddc
0048cddc  call       dword ptr [0x4980e4]

// block 0048cde2
0048cde2  push       eax
0048cde3  call       0x46e995

// block 0048cde8
0048cde8  pop        ecx

// block 0048cde9
0048cde9  or         eax, 0xffffffff

// block 0048cdec
0048cdec  call       0x46fd31

// block 0048cdf1
0048cdf1  ret        

// block 0048cdf2
0048cdf2  cmp        eax, 2
0048cdf5  jne        0x48cdfc

// block 0048cdf7
0048cdf7  or         bl, 0x40
0048cdfa  jmp        0x48ce04

// block 0048cdfc
0048cdfc  cmp        eax, 3
0048cdff  jne        0x48ce04

// block 0048ce01
0048ce01  or         bl, 8

// block 0048ce04
0048ce04  call       0x48cc04

// block 0048ce09
0048ce09  mov        esi, eax
0048ce0b  mov        dword ptr [ebp + 0xc], esi
0048ce0e  cmp        esi, -1
0048ce11  jne        0x48ce27

// block 0048ce13
0048ce13  call       0x46e96f

// block 0048ce18
0048ce18  mov        dword ptr [eax], 0x18
0048ce1e  call       0x46e982

// block 0048ce23
0048ce23  mov        dword ptr [eax], edi
0048ce25  jmp        0x48cde9

// block 0048ce27
0048ce27  mov        dword ptr [ebp - 4], edi
0048ce2a  push       dword ptr [ebp + 8]
0048ce2d  push       esi
0048ce2e  call       0x48c9bf

// block 0048ce33
0048ce33  pop        ecx
0048ce34  pop        ecx
0048ce35  or         bl, 1
0048ce38  mov        eax, esi
0048ce3a  sar        eax, 5
0048ce3d  lea        ecx, [eax*4 + 0x4d6320]
0048ce44  mov        eax, esi
0048ce46  and        eax, 0x1f
0048ce49  shl        eax, 6
0048ce4c  mov        edx, dword ptr [ecx]
0048ce4e  mov        byte ptr [edx + eax + 4], bl
0048ce52  mov        edx, dword ptr [ecx]
0048ce54  lea        edx, [edx + eax + 0x24]
0048ce58  and        byte ptr [edx], 0x80
0048ce5b  mov        ecx, dword ptr [ecx]
0048ce5d  lea        eax, [ecx + eax + 0x24]
0048ce61  and        byte ptr [eax], 0x7f
0048ce64  mov        dword ptr [ebp - 0x1c], 1
0048ce6b  mov        dword ptr [ebp - 4], 0xfffffffe
0048ce72  call       0x48ce8c

// block 0048ce77
0048ce77  cmp        dword ptr [ebp - 0x1c], edi
0048ce7a  je         0x48cde9

// block 0048ce80
0048ce80  mov        eax, esi
0048ce82  jmp        0x48cdec
