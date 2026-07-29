#include <stddef.h>
#include <stdint.h>
#include <string>

#include "testevents.h"

#define SH_SYSV_NOINLINE __attribute__((noinline))

struct SourceHookTestFloat3
{
	float x;
	float y;
	float z;
};

extern "C" SH_SYSV_NOINLINE uint64_t SourceHookTestSysVMixed(
	uint64_t g0, double s0,
	uint64_t g1, double s1,
	uint64_t g2, double s2,
	uint64_t g3, double s3,
	uint64_t g4, double s4,
	uint64_t g5, double s5,
	double s6, double s7,
	uint64_t g6, double s8)
{
	uint64_t result = 0;
	result |= static_cast<uint64_t>(g0 == 0x101) << 0;
	result |= static_cast<uint64_t>(g1 == 0x202) << 1;
	result |= static_cast<uint64_t>(g2 == 0x303) << 2;
	result |= static_cast<uint64_t>(g3 == 0x404) << 3;
	result |= static_cast<uint64_t>(g4 == 0x505) << 4;
	result |= static_cast<uint64_t>(g5 == 0x606) << 5;
	result |= static_cast<uint64_t>(g6 == 0x707) << 6;
	result |= static_cast<uint64_t>(s0 == 0.5) << 7;
	result |= static_cast<uint64_t>(s1 == 1.5) << 8;
	result |= static_cast<uint64_t>(s2 == 2.5) << 9;
	result |= static_cast<uint64_t>(s3 == 3.5) << 10;
	result |= static_cast<uint64_t>(s4 == 4.5) << 11;
	result |= static_cast<uint64_t>(s5 == 5.5) << 12;
	result |= static_cast<uint64_t>(s6 == 6.5) << 13;
	result |= static_cast<uint64_t>(s7 == 7.5) << 14;
	result |= static_cast<uint64_t>(s8 == 8.5) << 15;
	return result;
}

extern "C" SH_SYSV_NOINLINE uint64_t SourceHookTestSysVGprSpill(
	uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
	uint64_t a4, uint64_t a5, uint64_t a6)
{
	return a0 + 2 * a1 + 3 * a2 + 4 * a3 + 5 * a4 + 6 * a5 + 7 * a6;
}

extern "C" SH_SYSV_NOINLINE double SourceHookTestSysVSseSpill(
	double a0, double a1, double a2, double a3, double a4,
	double a5, double a6, double a7, double a8)
{
	return a0 + 2 * a1 + 3 * a2 + 4 * a3 + 5 * a4
		+ 6 * a5 + 7 * a6 + 8 * a7 + 9 * a8;
}

extern "C" SH_SYSV_NOINLINE int64_t SourceHookTestSysVScalarReturn(int64_t value)
{
	return value ^ INT64_C(0x123456789abcdef0);
}

extern "C" SH_SYSV_NOINLINE void *SourceHookTestSysVPointerReturn(void *value)
{
	return static_cast<unsigned char *>(value) + 3;
}

extern "C" SH_SYSV_NOINLINE float SourceHookTestSysVFloatReturn(float value)
{
	return value * 2.0f + 0.5f;
}

extern "C" SH_SYSV_NOINLINE double SourceHookTestSysVDoubleReturn(double value)
{
	return value * 4.0 - 0.5;
}

extern "C" SH_SYSV_NOINLINE SourceHookTestFloat3 SourceHookTestSysVFloat3Return(
	float x, float y, float z)
{
	SourceHookTestFloat3 result = {x + 1.0f, y + 2.0f, z + 4.0f};
	return result;
}

extern "C" SH_SYSV_NOINLINE uint64_t SourceHookTestSysVNestedLeaf(uint64_t value)
{
	return value * 3 + 1;
}

extern "C" SH_SYSV_NOINLINE uint64_t SourceHookTestSysVNestedMiddle(uint64_t value)
{
	uint64_t result = SourceHookTestSysVNestedLeaf(value + 5);
	return result ^ UINT64_C(0x55aa55aa55aa55aa);
}

extern "C" SH_SYSV_NOINLINE uint64_t SourceHookTestSysVNestedOuter(uint64_t value)
{
	uint64_t result = SourceHookTestSysVNestedMiddle(value * 2);
	return result + value;
}

extern "C" uintptr_t SourceHookTestSysVStackAlignment();
extern "C" uint64_t SourceHookTestSysVPreservesCalleeSaved(void (*target)());

extern "C" SH_SYSV_NOINLINE uintptr_t SourceHookTestSysVNestedAlignment(
	uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
	uint64_t a4, uint64_t a5, uint64_t a6)
{
	uintptr_t result = SourceHookTestSysVStackAlignment();
	return result | static_cast<uintptr_t>((a0 + a1 + a2 + a3 + a4 + a5 + a6) != 28) << 4;
}

volatile uint64_t g_SourceHookTestSysVPressure;

extern "C" SH_SYSV_NOINLINE void SourceHookTestSysVCalleeSavedPressure()
{
	g_SourceHookTestSysVPressure = SourceHookTestSysVNestedOuter(UINT64_C(0x12345678));
}

__asm__(
	".text\n"
	".type SourceHookTestSysVStackAlignment,@function\n"
	"SourceHookTestSysVStackAlignment:\n"
	"movq %rsp, %rax\n"
	"andq $15, %rax\n"
	"ret\n"
	".size SourceHookTestSysVStackAlignment,.-SourceHookTestSysVStackAlignment\n"
	".type SourceHookTestSysVPreservesCalleeSaved,@function\n"
	"SourceHookTestSysVPreservesCalleeSaved:\n"
	"pushq %rbx\n"
	"pushq %rbp\n"
	"pushq %r12\n"
	"pushq %r13\n"
	"pushq %r14\n"
	"pushq %r15\n"
	"subq $8, %rsp\n"
	"movq %rdi, (%rsp)\n"
	"movabsq $0x1122334455667788, %rbx\n"
	"movabsq $0x2233445566778899, %rbp\n"
	"movabsq $0x33445566778899aa, %r12\n"
	"movabsq $0x445566778899aabb, %r13\n"
	"movabsq $0x5566778899aabbcc, %r14\n"
	"movabsq $0x66778899aabbccdd, %r15\n"
	"call *(%rsp)\n"
	"xorl %eax, %eax\n"
	"movabsq $0x1122334455667788, %rdi\n"
	"cmpq %rdi, %rbx\n"
	"jne .LSourceHookTestSysVCalleeSavedDone\n"
	"movabsq $0x2233445566778899, %rdi\n"
	"cmpq %rdi, %rbp\n"
	"jne .LSourceHookTestSysVCalleeSavedDone\n"
	"movabsq $0x33445566778899aa, %rdi\n"
	"cmpq %rdi, %r12\n"
	"jne .LSourceHookTestSysVCalleeSavedDone\n"
	"movabsq $0x445566778899aabb, %rdi\n"
	"cmpq %rdi, %r13\n"
	"jne .LSourceHookTestSysVCalleeSavedDone\n"
	"movabsq $0x5566778899aabbcc, %rdi\n"
	"cmpq %rdi, %r14\n"
	"jne .LSourceHookTestSysVCalleeSavedDone\n"
	"movabsq $0x66778899aabbccdd, %rdi\n"
	"cmpq %rdi, %r15\n"
	"jne .LSourceHookTestSysVCalleeSavedDone\n"
	"movl $1, %eax\n"
	".LSourceHookTestSysVCalleeSavedDone:\n"
	"addq $8, %rsp\n"
	"popq %r15\n"
	"popq %r14\n"
	"popq %r13\n"
	"popq %r12\n"
	"popq %rbp\n"
	"popq %rbx\n"
	"ret\n"
	".size SourceHookTestSysVPreservesCalleeSaved,.-SourceHookTestSysVPreservesCalleeSaved\n");

bool TestX64SysVAbiOracle(std::string &error)
{
	typedef uint64_t (*MixedFn)(
		uint64_t, double, uint64_t, double, uint64_t, double,
		uint64_t, double, uint64_t, double, uint64_t, double,
		double, double, uint64_t, double);
	typedef uint64_t (*GprSpillFn)(
		uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
	typedef double (*SseSpillFn)(
		double, double, double, double, double, double, double, double, double);
	typedef int64_t (*ScalarReturnFn)(int64_t);
	typedef void *(*PointerReturnFn)(void *);
	typedef float (*FloatReturnFn)(float);
	typedef double (*DoubleReturnFn)(double);
	typedef SourceHookTestFloat3 (*Float3ReturnFn)(float, float, float);
	typedef uint64_t (*NestedFn)(uint64_t);
	typedef uintptr_t (*NestedAlignmentFn)(
		uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

	MixedFn volatile mixed = SourceHookTestSysVMixed;
	GprSpillFn volatile gprSpill = SourceHookTestSysVGprSpill;
	SseSpillFn volatile sseSpill = SourceHookTestSysVSseSpill;
	ScalarReturnFn volatile scalarReturn = SourceHookTestSysVScalarReturn;
	PointerReturnFn volatile pointerReturn = SourceHookTestSysVPointerReturn;
	FloatReturnFn volatile floatReturn = SourceHookTestSysVFloatReturn;
	DoubleReturnFn volatile doubleReturn = SourceHookTestSysVDoubleReturn;
	Float3ReturnFn volatile float3Return = SourceHookTestSysVFloat3Return;
	NestedFn volatile nested = SourceHookTestSysVNestedOuter;
	NestedAlignmentFn volatile nestedAlignment = SourceHookTestSysVNestedAlignment;

	CHECK_COND(mixed(
		0x101, 0.5, 0x202, 1.5, 0x303, 2.5, 0x404, 3.5,
		0x505, 4.5, 0x606, 5.5, 6.5, 7.5, 0x707, 8.5) == UINT64_C(0xffff),
		"Independent GPR and SSE cursors");
	CHECK_COND(gprSpill(1, 2, 3, 4, 5, 6, 7) == 140, "First GPR stack spill");
	CHECK_COND(sseSpill(1, 2, 3, 4, 5, 6, 7, 8, 9) == 285.0, "First SSE stack spill");
	CHECK_COND(
		scalarReturn(INT64_C(0x102030405060708)) == INT64_C(0x1336557c9fbad9f8),
		"Scalar return");

	unsigned char pointerStorage[16] = {};
	CHECK_COND(pointerReturn(pointerStorage + 4) == pointerStorage + 7, "Pointer return");
	CHECK_COND(floatReturn(1.25f) == 3.0f, "Float return");
	CHECK_COND(doubleReturn(2.5) == 9.5, "Double return");

	CHECK_COND(sizeof(SourceHookTestFloat3) == 12, "Float3 layout");
	SourceHookTestFloat3 float3 = float3Return(1.0f, 2.0f, 3.0f);
	CHECK_COND(
		float3.x == 2.0f && float3.y == 4.0f && float3.z == 7.0f,
		"Float3 return");

	uint64_t nestedInput = UINT64_C(0x1234);
	uint64_t nestedExpected =
		(((nestedInput * 2 + 5) * 3 + 1) ^ UINT64_C(0x55aa55aa55aa55aa))
		+ nestedInput;
	CHECK_COND(nested(nestedInput) == nestedExpected, "Nested calls");
	CHECK_COND(SourceHookTestSysVStackAlignment() == 8, "Stack alignment");
	CHECK_COND(
		nestedAlignment(1, 2, 3, 4, 5, 6, 7) == 8,
		"Nested stack alignment");
	CHECK_COND(
		SourceHookTestSysVPreservesCalleeSaved(SourceHookTestSysVCalleeSavedPressure) == 1,
		"Callee-saved registers");

	return true;
}
