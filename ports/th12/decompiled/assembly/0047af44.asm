
// block 0047af44
0047af44  mov        edi, edi
0047af46  push       ebp
0047af47  mov        ebp, esp
0047af49  mov        eax, dword ptr [0x4b42e8]
0047af4e  sub        esp, 0xc
0047af51  push       ebx
0047af52  push       esi
0047af53  mov        esi, dword ptr [0x49811c]
0047af59  push       edi
0047af5a  xor        ebx, ebx
0047af5c  xor        edi, edi
0047af5e  cmp        eax, ebx
0047af60  jne        0x47af90

// block 0047af62
0047af62  call       esi

// block 0047af64
0047af64  mov        edi, eax
0047af66  cmp        edi, ebx
0047af68  je         0x47af76

// block 0047af6a
0047af6a  mov        dword ptr [0x4b42e8], 1
0047af74  jmp        0x47af99

// block 0047af76
0047af76  call       dword ptr [0x4980e4]

// block 0047af7c
0047af7c  cmp        eax, 0x78
0047af7f  jne        0x47af8b

// block 0047af81
0047af81  push       2
0047af83  pop        eax
0047af84  mov        dword ptr [0x4b42e8], eax
0047af89  jmp        0x47af90

// block 0047af8b
0047af8b  mov        eax, dword ptr [0x4b42e8]

// block 0047af90
0047af90  cmp        eax, 1
0047af93  jne        0x47b01a

// block 0047af99
0047af99  cmp        edi, ebx
0047af9b  jne        0x47afac

// block 0047af9d
0047af9d  call       esi

// block 0047af9f
0047af9f  mov        edi, eax
0047afa1  cmp        edi, ebx
0047afa3  jne        0x47afac

// block 0047afa5
0047afa5  xor        eax, eax
0047afa7  jmp        0x47b076

// block 0047afac
0047afac  mov        eax, edi
0047afae  cmp        word ptr [edi], bx
0047afb1  je         0x47afc1

// block 0047afb3
0047afb3  inc        eax
0047afb4  inc        eax
0047afb5  cmp        word ptr [eax], bx
0047afb8  jne        0x47afb3

// block 0047afba
0047afba  inc        eax
0047afbb  inc        eax
0047afbc  cmp        word ptr [eax], bx
0047afbf  jne        0x47afb3

// block 0047afc1
0047afc1  mov        esi, dword ptr [0x498134]
0047afc7  push       ebx
0047afc8  push       ebx
0047afc9  push       ebx
0047afca  sub        eax, edi
0047afcc  push       ebx
0047afcd  sar        eax, 1
0047afcf  inc        eax
0047afd0  push       eax
0047afd1  push       edi
0047afd2  push       ebx
0047afd3  push       ebx
0047afd4  mov        dword ptr [ebp - 0xc], eax
0047afd7  call       esi

// block 0047afd9
0047afd9  mov        dword ptr [ebp - 8], eax
0047afdc  cmp        eax, ebx
0047afde  je         0x47b00f

// block 0047afe0
0047afe0  push       eax
0047afe1  call       0x4777ce

// block 0047afe6
0047afe6  pop        ecx
0047afe7  mov        dword ptr [ebp - 4], eax
0047afea  cmp        eax, ebx
0047afec  je         0x47b00f

// block 0047afee
0047afee  push       ebx
0047afef  push       ebx
0047aff0  push       dword ptr [ebp - 8]
0047aff3  push       eax
0047aff4  push       dword ptr [ebp - 0xc]
0047aff7  push       edi
0047aff8  push       ebx
0047aff9  push       ebx
0047affa  call       esi

// block 0047affc
0047affc  test       eax, eax
0047affe  jne        0x47b00c

// block 0047b000
0047b000  push       dword ptr [ebp - 4]
0047b003  call       0x46c941

// block 0047b008
0047b008  pop        ecx
0047b009  mov        dword ptr [ebp - 4], ebx

// block 0047b00c
0047b00c  mov        ebx, dword ptr [ebp - 4]

// block 0047b00f
0047b00f  push       edi
0047b010  call       dword ptr [0x498120]

// block 0047b016
0047b016  mov        eax, ebx
0047b018  jmp        0x47b076

// block 0047b01a
0047b01a  cmp        eax, 2
0047b01d  je         0x47b023

// block 0047b01f
0047b01f  cmp        eax, ebx
0047b021  jne        0x47afa5

// block 0047b023
0047b023  call       dword ptr [0x498124]

// block 0047b029
0047b029  mov        esi, eax
0047b02b  cmp        esi, ebx
0047b02d  je         0x47afa5

// block 0047b033
0047b033  cmp        byte ptr [esi], bl
0047b035  je         0x47b041

// block 0047b037
0047b037  inc        eax
0047b038  cmp        byte ptr [eax], bl
0047b03a  jne        0x47b037

// block 0047b03c
0047b03c  inc        eax
0047b03d  cmp        byte ptr [eax], bl
0047b03f  jne        0x47b037

// block 0047b041
0047b041  sub        eax, esi
0047b043  inc        eax
0047b044  push       eax
0047b045  mov        dword ptr [ebp - 8], eax
0047b048  call       0x4777ce

// block 0047b04d
0047b04d  mov        edi, eax
0047b04f  pop        ecx
0047b050  cmp        edi, ebx
0047b052  jne        0x47b060

// block 0047b054
0047b054  push       esi
0047b055  call       dword ptr [0x498128]

// block 0047b05b
0047b05b  jmp        0x47afa5

// block 0047b060
0047b060  push       dword ptr [ebp - 8]
0047b063  push       esi
0047b064  push       edi
0047b065  call       0x47a330

// block 0047b06a
0047b06a  add        esp, 0xc
0047b06d  push       esi
0047b06e  call       dword ptr [0x498128]

// block 0047b074
0047b074  mov        eax, edi

// block 0047b076
0047b076  pop        edi
0047b077  pop        esi
0047b078  pop        ebx
0047b079  leave      
0047b07a  ret        
