#include "mips64/jit_compiler_x64.h"

void JIT_LOAD32(struct JITParsing* p, int d, int s, int o) {
	JIT__INFO(p)

	if (d != JIT_R0 && s != JIT_R0) {
		if (s == JIT_SP) {
			MOV(p->buffer, RAX, s)
			LOAD32(p->buffer, d, RAX, o)
		}
		else {
			LOAD32(p->buffer, d, s, o)
		}
	}
}

void JIT_STORE32(struct JITParsing* p, int d, int o, int s) {
	JIT__INFO(p)

	if (d != JIT_R0 && s != JIT_R0) {
		if (s == JIT_SP) {
			MOV(p->buffer, RAX, s)
			STORE32(p->buffer, d, RAX, o)
		}
		else {
			STORE32(p->buffer, d, s, o)
		}
	}
}

void JIT_LOAD64(struct JITParsing* p, int d, int s, int o) {
	JIT__INFO(p)
	if (d != JIT_R0 && s != JIT_R0) {
		if (s == JIT_SP) {
			MOV(p->buffer, RAX, s)
			LOAD64(p->buffer, d, RAX, o)
		}
		else {
			LOAD64(p->buffer, d, s, o)
		}
	}
}

void JIT_STORE64(struct JITParsing* p, int d, int o, int s) {
	JIT__INFO(p)
	if (d != JIT_R0 && s != JIT_R0) {
		if (s == JIT_SP) {
			MOV(p->buffer, RAX, s)
			STORE64(p->buffer, d, RAX, o)
		}
		else {
			STORE64(p->buffer, d, s, o)
		}
	}
}

void JIT_ADD(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
	if (d != JIT_R0) {
		if (s == JIT_R0 && t == JIT_R0) {
			MOVI32(p->buffer, d, 0)
		}
		else if (s == JIT_R0) {
			if (t != d) MOV(p->buffer, d, t)
		}
		else if (t == JIT_R0) {
			if (s != d) MOV(p->buffer, d, s)
		}
		else {
			if (s != d) MOV(p->buffer, d, s)
				ADD(p->buffer, d, t)
		}
	}
}

void JIT_ADDI(struct JITParsing* p, int d, int s, int i) {
	JIT__INFO(p)
	if (d != JIT_R0) {
		if (s == JIT_R0) {
			MOVI32(p->buffer, d, i)
		}
	}
	else {
		if (s != d) MOV(p->buffer, d, s)
		if (i != 0) MOV(p->buffer, d, i)
	}
}


void JIT_SUB(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)

	if (d != JIT_R0) {
		if (s == JIT_R0 && t == JIT_R0) {
			MOVI32(p->buffer, d, 0)
		}
		else if (s == JIT_R0) {
			if (t != d) MOV(p->buffer, d, t)
		}
		else if (t == JIT_R0) {
			if (s != d) MOV(p->buffer, d, s)
		}
		else {
			if (s != d) MOV(p->buffer, d, s)
				SUB(p->buffer, d, t)
		}
	}
	
}

void JIT_MUL(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
	if (d != JIT_R0) {
		if (s == JIT_R0 || t == JIT_R0) {
			MOVI32(p->buffer, d, 0)
		}
		else {
			if (s != d) MOV(p->buffer, d, s)
			IMUL(p->buffer, d, t)
		}
	}
}

void JIT_DIV(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p);
	if (d != JIT_R0) {
		if (s == JIT_R0 || t == JIT_R0) {
			MOVI32(p->buffer, d, 0)
		}
		else {
			// USE RAX/RDX
			PUSH(p->buffer, RDX)
			MOV(p->buffer, RAX, s)
			CDQ(p->buffer)
			IDIV64(p->buffer, t)
			MOV(p->buffer, d, RAX)
			POP(p->buffer, RDX)
		}
	}
}

void JIT_SHIFTL(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p);
	if (d != JIT_R0) {
		if (s == JIT_R0) {
			MOVI32(p->buffer, d, 0)
		}

		else if (t == JIT_R0) {
			if(s != d)MOVI32(p->buffer, s, t)
		}

		else {
			if(s != d) MOV(p->buffer, d, s)
				PUSH(p->buffer, RCX)
				MOV(p->buffer, RCX, t)
				SAL(p->buffer, d)
				POP(p->buffer, RCX)
		}
	}
}

void JIT_SHIFTR(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p);
	if (d != JIT_R0) {
		if (s == JIT_R0) {
			MOVI32(p->buffer, d, 0)
		}

		else if (t == JIT_R0) {
			if (s != d)MOVI32(p->buffer, s, t)
		}

		else {
			if (s != d) MOV(p->buffer, d, s)
				PUSH(p->buffer, RCX)
				MOV(p->buffer, RCX, t)
				SAR(p->buffer, d)
				POP(p->buffer, RCX)
		}
	}
}

void JIT_SHIFTLI(struct JITParsing* p, int d, int s, int i) {
	JIT__INFO(p);
	if (d != JIT_R0) {
		if (s == JIT_R0) {
			MOVI32(p->buffer, d, 0)
		}

		else if (i == JIT_R0) {
			if (s != d)MOVI32(p->buffer, d, s)
				SALI(p->buffer, d, i)
		}
	}
}

void JIT_SHIFTRI(struct JITParsing* p, int d, int s, int i) {
	JIT__INFO(p);
	if (d != JIT_R0) {
		if (s == JIT_R0) {
			MOVI32(p->buffer, d, 0)
		}

		else if (i == JIT_R0) {
			if (s != d) MOVI32(p->buffer, d, s)
				SARI(p->buffer, d, i)
		}
	}
}

void JIT_AND(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p);
	if (d != JIT_R0) {
		if (s == t) {
			if (s == JIT_R0) MOVI32(p->buffer, d, 0)
			else if (s != d) MOV(p->buffer, d, s)
		}
		else if (s == JIT_R0) {
			if (t != d) MOV(p->buffer, d, t)
		}
		else if (t == JIT_R0) {
			if (s != d) MOV(p->buffer, d, s)
		}
		else {
			if (s != d) MOV(p->buffer, d, s)
				AND(p->buffer, d, t)
		}
	}
}

void JIT_OR(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p);
	if (d != JIT_R0) {
		if (s == t) {
			if (s == JIT_R0) MOVI32(p->buffer, d, 0)
			else if (s != d) MOV(p->buffer, d, s)
		}
		else if (s == JIT_R0) {
			if (t != d) MOV(p->buffer, d, t)
		}
		else if (t == JIT_R0) {
			if (s != d) MOV(p->buffer, d, s)
		}
		else {
			if (s != d) MOV(p->buffer, d, s)
			OR(p->buffer, d, t)
		}
	}
}

void JIT_XOR(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p);
	if (d != JIT_R0) {
		if (s == t) {
			if (s == JIT_R0) MOVI32(p->buffer, d, 0)
			else if (s != d) MOV(p->buffer, d, s)
		}
		else if (s == JIT_R0) {
			if (t != d) MOV(p->buffer, d, t)
		}
		else if (t == JIT_R0) {
			if (s != d) MOV(p->buffer, d, s)
		}
		else {
			if (s != d) MOV(p->buffer, d, s)
				XOR(p->buffer, d, t)
		}
	}
}

void JIT_LESS(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (d != JIT_R0) {

			if (s == t) {
				MOVI32(p->buffer, d, 0)
			}
			else {
				if (s == JIT_R0)      CMPI8(p->buffer, t, 0)
				else if (t == JIT_R0) CMPI8(p->buffer, s, 0)
				else                  CMP64(p->buffer, s, t)
					SETL(p->buffer, AL)
					MOVZX(p->buffer, RAX, AL)
					MOV(p->buffer, d, RAX)
			}
		}
}

void JIT_JUMP(struct JITParsing* p, int o) {
	JIT__INFO(p)
	if (o != JIT_R0 || o < -1) {
		JMP(p->buffer, o)
		JIT__INFO2(p)
	}
}

void JIT_BRANCH(struct JITParsing* p, int s, int t, int o) {
	if (s == t) {
		JIT_JUMP(p, o);
		return;
	}

	JIT__INFO(p)
		if (o > 0 || o < -1) {

			if (s == JIT_R0)      CMPI8(p->buffer, t, 0)
			else if (t == JIT_R0) CMPI8(p->buffer, s, 0)
			else                  CMP64(p->buffer, s, t)
				SETE(p->buffer, AL)
				MOVZX(p->buffer, RAX, AL)
				JNZ(p->buffer, o)
				JIT__INFO2(p)
		}
}

void JIT_NBRANCH(struct JITParsing* p, int s, int t, int o) {
	JIT__INFO(p)
		if ((o > 0 || o < -1) && (s != t)) {

			if (s == JIT_R0)      CMPI8(p->buffer, t, 0)
			else if (t == JIT_R0) CMPI8(p->buffer, s, 0)
			else                  CMP64(p->buffer, s, t)
				SETE(p->buffer, AL)
				MOVZX(p->buffer, RAX, AL)
				JZ(p->buffer, o)
				JIT__INFO2(p)
		}
}

void JIT_CALL(struct JITParsing* p, int o) {
	JIT__INFO(p)
		if (o > 0 || o < -1) {
			CALLI32(p->buffer, o)
				JIT__INFO2(p)
		}
}

void JIT_CALLEX(struct JITParsing* p, int s) {
	JIT__INFO(p)
		if (s != JIT_R0) CALL64(p->buffer, s)
}

void JIT_RETURN(struct JITParsing* p) {
	JIT__INFO(p)
		RET(p->buffer)
}

void JIT_LOADF(struct JITParsing* p, int d, int s, int o) {
	JIT__INFO(p)
		if (d != JIT_R0 && s != JIT_R0) {

			if (s == JIT_SP) {
				MOV(p->buffer, RAX, s)
					LOADSS(p->buffer, d, RAX, o)
			}
			else {
				LOADSS(p->buffer, d, s, o)
			}
		}
}

void JIT_STOREF(struct JITParsing* p, int d, int o, int s) {
	JIT__INFO(p)
		if (d != JIT_R0 && s != JIT_R0) {

			if (d == JIT_SP) {
				MOV(p->buffer, RAX, d)
					STORESS(p->buffer, RAX, o, s)
			}
			else {
				STORESS(p->buffer, d, o, s)
			}
		}
}

void JIT_ITOF(struct JITParsing* p, int d, int s) {
	JIT__INFO(p)
		PXOR(p->buffer, d, d)
		if (s != JIT_R0) CVTSI2SS(p->buffer, d, s)
}

void JIT_FTOI(struct JITParsing* p, int d, int s) {
	JIT__INFO(p)
		if(d != JIT_R0) CVTSS2SI(p->buffer, d, s)
}

void JIT_ADDF(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVSS(p->buffer, d, s)
			ADDSS(p->buffer, d, t)
}

void JIT_SUBF(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVSS(p->buffer, d, s)
			SUBSS(p->buffer, d, t)
}

void JIT_MULF(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVSS(p->buffer, d, s)
			MULSS(p->buffer, d, t)
}

void JIT_DIVF(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVSS(p->buffer, d, s)
			DIVSS(p->buffer, d, t)
}

void JIT_LESSF(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		UCOMISS(p->buffer, s, t)
		SETA(p->buffer, AL)
		MOVZX(p->buffer, RAX, AL)
		MOV(p->buffer, d, RAX)
}

void JIT_MINF(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVSS(p->buffer, d, s)
			MINSS(p->buffer, d, t)
}

void JIT_MAXF(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVSS(p->buffer, d, s)
			MAXSS(p->buffer, d, t)
}

void JIT_LOADF4(struct JITParsing* p, int d, int s, int o) {
	JIT__INFO(p)
		if (d != JIT_R0 && s != JIT_R0) {

			if (s == JIT_SP) {
				MOV(p->buffer, RAX, s)
					LOADUPS(p->buffer, d, RAX, o)
			}
			else {
				LOADUPS(p->buffer, d, s, o)
			}
		}
}

void JIT_STOREF4(struct JITParsing* p, int d, int o, int s) {
	JIT__INFO(p)
		if (d != JIT_R0) {

			if (d == JIT_SP) {
				MOV(p->buffer, RAX, d)
					STOREUPS(p->buffer, RAX, o, s)
			}
			else {
				STOREUPS(p->buffer, d, o, s)
			}
		}
}

void JIT_ADDF4(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVUPS(p->buffer, d, s)
			ADDPS(p->buffer, d, t)
}


void JIT_SUBF4(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVUPS(p->buffer, d, s)
			SUBPS(p->buffer, d, t)
}

void JIT_MULF4(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVUPS(p->buffer, d, s)
			MULPS(p->buffer, d, t)
}

void JIT_DIVF4(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVUPS(p->buffer, d, s)
			DIVPS(p->buffer, d, t)
}

void JIT_MINF4(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVUPS(p->buffer, d, s)
			MINPS(p->buffer, d, t)
}

void JIT_MAXF4(struct JITParsing* p, int d, int s, int t) {
	JIT__INFO(p)
		if (s != d) MOVUPS(p->buffer, d, s)
			MAXPS(p->buffer, d, t)
}