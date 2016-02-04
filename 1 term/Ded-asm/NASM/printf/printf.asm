;=================================================
; 32-bit printf
; Custom-designed printf
; Avaliable flags:
;	%s - string (+arg)
; 	%c - char (+arg)
;	%b - binary number (+arg)
;	%d - decimal number (+arg)
;	%h - hexadecimal number (+arg)
;	%% - %
;	\n - newline
; 	\t - tab
;	\a - beep
;	\\ - \
; (c) AQ <alexeyqu@gmail.com>, DCAM MIPT, gr. 376
;=================================================

global _start

%macro 	syscall 1-*
	push eax						; store registers
	push ebx
	push ecx
	push edx
	push esi
	push edi
	
	%rep %0							; push args
		%rotate -1
		push dword %1
	%endrep
  
	pop eax

	%if %0 > 1
	  pop ebx

	  %if %0 > 2
	    pop ecx

	    %if %0 > 3
	      pop edx

	      %if %0 > 4
		jmp Error					; too much args
	      %endif

	    %endif
	    
	  %endif

	%endif

	int 80h

	pop edi							; restore registers
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	  
%endmacro

section .data

TestString		db "\aWelcome %s, %s! :%c \nToday is the %d of May (bin = %b, hex = %h) \tDo you like symbols similar to %%? I %c %h\n", 0Ah
TestStringLen		equ $-TestString

str1			db "back$"

str2			db "Giovanni$"

char1			db ')'

numd			dd 10d

numb			dd 1010b

numh			dd 0Ah

percent			db '%'					; percent

char2			db 60

num1			dw 3802

;-------GENERAL_SYMBOLS---------------------------

newline 		db 0Ah

tab			db 9h

beep			db 7h

bslash			db 5Ch

section .bss

digit			resd 1

char			resd 1
  
section .text

_start:
	mov esi, TestString					; load string
	mov edi, TestString + TestStringLen			; and args

	cmp esi, 0h						; if (str == NULL) => Error!
	je Error

	cmp edi, 0h
	je Error

	mov ecx, TestStringLen					; and its length

.nextchar:
	push esi
	push edi
	call PrintChar						; print next character
	add esp, 8h

	inc esi
	
	loop .nextchar

Finish:
	syscall 1, 0						; return 0

Bad_finish:
	syscall 1, 1						; return 1

;=================================================
;-------FUNCTIONS---------------------------------
;=================================================

;=================================================
; PrintChar: 	prints next character considering existing flags
; Entry:	esi -> character, edi -> potential arg
; Exit:		-
; Destr:	World
;=================================================
PrintChar:	
	push ebp
        mov ebp, esp						; making frame

        mov esi, [ebp + 12]					; getting char address
        mov edi, [ebp + 8]

	cmp esi, 0h						; if (&c == NULL) => Error
	je Error

	cmp edi, 0h
	je Error

	mov al, byte [esi]

	cmp al, byte '%'					; is it just a char?
	je .processArgsPC

	cmp al, byte '\'
	je .processSpSymb
	
	syscall 4, 1, esi, 1					; if yes => print it!
	
.exit:	
	mov esp, ebp						; clearing frame
	pop ebp
	
	ret

.processArgsPC:							; '%' switch block
	inc esi
	mov al, byte [esi]

	cmp al, byte 's'
	je .string

	cmp al, byte 'c'
	je .char

	cmp al, byte 'b'
	je .bin

	cmp al, byte 'd'
	je .dec

	cmp al, byte 'h'
	je .hex

	cmp al, byte '%'
	je .percent
	
	jmp Error

.string:
	push ecx
	push edi
	
	call Strlen						; get len of string into ecx

	pop edi

	syscall 4, 1, edi, ecx					; print it!

	add edi, ecx						; next arg
	inc edi

	pop ecx

	jmp .exit

.char:
	syscall 4, 1, edi, 1					; print this char!

	inc edi							; next arg

	jmp .exit

.bin:								; here and in .dec and .hex I use Num_syst converter module
	push ecx
	mov eax, dword [edi]
	mov ebx, 2h

	push eax
	push ebx

	call Convert

	add esp, 8h

	add edi, 4h
	
	pop ecx

	jmp .exit

.dec:
	push ecx
	mov eax, dword [edi]
	mov ebx, 0Ah

	push eax
	push ebx

	call Convert

	add esp, 8h

	add edi, 4h

	pop ecx

	jmp .exit

.hex:
	push ecx
	mov eax, dword [edi]
	mov ebx, 10h

	push eax
	push ebx

	call Convert

	add esp, 8h

	add edi, 4h

	pop ecx

	jmp .exit

.percent:
	syscall 4, 1, percent, 1					; print this char!

	inc edi

	jmp .exit

.processSpSymb:
	inc esi
	mov al, byte [esi]

	cmp al, byte 'n'
	je .newline

	cmp al, byte 't'
	je .tab

	cmp al, byte 'a'
	je .beep

	cmp al, byte '\'
	je .backslash

	jmp Error

.newline:
	syscall 4, 1, newline, 1

	jmp .exit

.tab:
	syscall 4, 1, tab, 1

	jmp .exit

.beep:
	syscall 4, 1, beep, 1					; beep!						; next arg

	jmp .exit

.backslash:
	syscall 4, 1, bslash, 1

	jmp .exit

;================================================
; Strlen: 	Calculates string length
; Entry: 	edi -> string
; Exit: 	ecx = length
; Destr: 	al
;================================================
Strlen:
	push ebp
        mov ebp, esp						; making frame

	mov edi, [ebp + 8]					; getting args

	mov ecx, 0h						; ecx = 0

	cmp edi, 0h						; if (!ebx) => Error!
	je Error

	mov al, byte "$"

.next:
	scasb							; compare symbol with al, di++
	je .exit

	loop .next 						; jmp to Next, ecx--

.exit:
	neg ecx							; ecx *= -1

	mov esp, ebp						; clearing frame
	pop ebp

	ret

;================================================
; Convert: 	converts to another NS
; Entry:   	ecx = number, ebx = base
; Exit:    	stack = result -> prints
; Destr:   	eax, ebx, ecx, edx
;================================================
Convert:
	push ebp
        mov ebp, esp						; making frame

        mov ecx, [ebp + 12]
        mov ebx, [ebp + 8]

	mov eax, '$'						; stop symbol
	push eax

.convertNextDigit:
	mov eax, ecx

	div bl                  				; then divide, al = cx/bx, ah = cx mod bx

	mov ecx, 0h
	mov cl, al

	mov edx, 0h
	mov dl, ah

	push edx                 				; CX mod BX

	cmp cl, 0               				; if CX was less than BX => one more digit left
	jne .convertNextDigit

.printNextDigit:
	pop eax							; pop digit

	cmp eax, '$'             				; check stop symbol
	je .finish

	cmp eax, 9               				; if it's a digit
	ja .itsachar

	add eax, '0'						; To ASCII digit

	mov [digit], eax
	syscall 4, 1, digit, 4

	jmp .printNextDigit

.finish:

	mov esp, ebp						; clearing frame
	pop ebp

	ret

.itsachar:
	add eax, 'A'            				; convert to ASCII char digit A-F
	sub eax, 10   						; 10 -> 10 - 10 + 'A' = 'A'

	mov [char], eax
	syscall 4, 1, char, 4

	jmp .printNextDigit
	
;=================================================
;-------LABLES------------------------------------
;=================================================

Error:
	syscall 4, 1, ErrorMsg, ErrorMsgLen			; print error message

	jmp Bad_finish

section .data

ErrorMsg		db 0Ah, "Oops! An error occured!", 0Ah
ErrorMsgLen		equ $ - ErrorMsg
