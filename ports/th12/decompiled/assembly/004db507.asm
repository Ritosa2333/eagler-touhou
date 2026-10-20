
// block 004db49f
004db49f  je         0x4db475

// block 004db4a1
004db4a1  std        
004db4a3  mov        ebx, 0x492bc1ad
004db4a8  pop        ss
004db4a9  aaa        
004db4aa  sal        byte ptr [0x9f1b821e], cl
004db4b0  xchg       byte ptr [edi - 0x64], dh
004db4b3  dec        ebp
004db4b4  mov        esi, 0x557b6965

// block 004db507

// block 004db508
004db508  cmp        ch, bh
004db50a  ja         0x4db490

// block 004db50c
004db50c  test       al, 0x59
004db50e  inc        esi

// block 004db510

// block 004db515
004db515  fnstsw     word ptr [eax - 0x1354d7a8]
004db51b  ja         0x4db49f

// block 004db51d
004db51d  in         al, dx
004db51e  sal        dh, cl
004db520  inc        ebp
004db522  sbb        dword ptr [esi + 0x65dae7e7], eax
