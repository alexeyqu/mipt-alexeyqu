global _start

section .data

version db "1"

section .text

_start:
	 mov eax, 4
	 mov ebx, 1
	 mov ecx, version
	 mov edx, 4
	 int 80h

