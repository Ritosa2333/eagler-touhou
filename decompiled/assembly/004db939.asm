
// block 004db939
004db939  push       ds
004db93a  inc        esi
004db93b  sub        ebx, esp
004db93d  push       esi
004db93e  cmp        eax, 0x8459689f
004db943  test       dword ptr [edx], esp
004db945  add        esi, esp
004db947  popfd      
004db948  inc        ebx
004db949  adc        byte ptr [0xb2296db0], dh
004db94f  inc        esp
004db950  inc        edx
004db951  in         eax, dx
