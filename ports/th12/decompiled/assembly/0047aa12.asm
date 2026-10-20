
// block 0047aa12
0047aa12  mov        edi, edi
0047aa14  push       ebp
0047aa15  mov        ebp, esp
0047aa17  sub        esp, 0x18
0047aa1a  push       ebx
0047aa1b  push       esi
0047aa1c  push       dword ptr [ebp + 0xc]
0047aa1f  lea        ecx, [ebp - 0x18]
0047aa22  call       0x46d3b4

// block 0047aa27
0047aa27  mov        ebx, dword ptr [ebp + 8]
0047aa2a  mov        esi, 0x100
0047aa2f  cmp        ebx, esi
0047aa31  jae        0x47aa87

// block 0047aa33
0047aa33  mov        ecx, dword ptr [ebp - 0x18]
0047aa36  cmp        dword ptr [ecx + 0xac], 1
0047aa3d  jle        0x47aa53

// block 0047aa3f
0047aa3f  lea        eax, [ebp - 0x18]
0047aa42  push       eax
0047aa43  push       1
0047aa45  push       ebx
0047aa46  call       0x4758eb

// block 0047aa4b
0047aa4b  mov        ecx, dword ptr [ebp - 0x18]
0047aa4e  add        esp, 0xc
0047aa51  jmp        0x47aa60

// block 0047aa53
0047aa53  mov        eax, dword ptr [ecx + 0xc8]
0047aa59  movzx      eax, word ptr [eax + ebx*2]
0047aa5d  and        eax, 1

// block 0047aa60
0047aa60  test       eax, eax
0047aa62  je         0x47aa73

// block 0047aa64
0047aa64  mov        eax, dword ptr [ecx + 0xcc]
0047aa6a  movzx      eax, byte ptr [eax + ebx]
0047aa6e  jmp        0x47ab16

// block 0047aa73
0047aa73  cmp        byte ptr [ebp - 0xc], 0
0047aa77  je         0x47aa80

// block 0047aa79
0047aa79  mov        eax, dword ptr [ebp - 0x10]
0047aa7c  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0047aa80
0047aa80  mov        eax, ebx
0047aa82  jmp        0x47ab23

// block 0047aa87
0047aa87  mov        eax, dword ptr [ebp - 0x18]
0047aa8a  cmp        dword ptr [eax + 0xac], 1
0047aa91  jle        0x47aac4

// block 0047aa93
0047aa93  mov        dword ptr [ebp + 8], ebx
0047aa96  sar        dword ptr [ebp + 8], 8
0047aa9a  lea        eax, [ebp - 0x18]
0047aa9d  push       eax
0047aa9e  mov        eax, dword ptr [ebp + 8]
0047aaa1  and        eax, 0xff
0047aaa6  push       eax
0047aaa7  call       0x47c5a3

// block 0047aaac
0047aaac  pop        ecx
0047aaad  pop        ecx
0047aaae  test       eax, eax
0047aab0  je         0x47aac4

// block 0047aab2
0047aab2  mov        al, byte ptr [ebp + 8]
0047aab5  push       2
0047aab7  mov        byte ptr [ebp - 4], al
0047aaba  mov        byte ptr [ebp - 3], bl
0047aabd  mov        byte ptr [ebp - 2], 0
0047aac1  pop        ecx
0047aac2  jmp        0x47aad9

// block 0047aac4
0047aac4  call       0x46e96f

// block 0047aac9
0047aac9  mov        dword ptr [eax], 0x2a
0047aacf  xor        ecx, ecx
0047aad1  mov        byte ptr [ebp - 4], bl
0047aad4  mov        byte ptr [ebp - 3], 0
0047aad8  inc        ecx

// block 0047aad9
0047aad9  mov        eax, dword ptr [ebp - 0x18]
0047aadc  push       1
0047aade  push       dword ptr [eax + 4]
0047aae1  lea        edx, [ebp - 8]
0047aae4  push       3
0047aae6  push       edx
0047aae7  push       ecx
0047aae8  lea        ecx, [ebp - 4]
0047aaeb  push       ecx
0047aaec  push       esi
0047aaed  push       dword ptr [eax + 0x14]
0047aaf0  lea        eax, [ebp - 0x18]
0047aaf3  push       eax
0047aaf4  call       0x48364d

// block 0047aaf9
0047aaf9  add        esp, 0x24
0047aafc  test       eax, eax
0047aafe  je         0x47aa73

// block 0047ab04
0047ab04  cmp        eax, 1
0047ab07  movzx      eax, byte ptr [ebp - 8]
0047ab0b  je         0x47ab16

// block 0047ab0d
0047ab0d  movzx      ecx, byte ptr [ebp - 7]
0047ab11  shl        eax, 8
0047ab14  or         eax, ecx

// block 0047ab16
0047ab16  cmp        byte ptr [ebp - 0xc], 0
0047ab1a  je         0x47ab23

// block 0047ab1c
0047ab1c  mov        ecx, dword ptr [ebp - 0x10]
0047ab1f  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0047ab23
0047ab23  pop        esi
0047ab24  pop        ebx
0047ab25  leave      
0047ab26  ret        
