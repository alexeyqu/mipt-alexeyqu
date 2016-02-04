global _start

section .text

_start:
	pop esi
	pop edi
	cmp esi, edi
	jae 10
	not esi
	or esi, edi
	and esi, edi
	pop esi
	pop edi
	push eax
	push edx
	
	mov eax, esi
	xor edx, edx
	
	div edi
	
	xor esi, esi
	
	mov esi, eax
	
	pop edx
	pop eax
	push esi
	
	mov eax, 3
	mov ebx, 1
	mov ecx, esp
	mov ecx, ebp
	mov edx, 4
	int 80h
	
	sub esp, 4
	
  push dword 5000
  push dword 1000
  push dword 5001
  push dword 500000
  add esp, 4
  add esp, 8
  sub esp, 8

section .bss

  num resd 1
  
section .text
  mov eax, 3
  mov ebx, 1
  mov ecx, num
  mov edx, 4
  int 80h
  nop
  push eax
  push ebx
  push ecx
  push edx
  nop
  pop eax
  pop ebx
  pop ecx
  pop edx
  nop
  int 80h
  nop
  add ebx, ecx
  sub ebx, edx
  mul ebx
  div ebx
  nop

  mov eax, 1
  mov ebx, 0
  int 80h
