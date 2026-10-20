
// block 004db0fd
004db0fd  loopne     0x4db17d

// block 004db0ff
004db0ff  mov        esp, 0xb7707d0a
004db104  push       0x33fd06e1
004db109  call       0x8f8fd4dd

// block 004db10e
004db10e  or         eax, 0xd577c419

// block 004db113
004db113  and        ch, byte ptr [ecx]
004db115  adc        al, 0xf2
004db117  mov        ebp, 0x953b4b6f
004db11c  not        byte ptr [ebx - 0x7f]
004db11f  xchg       ebp, eax
004db120  not        byte ptr [edi + esi + 0x65]
004db124  pop        edi
004db125  add        dh, ah
004db127  imul       edx, edx, -0x69
004db12a  mov        ebp, 0x774cf9e9
004db12f  fisubr     word ptr [ebx]
004db131  and        byte ptr [edi], ch
004db133  loope      0x4db10f

// block 004db135
004db135  cwde       
004db136  cld        
004db137  sub        dl, byte ptr [eax + 0x6f12aa45]
004db13d  jmp        dword ptr [eax + 0x64]

// block 004db17d
004db17d  cwde       
004db17e  int3       
