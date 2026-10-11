#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "mips64/core.h"
#include "mips64/cpu.h"
#include "mips64/memory.h"
#include "mips64/decode.h"
#include "mips64/execute.h"
#include "mips64/jit_compiler_x64.h"

typedef size_t(*fun_t)(size_t, size_t, size_t*);

struct JITState jit_s;

void create(struct JITState* s, size_t size) {
	printf("=====================================TEST FOR JIT-COMPILER======================================\n");
	s->buffer_size = size * 32;

#ifdef _WIN32
	s->buffer = VirtualAlloc(NULL, s->buffer_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else
	s->buffer = mmap(0, s->buffer_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
#endif

	if (s->buffer == (void*)-1) {
		printf("jit mmap error\n");
		return;
	}

	s->info1 = malloc(size * sizeof(uint8_t*));
	s->info2 = malloc(size * sizeof(uint16_t));
}

void destroy(struct JITState* s) {
	free(s->info2);
	free(s->info1);
#ifdef _WIN32
	VirtualFree(s->buffer, 0, MEM_RELEASE);
#else	
	munmap(s->buffer, s->buffer_size);
#endif
}

void jit_begin(struct JITState* s)
{
	s->p.buffer = s->buffer;
	s->p.info1 = s->info1;
	s->p.info2 = s->info2;
	s->p.i1 = 0;
}

void jit_relocate(struct JITState* s)
{
	uint8_t** info1 = s->info1;
	uint16_t* info2 = s->info2;
	struct JITParsing* p = &s->p;

	uint16_t* i = info2;
	while (i < p->info2) {

		int i2o;
		int i2 = i[0];
		int32_t* o = (int32_t*)(info1[i2] - 4);
		i2o = info1[i2 + *o] - info1[i2];
		*o = i2o;
		i++;
	}
}

void test_x8664(uint8_t* buffer)
{
	RET(buffer)
}

void test_risc_equ(struct JITParsing* p)
{
	// ==
	JIT_BRANCH(p, JIT_A1, JIT_A2, 2);
	JIT_ADDI(p, JIT_RE, JIT_R0, 0);
	JIT_RETURN(p);
	JIT_ADDI(p, JIT_RE, JIT_R0, 1);
	JIT_RETURN(p);

	//JIT_LESS(p, JIT_RE, JIT_A1, JIT_A2);
	//JIT_RETURN(p);

	// == avec jump < 0
	/*
	JIT_JUMP(p, 2);
	JIT_ADDI(p, JIT_RE, JIT_R0, 1);
	JIT_RETURN(p);
	JIT_BRANCH(p, JIT_A1, JIT_A2, -3);
	JIT_ADDI(p, JIT_RE, JIT_R0, 0);
	JIT_RETURN(p);
	*/
}

void test_risc(struct JITParse* p)
{
	// push stack
	JIT_ADDI(p, JIT_SP, JIT_SP, -8);
	// save R3
	JIT_STORE64(p, JIT_SP, 0, JIT_R3);


	// add
	JIT_ADD(p, JIT_R1, JIT_A1, JIT_A2);

	// counter
	JIT_LOAD32(p, JIT_R2, JIT_A3, 0);
	JIT_ADDI(p, JIT_R2, JIT_R2, 1);
	JIT_STORE32(p, JIT_A3, 0, JIT_R2);


	// restore R3
	JIT_LOAD64(p, JIT_R3, JIT_SP, 0);
	// pop stack
	JIT_ADDI(p, JIT_SP, JIT_SP, 8);


	JIT_ADDI(p, JIT_RE, JIT_R1, 0);
	JIT_RETURN(p);
}

void test_risc_float(struct JITParsing* p)
{
	JIT_ADDF(p, JIT_RF0, JIT_RF0, JIT_RF1);
	JIT_RETURN(p);
}

void test_risc_float4(struct JITParsing* p)
{
	JIT_LOADF4(p, JIT_RF0, JIT_A1, 0);
	JIT_LOADF4(p, JIT_RF1, JIT_A1, 16);

	JIT_ADDF4(p, JIT_RF0, JIT_RF0, JIT_RF1);

	JIT_STOREF4(p, JIT_A1, 32, JIT_RF0);
	JIT_RETURN(p);
}

//void test_int(void)
//{
//	int mem[6] = {3, 7, 9, 4, 3, 6};
//
//	jit_begin(&jit_s);
//	test_risc(&jit_s.p);
//	jit_relocate(&jit_s);
//
//	fun_t func = (fun_t)jit_s.buffer;
//
//	for (int i = 0; i < 7; i++) {
//		func(mem[0], mem[1], mem[2]);
//	}
//
//	printf("result = %d", mem[3], mem[4], mem[5], mem[6]);
//}

void test_float(void)
{
	float mem[4] = { 2.2, 10.10, 22.22, 0.1 };
	typedef float (*fun_t)(float, float, float*);
	int i;

	jit_begin(&jit_s);
	test_risc_float(&jit_s.p);
	jit_relocate(&jit_s);

	fun_t func = (fun_t)jit_s.buffer;

	//void *dl = dlopen(0, RTLD_LAZY | RTLD_GLOBAL);
	//void *ptr = dlsym(dl, "sinf");
	//dlclose(dl);

	float d;
	for (i = 0; i < 1000; i++)
		d = func(mem[0], mem[1], mem);
	printf("result = %f\n", d);
}

void test_float4(void)
{
	float mem[12] = {
		2.2, 10.10, 22.22, 0.1,
		0.1,  0.20,  0.3,  0.4,
		0.0,  0.0,  0.0,  0.0
	};
	typedef void (*fun_t)(float*);
	int i;

	jit_begin(&jit_s);
	test_risc_float4(&jit_s.p);
	jit_relocate(&jit_s);

	fun_t func = (fun_t)jit_s.buffer;

	for (i = 0; i < 1000; i++)
		func(mem);
	printf("result = %f %f %f %f\n", mem[8], mem[9], mem[10], mem[11]);
}

void test_jit(void)
{
	/*test_int();*/
	test_float();
	test_float4();
}


void ctoy_begin(void)
{
	create(&jit_s, 256);

	test_jit();

	destroy(&jit_s);
}

int main(void) {
	// test_jit();
	ctoy_begin();
	return 0;
}