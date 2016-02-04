.model small

;================================================
; Numeral system converter
; Input base: 2..16
; Output base: 2..16
; Number < 256d
; ------------
; Algorithm:
; 1) Enter base of input number ( 2..16 ) -> BX
; 2) Enter number char-by-char into stack -> CX (hex)
; 3) Enter base of output number ( 2..16 ) -> BX
; 4) Print result -> stack -> screen
;-----------------------------------------------
; (c) AQ <alexeyqu@gmail.com>, DCAM MIPT, gr. 376
;================================================

.code

.386

org 100h

Main:	
            	mov ax, 0900h           ; print string
		mov dx, offset WlcmMsg	; print welcome message 
		int 21h

		mov dx, offset FStBaseMsg	
		int 21h

		call GetBase		; get base of first number

		mov ax, 0900h
		mov dx, offset NumMsg	
		int 21h
                           
		call GetNum		; get number to convert

		mov ax, 0900h
		mov dx, offset SNdBaseMsg	
		int 21h
                          
		call GetBase		; get outbase
                           
		call Convert		; convert to outbase numeral system
                        		
Finish:	        
		call Getchar       

		mov ax, 4C00h           ; return 0
		int 21h 

BadFinish:	
		call Getchar
		
		mov ax, 4C01h		; return 1
		int 21h

;================================================
;-------------------FUNCTIONS--------------------
;================================================

;================================================
; GetDigit: reads one digit
; Entry:
; Exit:     DX = digit
; Destr:    AX
;================================================
GetDigit	proc

                mov ax, 0100h		; al = getchar()
		int 21h

		cmp al, 0Dh		; if (al == ' ') => Error in base, end of number in number
		je GetDigitResult

		cmp al, '0'          	; if (al < '0') => Error
		jb ErrorDigit

		cmp al, 'G'             ; if (al > 'G') => Error
		ja ErrorDigit

		cmp al, '9'   		; if (al <= '9') => Base = 2..9
		ja GetDigitChar         ; if (al >  '9') => Base may be correct, >= 10
		jbe GetDigitNum

GetDigitResult:		
		mov dx, 0h              ; bx = base
		mov dl, al

		ret
		
GetDigitNum:		
		sub al, '0'		; al -= '0' => al = base
		jmp GetDigitResult

GetDigitChar:		
		cmp al, 'A'           	; if (al < 'A') => Base is incorrect ( '9' < al < 'A' )
		jb ErrorDigit

		sub al, 'A'		; al = al + 10 - 'A' => base = 10 = 'A' - 'A' + 10
		add al, 10
		jmp GetDigitResult
		
		ret
endp					; ENDP GetDigit

;================================================
; GetBase: gets number base
; Entry:
; Exit:    BX = base
; Destr:   (DX)
;================================================
GetBase		proc

		call GetDigit           ; dx = digit

		cmp dl, 2               ; base >= 2
		jb ErrorBase

		cmp dl, 0Dh         	; if (dl == '\n') => Error
		je ErrorBase
		
		mov bx, dx		; bx = base = digit

		call Getchar

		ret              
endp     				; ENDP GetBase

;================================================
; GetNum: gets number
; Entry:
; Exit:   CX = number
; Destr:  AX, (DX)
;================================================
GetNum		proc

		mov ax, 00h
		push ax			; during this function result is stored in stack

		call GetDigit		; dl = first digit

		cmp dl, 0Dh             ; if (first digit == ' ') => Error
		je ErrorDigit

GetNextDigit:
		cmp dl, bl    		; digit < base
		jae ErrorDigit

		mov ax, 0000h		; ax = 0
		mov al, bl       	; al = base
		pop cx                  ; 
		push dx			; Well, division stuff, sorry
		mul cx                  ; ax = old_number * base
		pop dx                  ; 
		add al, dl              ; ax += new_digit
		push ax                 ; store ax

		call GetDigit           ; dl = digit

		cmp dl, 0Dh		; if (dl == ' ') => end of number
		jne GetNextDigit	; else => get next digit

		pop Cx                  ; dx = result

;		call Getchar

		ret
endp					; ENDP GetNum

;================================================
; Convert: converts to another NS
; Entry:   CX = number, BX = base
; Exit:    stack = result -> prints
; Destr:   AX, DX
;================================================
Convert		proc
	
		mov ax, '$'		; stop symbol
		push ax

ConvertNextDigit:                                
		mov ax, cx
		
		div bl                  ; then divide, al = cx/bx, ah = cx mod bx

		mov cx, 0000h
		mov cl, al

		mov dx, 0000h
		mov dl, ah

		push dx                 ; CX mod BX

		cmp cl, 0               ; if CX was less than BX => one more digit left
		jne ConvertNextDigit

PrintNextDigit:
		pop ax			; pop digit
		
		cmp ax, '$'             ; check stop symbol
		je ConvertFinish

		cmp ax, 9               ; if it's a digit
		ja Itsachar

		add ax, '0'		; To ASCII digit

		mov dl, al
		mov ah, 02h     	; print char
		int 21h

		jmp PrintNextDigit 

ConvertFinish:	

		ret

Itsachar:	
		add ax, 'A'             ; convert to ASCII char digit A-F
		sub ax, 10   		; 10 -> 10 - 10 + 'A' = 'A'

		mov dl, al
		mov ah, 02h     	; print char
		int 21h

		jmp PrintNextDigit 
          
endp

;================================================
;-----------------MISCELLANEOUS------------------
;================================================

;================================================
; Getchar: getchar :)
; Entry:
; Exit:
; Destr:   AX
;================================================
Getchar		proc

		mov ax, 0100h		; getchar()
		int 21h

		ret
endp

;================================================
;--------------------LABLES----------------------
;================================================

ErrorDigit:
     		mov ax, 0900h
		mov dx, offset ErrorDigitMsg
		int 21h

		jmp BadFinish

ErrorBase:
     		mov ax, 0900h
		mov dx, offset ErrorBaseMsg
		int 21h

		jmp BadFinish	

;================================================
;-------------------MESSAGES---------------------
;================================================
                           
ErrorDigitMsg:	db 0Dh, 0Ah, "An invalid digit was entered!$" 

ErrorBaseMsg:	db 0Dh, 0Ah, "An invalid base was entered!$"

WlcmMsg:	db "Hello! I am a program that can convert numbers from one nymeral system to another.", 0Dh, 0Ah, "(c) AQ", 0Dh, 0Ah, "$"

FStBaseMsg:	db 0Dh, 0Ah, "Enter base of input number (2..G): $"

NumMsg:		db 0Dh, 0Ah, "Enter number to convert (1..255): $"

SNdBaseMsg:	db 0Dh, 0Ah, "Enter base of desired result (2..G): $"

ResultMsg:	db 0Dh, 0Ah, "Result: $" 

end Main