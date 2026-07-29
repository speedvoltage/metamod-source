#include <cstddef>
#include <cstdint>
#include <string>

#include "sourcehook.h"
#include "sourcehook_test.h"
#include "testevents.h"
#include "sourcehook_pibuilder.h"

extern "C" std::uintptr_t SourceHookTestHookManX64StackAlignment();
extern "C" void SourceHookTestHookManX64InvokeVoid(void *object);
extern "C" std::uint64_t SourceHookTestHookManX64InvokeIntegers(
	void *object,
	std::uint8_t a0,
	std::uint16_t a1,
	std::uint32_t a2,
	std::uint64_t a3,
	std::uint64_t &a4,
	double &a5,
	void *a6);
extern "C" double SourceHookTestHookManX64InvokeDoubles(
	void *object,
	double a0,
	double a1,
	double a2,
	double a3,
	double a4,
	double a5,
	double a6,
	double a7,
	double a8);
extern "C" float SourceHookTestHookManX64InvokeFloat(
	void *object,
	float value);
extern "C" std::uint64_t SourceHookTestHookManX64InvokeMixed(
	void *object,
	std::uint64_t g0,
	double s0,
	std::uint64_t g1,
	double s1,
	std::uint64_t g2,
	double s2,
	std::uint64_t g3,
	double s3,
	std::uint64_t g4,
	double s4,
	std::uint64_t g5,
	double s5,
	double s6,
	double s7,
	std::uint64_t g6,
	double s8);
extern "C" std::uint64_t SourceHookTestHookManX64InvokeScalar(
	void *object,
	std::uint64_t value);
extern "C" std::uint64_t *SourceHookTestHookManX64InvokeReference(
	void *object,
	std::uint64_t selector);
extern "C" std::uint64_t SourceHookTestHookManX64InvokeReentrant(
	void *object,
	std::uint64_t depth);
extern "C" std::uint64_t SourceHookTestHookManX64PreservesCalleeSaved(
	void (*target)());

#define SH_TEST_X64_INVOKER(name) \
	".type " #name ",@function\n" \
	#name ":\n" \
	"movq (%rdi), %rax\n" \
	"jmp *(%rax)\n" \
	".size " #name ",.-" #name "\n"

__asm__(
	".text\n"
	".type SourceHookTestHookManX64StackAlignment,@function\n"
	"SourceHookTestHookManX64StackAlignment:\n"
	"movq %rsp, %rax\n"
	"andq $15, %rax\n"
	"ret\n"
	".size SourceHookTestHookManX64StackAlignment,.-SourceHookTestHookManX64StackAlignment\n"
	SH_TEST_X64_INVOKER(SourceHookTestHookManX64InvokeVoid)
	SH_TEST_X64_INVOKER(SourceHookTestHookManX64InvokeIntegers)
	SH_TEST_X64_INVOKER(SourceHookTestHookManX64InvokeDoubles)
	SH_TEST_X64_INVOKER(SourceHookTestHookManX64InvokeFloat)
	SH_TEST_X64_INVOKER(SourceHookTestHookManX64InvokeMixed)
	SH_TEST_X64_INVOKER(SourceHookTestHookManX64InvokeScalar)
	SH_TEST_X64_INVOKER(SourceHookTestHookManX64InvokeReference)
	SH_TEST_X64_INVOKER(SourceHookTestHookManX64InvokeReentrant)
	".type SourceHookTestHookManX64PreservesCalleeSaved,@function\n"
	"SourceHookTestHookManX64PreservesCalleeSaved:\n"
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
	"jne .LSourceHookTestHookManX64CalleeSavedDone\n"
	"movabsq $0x2233445566778899, %rdi\n"
	"cmpq %rdi, %rbp\n"
	"jne .LSourceHookTestHookManX64CalleeSavedDone\n"
	"movabsq $0x33445566778899aa, %rdi\n"
	"cmpq %rdi, %r12\n"
	"jne .LSourceHookTestHookManX64CalleeSavedDone\n"
	"movabsq $0x445566778899aabb, %rdi\n"
	"cmpq %rdi, %r13\n"
	"jne .LSourceHookTestHookManX64CalleeSavedDone\n"
	"movabsq $0x5566778899aabbcc, %rdi\n"
	"cmpq %rdi, %r14\n"
	"jne .LSourceHookTestHookManX64CalleeSavedDone\n"
	"movabsq $0x66778899aabbccdd, %rdi\n"
	"cmpq %rdi, %r15\n"
	"jne .LSourceHookTestHookManX64CalleeSavedDone\n"
	"movl $1, %eax\n"
	".LSourceHookTestHookManX64CalleeSavedDone:\n"
	"addq $8, %rsp\n"
	"popq %r15\n"
	"popq %r14\n"
	"popq %r13\n"
	"popq %r12\n"
	"popq %rbp\n"
	"popq %rbx\n"
	"ret\n"
	".size SourceHookTestHookManX64PreservesCalleeSaved,.-SourceHookTestHookManX64PreservesCalleeSaved\n");

#undef SH_TEST_X64_INVOKER

namespace
{
	SourceHook::ISourceHook *g_SHPtr;
	SourceHook::Plugin g_PLID;
	SourceHook::IHookManagerAutoGen *g_HMAGPtr;

	const std::uint64_t kCanaryLow = UINT64_C(0x13579bdf2468ace0);
	const std::uint64_t kCanaryHigh = UINT64_C(0xfedcba9876543210);

	bool FrameHealthy()
	{
		volatile std::uint64_t low = kCanaryLow;
		volatile std::uint64_t high = kCanaryHigh;
		bool aligned = SourceHookTestHookManX64StackAlignment() == 8;
		return aligned && low == kCanaryLow && high == kCanaryHigh;
	}

	bool CallerCanaries(
		volatile std::uint64_t &low,
		volatile std::uint64_t &high)
	{
		return low == kCanaryLow && high == kCanaryHigh;
	}

	void AddBasic(
		SourceHook::CProtoInfoBuilder &proto,
		std::size_t size,
		unsigned int flags = SourceHook::PassInfo::PassFlag_ByVal)
	{
		proto.AddParam(
			size,
			SourceHook::PassInfo::PassType_Basic,
			flags,
			nullptr,
			nullptr,
			nullptr,
			nullptr);
	}

	void AddFloat(
		SourceHook::CProtoInfoBuilder &proto,
		std::size_t size,
		unsigned int flags = SourceHook::PassInfo::PassFlag_ByVal)
	{
		proto.AddParam(
			size,
			SourceHook::PassInfo::PassType_Float,
			flags,
			nullptr,
			nullptr,
			nullptr,
			nullptr);
	}

	void SetBasicReturn(
		SourceHook::CProtoInfoBuilder &proto,
		std::size_t size,
		unsigned int flags = SourceHook::PassInfo::PassFlag_ByVal)
	{
		proto.SetReturnType(
			size,
			SourceHook::PassInfo::PassType_Basic,
			flags,
			nullptr,
			nullptr,
			nullptr,
			nullptr);
	}

	void SetFloatReturn(
		SourceHook::CProtoInfoBuilder &proto,
		std::size_t size)
	{
		proto.SetReturnType(
			size,
			SourceHook::PassInfo::PassType_Float,
			SourceHook::PassInfo::PassFlag_ByVal,
			nullptr,
			nullptr,
			nullptr,
			nullptr);
	}

	template <typename Derived>
	class DelegateRoot : public SourceHook::ISHDelegate
	{
	public:
		bool IsEqual(SourceHook::ISHDelegate *other) override
		{
			return other == this;
		}

		void DeleteThis() override
		{
			delete static_cast<Derived *>(this);
		}
	};

	template <typename Target, typename Delegate>
	int InstallHook(
		Target *target,
		SourceHook::HookManagerPubFunc hookman,
		Delegate *delegate,
		bool post)
	{
		int hook = g_SHPtr->AddHook(
			g_PLID,
			SourceHook::ISourceHook::Hook_Normal,
			reinterpret_cast<void *>(target),
			0,
			hookman,
			delegate,
			post);
		if (hook == 0)
			delegate->DeleteThis();
		return hook;
	}

	bool Rejects(SourceHook::CProtoInfoBuilder &proto)
	{
		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
			return true;
		g_HMAGPtr->ReleaseHookMan(hookman);
		return false;
	}

	bool InstallFailed(
		SourceHook::HookManagerPubFunc hookman,
		std::string &error,
		const char *message)
	{
		Test_CompleteShutdown(g_SHPtr);
		g_HMAGPtr->ReleaseHookMan(hookman);
		error = message;
		return false;
	}

	enum ZeroEvent
	{
		Zero_PreOne,
		Zero_PreTwo,
		Zero_Original,
		Zero_Post
	};

	struct ZeroState
	{
		ZeroEvent events[8];
		int eventCount;
		int originals;
		bool preOneOrder;
		bool preTwoOrder;
		bool postOrder;
		bool healthy;
	};

	ZeroState g_Zero;

	void ResetZero()
	{
		g_Zero.eventCount = 0;
		g_Zero.originals = 0;
		g_Zero.preOneOrder = false;
		g_Zero.preTwoOrder = false;
		g_Zero.postOrder = false;
		g_Zero.healthy = true;
	}

	void RecordZero(ZeroEvent event)
	{
		if (g_Zero.eventCount < 8)
			g_Zero.events[g_Zero.eventCount] = event;
		++g_Zero.eventCount;
	}

	class ZeroTarget
	{
	public:
		virtual void Run()
		{
			RecordZero(Zero_Original);
			++g_Zero.originals;
			g_Zero.healthy = g_Zero.healthy && FrameHealthy();
		}
	};

	class ZeroDelegate final : public DelegateRoot<ZeroDelegate>
	{
	public:
		enum Kind
		{
			PreOne,
			PreTwo,
			Post
		};

		explicit ZeroDelegate(Kind kind)
			: kind_(kind)
		{
		}

		virtual void Call()
		{
			g_Zero.healthy = g_Zero.healthy && FrameHealthy();
			switch (kind_)
			{
			case PreOne:
				RecordZero(Zero_PreOne);
				g_Zero.preOneOrder =
					META_RESULT_PREVIOUS == MRES_IGNORED &&
					META_RESULT_STATUS == MRES_IGNORED;
				RETURN_META(MRES_HANDLED);
			case PreTwo:
				RecordZero(Zero_PreTwo);
				g_Zero.preTwoOrder =
					META_RESULT_PREVIOUS == MRES_HANDLED &&
					META_RESULT_STATUS == MRES_HANDLED;
				RETURN_META(MRES_IGNORED);
			case Post:
				RecordZero(Zero_Post);
				g_Zero.postOrder =
					META_RESULT_PREVIOUS == MRES_IGNORED &&
					META_RESULT_STATUS == MRES_HANDLED;
				RETURN_META(MRES_IGNORED);
			}
		}

	private:
		Kind kind_;
	};

	ZeroTarget *g_PreservedZeroTarget;

	extern "C" void SourceHookTestHookManX64PreservedCall()
	{
		SourceHookTestHookManX64InvokeVoid(g_PreservedZeroTarget);
	}

	bool TestZeroAndPreservation(std::string &error)
	{
		SourceHook::CProtoInfoBuilder proto(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
		{
			error = "Generate zero-parameter void hook manager";
			return false;
		}

		ZeroTarget target;
		int preOne = InstallHook(
			&target,
			hookman,
			new ZeroDelegate(ZeroDelegate::PreOne),
			false);
		int preTwo = InstallHook(
			&target,
			hookman,
			new ZeroDelegate(ZeroDelegate::PreTwo),
			false);
		int post = InstallHook(
			&target,
			hookman,
			new ZeroDelegate(ZeroDelegate::Post),
			true);
		if (preOne == 0 || preTwo == 0 || post == 0)
			return InstallFailed(hookman, error, "Install zero-parameter hooks");

		ResetZero();
		volatile std::uint64_t low = kCanaryLow;
		volatile std::uint64_t high = kCanaryHigh;
		SourceHookTestHookManX64InvokeVoid(&target);
		bool direct =
			g_Zero.eventCount == 4 &&
			g_Zero.events[0] == Zero_PreOne &&
			g_Zero.events[1] == Zero_PreTwo &&
			g_Zero.events[2] == Zero_Original &&
			g_Zero.events[3] == Zero_Post &&
			g_Zero.originals == 1 &&
			g_Zero.preOneOrder &&
			g_Zero.preTwoOrder &&
			g_Zero.postOrder &&
			g_Zero.healthy &&
			CallerCanaries(low, high);

		ResetZero();
		g_PreservedZeroTarget = &target;
		std::uint64_t preserved =
			SourceHookTestHookManX64PreservesCalleeSaved(
				&SourceHookTestHookManX64PreservedCall);
		g_PreservedZeroTarget = nullptr;
		bool preservation =
			preserved == 1 &&
			g_Zero.eventCount == 4 &&
			g_Zero.events[0] == Zero_PreOne &&
			g_Zero.events[1] == Zero_PreTwo &&
			g_Zero.events[2] == Zero_Original &&
			g_Zero.events[3] == Zero_Post &&
			g_Zero.originals == 1 &&
			g_Zero.healthy;

		bool removed =
			g_SHPtr->RemoveHookByID(preOne) &&
			g_SHPtr->RemoveHookByID(preTwo) &&
			g_SHPtr->RemoveHookByID(post);
		if (!removed)
		{
			error = "Remove zero-parameter hooks";
			return false;
		}

		ResetZero();
		SourceHookTestHookManX64InvokeVoid(&target);
		bool unhooked =
			g_Zero.eventCount == 1 &&
			g_Zero.events[0] == Zero_Original &&
			g_Zero.originals == 1 &&
			g_Zero.healthy;

		g_HMAGPtr->ReleaseHookMan(hookman);

		if (!direct)
			error = "Zero-parameter ordering, results, alignment, or canaries";
		else if (!preservation)
			error = "Generated hook callee-saved GPR preservation";
		else if (!unhooked)
			error = "Zero-parameter invocation after removal";
		return direct && preservation && unhooked;
	}

	struct IntegerState
	{
		int hooks;
		int originals;
		bool hookArgs;
		bool originalArgs;
		bool healthy;
		std::uint64_t *integerReference;
		double *floatReference;
		void *pointer;
	};

	const std::uint64_t kIntegerReturn = UINT64_C(0x89abcdef01234567);

	bool IntegerArgsMatch(
		IntegerState &state,
		std::uint8_t a0,
		std::uint16_t a1,
		std::uint32_t a2,
		std::uint64_t a3,
		std::uint64_t &a4,
		double &a5,
		void *a6)
	{
		return
			a0 == UINT8_C(0x12) &&
			a1 == UINT16_C(0x2345) &&
			a2 == UINT32_C(0x3456789a) &&
			a3 == UINT64_C(0x456789abcdef0123) &&
			&a4 == state.integerReference &&
			a4 == UINT64_C(0x56789abcdef01234) &&
			&a5 == state.floatReference &&
			a5 == 67.25 &&
			a6 == state.pointer;
	}

	class IntegerTarget
	{
	public:
		explicit IntegerTarget(IntegerState &state)
			: state_(state)
		{
		}

		virtual std::uint64_t Run(
			std::uint8_t a0,
			std::uint16_t a1,
			std::uint32_t a2,
			std::uint64_t a3,
			std::uint64_t &a4,
			double &a5,
			void *a6)
		{
			++state_.originals;
			state_.originalArgs =
				state_.originalArgs &&
				IntegerArgsMatch(state_, a0, a1, a2, a3, a4, a5, a6);
			state_.healthy = state_.healthy && FrameHealthy();
			return kIntegerReturn;
		}

	private:
		IntegerState &state_;
	};

	class IntegerDelegate final : public DelegateRoot<IntegerDelegate>
	{
	public:
		explicit IntegerDelegate(IntegerState &state)
			: state_(state)
		{
		}

		virtual std::uint64_t Call(
			std::uint8_t a0,
			std::uint16_t a1,
			std::uint32_t a2,
			std::uint64_t a3,
			std::uint64_t &a4,
			double &a5,
			void *a6)
		{
			++state_.hooks;
			state_.hookArgs =
				state_.hookArgs &&
				IntegerArgsMatch(state_, a0, a1, a2, a3, a4, a5, a6);
			state_.healthy = state_.healthy && FrameHealthy();
			RETURN_META_VALUE(MRES_HANDLED, UINT64_C(0x1111111111111111));
		}

	private:
		IntegerState &state_;
	};

	bool TestIntegerArguments(std::string &error)
	{
		SourceHook::CProtoInfoBuilder proto(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		SetBasicReturn(proto, sizeof(std::uint64_t));
		AddBasic(proto, sizeof(std::uint8_t));
		AddBasic(proto, sizeof(std::uint16_t));
		AddBasic(proto, sizeof(std::uint32_t));
		AddBasic(proto, sizeof(std::uint64_t));
		AddBasic(
			proto,
			sizeof(std::uint64_t),
			SourceHook::PassInfo::PassFlag_ByRef);
		AddFloat(
			proto,
			sizeof(double),
			SourceHook::PassInfo::PassFlag_ByRef);
		AddBasic(proto, sizeof(void *));

		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
		{
			error = "Generate integer and pointer-class argument hook manager";
			return false;
		}

		std::uint64_t integerReference = UINT64_C(0x56789abcdef01234);
		double floatReference = 67.25;
		std::uint64_t pointerValue = UINT64_C(0x789abcdef0123456);
		IntegerState state = {
			0,
			0,
			true,
			true,
			true,
			&integerReference,
			&floatReference,
			&pointerValue
		};
		IntegerTarget target(state);
		int hook = InstallHook(
			&target,
			hookman,
			new IntegerDelegate(state),
			false);
		if (hook == 0)
			return InstallFailed(hookman, error, "Install integer argument hook");

		volatile std::uint64_t low = kCanaryLow;
		volatile std::uint64_t high = kCanaryHigh;
		std::uint64_t result = SourceHookTestHookManX64InvokeIntegers(
			&target,
			UINT8_C(0x12),
			UINT16_C(0x2345),
			UINT32_C(0x3456789a),
			UINT64_C(0x456789abcdef0123),
			integerReference,
			floatReference,
			&pointerValue);
		bool valid =
			result == kIntegerReturn &&
			state.hooks == 1 &&
			state.originals == 1 &&
			state.hookArgs &&
			state.originalArgs &&
			state.healthy &&
			CallerCanaries(low, high);

		if (!g_SHPtr->RemoveHookByID(hook))
		{
			error = "Remove integer argument hook";
			return false;
		}
		g_HMAGPtr->ReleaseHookMan(hookman);

		if (!valid)
			error = "Integer and pointer-class register/stack marshalling";
		return valid;
	}

	struct DoubleState
	{
		int preHooks;
		int postHooks;
		int originals;
		bool args;
		bool originalReturn;
		bool healthy;
	};

	double DoubleResult(
		double a0,
		double a1,
		double a2,
		double a3,
		double a4,
		double a5,
		double a6,
		double a7,
		double a8)
	{
		return
			a0 +
			2.0 * a1 +
			3.0 * a2 +
			4.0 * a3 +
			5.0 * a4 +
			6.0 * a5 +
			7.0 * a6 +
			8.0 * a7 +
			9.0 * a8;
	}

	bool DoubleArgsMatch(
		double a0,
		double a1,
		double a2,
		double a3,
		double a4,
		double a5,
		double a6,
		double a7,
		double a8)
	{
		return
			a0 == 0.5 &&
			a1 == 1.5 &&
			a2 == 2.5 &&
			a3 == 3.5 &&
			a4 == 4.5 &&
			a5 == 5.5 &&
			a6 == 6.5 &&
			a7 == 7.5 &&
			a8 == 8.5;
	}

	class DoubleTarget
	{
	public:
		explicit DoubleTarget(DoubleState &state)
			: state_(state)
		{
		}

		virtual double Run(
			double a0,
			double a1,
			double a2,
			double a3,
			double a4,
			double a5,
			double a6,
			double a7,
			double a8)
		{
			++state_.originals;
			state_.args =
				state_.args &&
				DoubleArgsMatch(a0, a1, a2, a3, a4, a5, a6, a7, a8);
			state_.healthy = state_.healthy && FrameHealthy();
			return DoubleResult(a0, a1, a2, a3, a4, a5, a6, a7, a8);
		}

	private:
		DoubleState &state_;
	};

	class DoubleDelegate final : public DelegateRoot<DoubleDelegate>
	{
	public:
		enum Kind
		{
			Pre,
			Post
		};

		DoubleDelegate(DoubleState &state, Kind kind)
			: state_(state),
			  kind_(kind)
		{
		}

		virtual double Call(
			double a0,
			double a1,
			double a2,
			double a3,
			double a4,
			double a5,
			double a6,
			double a7,
			double a8)
		{
			state_.args =
				state_.args &&
				DoubleArgsMatch(a0, a1, a2, a3, a4, a5, a6, a7, a8);
			state_.healthy = state_.healthy && FrameHealthy();
			if (kind_ == Pre)
			{
				++state_.preHooks;
				RETURN_META_VALUE(MRES_IGNORED, -1.0);
			}
			++state_.postHooks;
			state_.originalReturn =
				state_.originalReturn &&
				META_RESULT_ORIG_RET(double) ==
					DoubleResult(a0, a1, a2, a3, a4, a5, a6, a7, a8);
			RETURN_META_VALUE(MRES_OVERRIDE, 4321.25);
		}

	private:
		DoubleState &state_;
		Kind kind_;
	};

	bool TestDoubleArguments(std::string &error)
	{
		SourceHook::CProtoInfoBuilder proto(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		SetFloatReturn(proto, sizeof(double));
		for (int i = 0; i < 9; ++i)
			AddFloat(proto, sizeof(double));

		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
		{
			error = "Generate nine-double hook manager";
			return false;
		}

		DoubleState state = {0, 0, 0, true, true, true};
		DoubleTarget target(state);
		int pre = InstallHook(
			&target,
			hookman,
			new DoubleDelegate(state, DoubleDelegate::Pre),
			false);
		int post = InstallHook(
			&target,
			hookman,
			new DoubleDelegate(state, DoubleDelegate::Post),
			true);
		if (pre == 0 || post == 0)
			return InstallFailed(hookman, error, "Install nine-double hooks");

		volatile std::uint64_t low = kCanaryLow;
		volatile std::uint64_t high = kCanaryHigh;
		double result = SourceHookTestHookManX64InvokeDoubles(
			&target,
			0.5,
			1.5,
			2.5,
			3.5,
			4.5,
			5.5,
			6.5,
			7.5,
			8.5);
		bool valid =
			result == 4321.25 &&
			state.preHooks == 1 &&
			state.postHooks == 1 &&
			state.originals == 1 &&
			state.args &&
			state.originalReturn &&
			state.healthy &&
			CallerCanaries(low, high);

		bool removed =
			g_SHPtr->RemoveHookByID(pre) &&
			g_SHPtr->RemoveHookByID(post);
		if (!removed)
		{
			error = "Remove nine-double hooks";
			return false;
		}
		g_HMAGPtr->ReleaseHookMan(hookman);

		if (!valid)
			error = "Independent XMM allocation, spill, or double return";
		return valid;
	}

	struct FloatState
	{
		int preHooks;
		int postHooks;
		int originals;
		bool args;
		bool originalReturn;
		bool healthy;
	};

	class FloatTarget
	{
	public:
		explicit FloatTarget(FloatState &state)
			: state_(state)
		{
		}

		virtual float Run(float value)
		{
			++state_.originals;
			state_.args = state_.args && value == 7.25f;
			state_.healthy = state_.healthy && FrameHealthy();
			return value * 2.0f + 0.5f;
		}

	private:
		FloatState &state_;
	};

	class FloatDelegate final : public DelegateRoot<FloatDelegate>
	{
	public:
		enum Kind
		{
			Pre,
			Post
		};

		FloatDelegate(FloatState &state, Kind kind)
			: state_(state),
			  kind_(kind)
		{
		}

		virtual float Call(float value)
		{
			state_.args = state_.args && value == 7.25f;
			state_.healthy = state_.healthy && FrameHealthy();
			if (kind_ == Pre)
			{
				++state_.preHooks;
				RETURN_META_VALUE(MRES_IGNORED, -1.0f);
			}
			++state_.postHooks;
			state_.originalReturn =
				state_.originalReturn &&
				META_RESULT_ORIG_RET(float) == 15.0f;
			RETURN_META_VALUE(MRES_OVERRIDE, 19.75f);
		}

	private:
		FloatState &state_;
		Kind kind_;
	};

	bool TestFloatArgumentAndReturn(std::string &error)
	{
		SourceHook::CProtoInfoBuilder proto(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		SetFloatReturn(proto, sizeof(float));
		AddFloat(proto, sizeof(float));

		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
		{
			error = "Generate float argument and return hook manager";
			return false;
		}

		FloatState state = {0, 0, 0, true, true, true};
		FloatTarget target(state);
		int pre = InstallHook(
			&target,
			hookman,
			new FloatDelegate(state, FloatDelegate::Pre),
			false);
		int post = InstallHook(
			&target,
			hookman,
			new FloatDelegate(state, FloatDelegate::Post),
			true);
		if (pre == 0 || post == 0)
			return InstallFailed(
				hookman,
				error,
				"Install float argument and return hooks");

		volatile std::uint64_t low = kCanaryLow;
		volatile std::uint64_t high = kCanaryHigh;
		float result =
			SourceHookTestHookManX64InvokeFloat(&target, 7.25f);
		bool valid =
			result == 19.75f &&
			state.preHooks == 1 &&
			state.postHooks == 1 &&
			state.originals == 1 &&
			state.args &&
			state.originalReturn &&
			state.healthy &&
			CallerCanaries(low, high);

		bool removed =
			g_SHPtr->RemoveHookByID(pre) &&
			g_SHPtr->RemoveHookByID(post);
		if (!removed)
		{
			error = "Remove float argument and return hooks";
			return false;
		}
		g_HMAGPtr->ReleaseHookMan(hookman);

		if (!valid)
			error = "Four-byte float argument, return, or result mode";
		return valid;
	}

	struct MixedState
	{
		int hooks;
		int originals;
		bool args;
		bool healthy;
	};

	bool MixedArgsMatch(
		std::uint64_t g0,
		double s0,
		std::uint64_t g1,
		double s1,
		std::uint64_t g2,
		double s2,
		std::uint64_t g3,
		double s3,
		std::uint64_t g4,
		double s4,
		std::uint64_t g5,
		double s5,
		double s6,
		double s7,
		std::uint64_t g6,
		double s8)
	{
		return
			g0 == UINT64_C(0x101) &&
			g1 == UINT64_C(0x202) &&
			g2 == UINT64_C(0x303) &&
			g3 == UINT64_C(0x404) &&
			g4 == UINT64_C(0x505) &&
			g5 == UINT64_C(0x606) &&
			g6 == UINT64_C(0x707) &&
			s0 == 0.5 &&
			s1 == 1.5 &&
			s2 == 2.5 &&
			s3 == 3.5 &&
			s4 == 4.5 &&
			s5 == 5.5 &&
			s6 == 6.5 &&
			s7 == 7.5 &&
			s8 == 8.5;
	}

	class MixedTarget
	{
	public:
		explicit MixedTarget(MixedState &state)
			: state_(state)
		{
		}

		virtual std::uint64_t Run(
			std::uint64_t g0,
			double s0,
			std::uint64_t g1,
			double s1,
			std::uint64_t g2,
			double s2,
			std::uint64_t g3,
			double s3,
			std::uint64_t g4,
			double s4,
			std::uint64_t g5,
			double s5,
			double s6,
			double s7,
			std::uint64_t g6,
			double s8)
		{
			++state_.originals;
			state_.args =
				state_.args &&
				MixedArgsMatch(
					g0,
					s0,
					g1,
					s1,
					g2,
					s2,
					g3,
					s3,
					g4,
					s4,
					g5,
					s5,
					s6,
					s7,
					g6,
					s8);
			state_.healthy = state_.healthy && FrameHealthy();
			return UINT64_C(0xa55aa55a55aa55aa);
		}

	private:
		MixedState &state_;
	};

	class MixedDelegate final : public DelegateRoot<MixedDelegate>
	{
	public:
		explicit MixedDelegate(MixedState &state)
			: state_(state)
		{
		}

		virtual std::uint64_t Call(
			std::uint64_t g0,
			double s0,
			std::uint64_t g1,
			double s1,
			std::uint64_t g2,
			double s2,
			std::uint64_t g3,
			double s3,
			std::uint64_t g4,
			double s4,
			std::uint64_t g5,
			double s5,
			double s6,
			double s7,
			std::uint64_t g6,
			double s8)
		{
			++state_.hooks;
			state_.args =
				state_.args &&
				MixedArgsMatch(
					g0,
					s0,
					g1,
					s1,
					g2,
					s2,
					g3,
					s3,
					g4,
					s4,
					g5,
					s5,
					s6,
					s7,
					g6,
					s8);
			state_.healthy = state_.healthy && FrameHealthy();
			RETURN_META_VALUE(MRES_HANDLED, UINT64_C(0x1111222233334444));
		}

	private:
		MixedState &state_;
	};

	bool TestMixedArguments(std::string &error)
	{
		SourceHook::CProtoInfoBuilder proto(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		SetBasicReturn(proto, sizeof(std::uint64_t));
		AddBasic(proto, sizeof(std::uint64_t));
		AddFloat(proto, sizeof(double));
		AddBasic(proto, sizeof(std::uint64_t));
		AddFloat(proto, sizeof(double));
		AddBasic(proto, sizeof(std::uint64_t));
		AddFloat(proto, sizeof(double));
		AddBasic(proto, sizeof(std::uint64_t));
		AddFloat(proto, sizeof(double));
		AddBasic(proto, sizeof(std::uint64_t));
		AddFloat(proto, sizeof(double));
		AddBasic(proto, sizeof(std::uint64_t));
		AddFloat(proto, sizeof(double));
		AddFloat(proto, sizeof(double));
		AddFloat(proto, sizeof(double));
		AddBasic(proto, sizeof(std::uint64_t));
		AddFloat(proto, sizeof(double));

		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
		{
			error = "Generate mixed register-stream hook manager";
			return false;
		}

		MixedState state = {0, 0, true, true};
		MixedTarget target(state);
		int hook = InstallHook(
			&target,
			hookman,
			new MixedDelegate(state),
			false);
		if (hook == 0)
			return InstallFailed(hookman, error, "Install mixed argument hook");

		volatile std::uint64_t low = kCanaryLow;
		volatile std::uint64_t high = kCanaryHigh;
		std::uint64_t result = SourceHookTestHookManX64InvokeMixed(
			&target,
			UINT64_C(0x101),
			0.5,
			UINT64_C(0x202),
			1.5,
			UINT64_C(0x303),
			2.5,
			UINT64_C(0x404),
			3.5,
			UINT64_C(0x505),
			4.5,
			UINT64_C(0x606),
			5.5,
			6.5,
			7.5,
			UINT64_C(0x707),
			8.5);
		bool valid =
			result == UINT64_C(0xa55aa55a55aa55aa) &&
			state.hooks == 1 &&
			state.originals == 1 &&
			state.args &&
			state.healthy &&
			CallerCanaries(low, high);

		if (!g_SHPtr->RemoveHookByID(hook))
		{
			error = "Remove mixed argument hook";
			return false;
		}
		g_HMAGPtr->ReleaseHookMan(hookman);

		if (!valid)
			error = "Independent mixed register streams or shared ordered spills";
		return valid;
	}

	struct ScalarState
	{
		int hooks;
		int originals;
		bool args;
		bool resultContext;
		bool healthy;
	};

	std::uint64_t ScalarOriginalResult(std::uint64_t value)
	{
		return value ^ UINT64_C(0x123456789abcdef0);
	}

	class ScalarTarget
	{
	public:
		explicit ScalarTarget(ScalarState &state)
			: state_(state)
		{
		}

		virtual std::uint64_t Run(std::uint64_t value)
		{
			++state_.originals;
			state_.args = state_.args && value == UINT64_C(0x2468);
			state_.healthy = state_.healthy && FrameHealthy();
			return ScalarOriginalResult(value);
		}

	private:
		ScalarState &state_;
	};

	class ScalarDelegate final : public DelegateRoot<ScalarDelegate>
	{
	public:
		ScalarDelegate(
			ScalarState &state,
			META_RES result,
			std::uint64_t value)
			: state_(state),
			  result_(result),
			  value_(value)
		{
		}

		virtual std::uint64_t Call(std::uint64_t value)
		{
			++state_.hooks;
			state_.args = state_.args && value == UINT64_C(0x2468);
			state_.resultContext =
				state_.resultContext &&
				META_RESULT_PREVIOUS == MRES_IGNORED &&
				META_RESULT_STATUS == MRES_IGNORED;
			state_.healthy = state_.healthy && FrameHealthy();
			RETURN_META_VALUE(result_, value_);
		}

	private:
		ScalarState &state_;
		META_RES result_;
		std::uint64_t value_;
	};

	bool TestScalarResults(std::string &error)
	{
		SourceHook::CProtoInfoBuilder proto(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		SetBasicReturn(proto, sizeof(std::uint64_t));
		AddBasic(proto, sizeof(std::uint64_t));

		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
		{
			error = "Generate scalar return hook manager";
			return false;
		}

		ScalarState state = {0, 0, true, true, true};
		ScalarTarget target(state);
		const META_RES modes[] = {
			MRES_IGNORED,
			MRES_HANDLED,
			MRES_OVERRIDE,
			MRES_SUPERCEDE
		};
		const std::uint64_t hookResults[] = {
			UINT64_C(0x1001),
			UINT64_C(0x1002),
			UINT64_C(0x1003),
			UINT64_C(0x1004)
		};
		bool valid = true;

		for (int i = 0; i < 4; ++i)
		{
			int originalsBefore = state.originals;
			int hook = InstallHook(
				&target,
				hookman,
				new ScalarDelegate(state, modes[i], hookResults[i]),
				false);
			if (hook == 0)
				return InstallFailed(hookman, error, "Install scalar result hook");

			volatile std::uint64_t low = kCanaryLow;
			volatile std::uint64_t high = kCanaryHigh;
			std::uint64_t result =
				SourceHookTestHookManX64InvokeScalar(
					&target,
					UINT64_C(0x2468));
			std::uint64_t expected =
				modes[i] >= MRES_OVERRIDE
					? hookResults[i]
					: ScalarOriginalResult(UINT64_C(0x2468));
			int expectedOriginals =
				originalsBefore + (modes[i] == MRES_SUPERCEDE ? 0 : 1);
			valid =
				valid &&
				result == expected &&
				state.originals == expectedOriginals &&
				CallerCanaries(low, high);

			if (!g_SHPtr->RemoveHookByID(hook))
			{
				error = "Remove scalar result hook";
				return false;
			}
		}

		valid =
			valid &&
			state.hooks == 4 &&
			state.args &&
			state.resultContext &&
			state.healthy;
		g_HMAGPtr->ReleaseHookMan(hookman);

		if (!valid)
			error = "Ignored, handled, override, or supercede scalar result";
		return valid;
	}

	struct ReferenceState
	{
		int hooks;
		int originals;
		bool args;
		bool healthy;
		std::uint64_t original;
		std::uint64_t alternate;
	};

	class ReferenceTarget
	{
	public:
		explicit ReferenceTarget(ReferenceState &state)
			: state_(state)
		{
		}

		virtual std::uint64_t &Run(std::uint64_t selector)
		{
			++state_.originals;
			state_.args = state_.args && selector == UINT64_C(0x55);
			state_.healthy = state_.healthy && FrameHealthy();
			return state_.original;
		}

	private:
		ReferenceState &state_;
	};

	class ReferenceDelegate final : public DelegateRoot<ReferenceDelegate>
	{
	public:
		ReferenceDelegate(ReferenceState &state, META_RES result)
			: state_(state),
			  result_(result)
		{
		}

		virtual std::uint64_t &Call(std::uint64_t selector)
		{
			++state_.hooks;
			state_.args = state_.args && selector == UINT64_C(0x55);
			state_.healthy = state_.healthy && FrameHealthy();
			RETURN_META_VALUE(result_, state_.alternate);
		}

	private:
		ReferenceState &state_;
		META_RES result_;
	};

	bool TestReferenceResults(std::string &error)
	{
		SourceHook::CProtoInfoBuilder proto(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		SetBasicReturn(
			proto,
			sizeof(std::uint64_t),
			SourceHook::PassInfo::PassFlag_ByRef);
		AddBasic(proto, sizeof(std::uint64_t));

		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
		{
			error = "Generate reference return hook manager";
			return false;
		}

		ReferenceState state = {
			0,
			0,
			true,
			true,
			UINT64_C(0x1111222233334444),
			UINT64_C(0xaaaabbbbccccdddd)
		};
		ReferenceTarget target(state);
		const META_RES modes[] = {
			MRES_IGNORED,
			MRES_OVERRIDE,
			MRES_SUPERCEDE
		};
		bool valid = true;

		for (int i = 0; i < 3; ++i)
		{
			int originalsBefore = state.originals;
			int hook = InstallHook(
				&target,
				hookman,
				new ReferenceDelegate(state, modes[i]),
				false);
			if (hook == 0)
				return InstallFailed(hookman, error, "Install reference result hook");

			volatile std::uint64_t low = kCanaryLow;
			volatile std::uint64_t high = kCanaryHigh;
			std::uint64_t *result =
				SourceHookTestHookManX64InvokeReference(
					&target,
					UINT64_C(0x55));
			std::uint64_t *expected =
				modes[i] >= MRES_OVERRIDE
					? &state.alternate
					: &state.original;
			int expectedOriginals =
				originalsBefore + (modes[i] == MRES_SUPERCEDE ? 0 : 1);
			valid =
				valid &&
				result == expected &&
				state.originals == expectedOriginals &&
				CallerCanaries(low, high);

			if (!g_SHPtr->RemoveHookByID(hook))
			{
				error = "Remove reference result hook";
				return false;
			}
		}

		valid =
			valid &&
			state.hooks == 3 &&
			state.args &&
			state.healthy;
		g_HMAGPtr->ReleaseHookMan(hookman);

		if (!valid)
			error = "Reference return RAX path or result mode";
		return valid;
	}

	enum ReentrantEvent
	{
		Reentrant_OuterPre = 12,
		Reentrant_InnerPre = 11,
		Reentrant_InnerOriginal = 21,
		Reentrant_InnerPost = 31,
		Reentrant_OuterOriginal = 22,
		Reentrant_OuterPost = 32
	};

	struct ReentrantState
	{
		int events[8];
		int eventCount;
		int originals;
		std::uint64_t nestedResult;
		bool contextRestored;
		bool originalResults;
		bool healthy;
	};

	void RecordReentrant(ReentrantState &state, int event)
	{
		if (state.eventCount < 8)
			state.events[state.eventCount] = event;
		++state.eventCount;
	}

	class ReentrantTarget
	{
	public:
		explicit ReentrantTarget(ReentrantState &state)
			: state_(state)
		{
		}

		virtual std::uint64_t Run(std::uint64_t depth)
		{
			RecordReentrant(state_, static_cast<int>(20 + depth));
			++state_.originals;
			state_.healthy = state_.healthy && FrameHealthy();
			return 100 + depth;
		}

	private:
		ReentrantState &state_;
	};

	class ReentrantDelegate final : public DelegateRoot<ReentrantDelegate>
	{
	public:
		enum Kind
		{
			Pre,
			Post
		};

		ReentrantDelegate(
			ReentrantState &state,
			ReentrantTarget &target,
			Kind kind)
			: state_(state),
			  target_(target),
			  kind_(kind)
		{
		}

		virtual std::uint64_t Call(std::uint64_t depth)
		{
			state_.healthy = state_.healthy && FrameHealthy();
			if (kind_ == Pre)
			{
				RecordReentrant(state_, static_cast<int>(10 + depth));
				if (depth == 2)
				{
					state_.nestedResult =
						SourceHookTestHookManX64InvokeReentrant(
							&target_,
							1);
					state_.contextRestored =
						state_.contextRestored &&
						META_RESULT_PREVIOUS == MRES_IGNORED &&
						META_RESULT_STATUS == MRES_IGNORED &&
						META_IFACEPTR(ReentrantTarget) == &target_;
				}
				RETURN_META_VALUE(MRES_HANDLED, UINT64_C(0));
			}

			RecordReentrant(state_, static_cast<int>(30 + depth));
			state_.originalResults =
				state_.originalResults &&
				META_RESULT_ORIG_RET(std::uint64_t) == 100 + depth;
			RETURN_META_VALUE(MRES_IGNORED, UINT64_C(0));
		}

	private:
		ReentrantState &state_;
		ReentrantTarget &target_;
		Kind kind_;
	};

	bool TestReentrant(std::string &error)
	{
		SourceHook::CProtoInfoBuilder proto(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		SetBasicReturn(proto, sizeof(std::uint64_t));
		AddBasic(proto, sizeof(std::uint64_t));

		SourceHook::HookManagerPubFunc hookman =
			g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
		{
			error = "Generate reentrant hook manager";
			return false;
		}

		ReentrantState state = {
			{},
			0,
			0,
			0,
			true,
			true,
			true
		};
		ReentrantTarget target(state);
		int pre = InstallHook(
			&target,
			hookman,
			new ReentrantDelegate(state, target, ReentrantDelegate::Pre),
			false);
		int post = InstallHook(
			&target,
			hookman,
			new ReentrantDelegate(state, target, ReentrantDelegate::Post),
			true);
		if (pre == 0 || post == 0)
			return InstallFailed(hookman, error, "Install reentrant hooks");

		volatile std::uint64_t low = kCanaryLow;
		volatile std::uint64_t high = kCanaryHigh;
		std::uint64_t result =
			SourceHookTestHookManX64InvokeReentrant(&target, 2);
		bool valid =
			result == 102 &&
			state.nestedResult == 101 &&
			state.originals == 2 &&
			state.eventCount == 6 &&
			state.events[0] == Reentrant_OuterPre &&
			state.events[1] == Reentrant_InnerPre &&
			state.events[2] == Reentrant_InnerOriginal &&
			state.events[3] == Reentrant_InnerPost &&
			state.events[4] == Reentrant_OuterOriginal &&
			state.events[5] == Reentrant_OuterPost &&
			state.contextRestored &&
			state.originalResults &&
			state.healthy &&
			CallerCanaries(low, high);

		bool removed =
			g_SHPtr->RemoveHookByID(pre) &&
			g_SHPtr->RemoveHookByID(post);
		if (!removed)
		{
			error = "Remove reentrant hooks";
			return false;
		}
		g_HMAGPtr->ReleaseHookMan(hookman);

		if (!valid)
			error = "Nested generated hook invocation or context restoration";
		return valid;
	}

	struct UnsupportedObject
	{
		std::uint64_t words[3];
	};

	bool TestFailClosed(std::string &error)
	{
		SourceHook::CProtoInfoBuilder objectValue(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		objectValue.AddParam(
			sizeof(UnsupportedObject),
			SourceHook::PassInfo::PassType_Object,
			SourceHook::PassInfo::PassFlag_ByVal,
			nullptr,
			nullptr,
			nullptr,
			nullptr);

		SourceHook::CProtoInfoBuilder objectReference(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		objectReference.AddParam(
			sizeof(UnsupportedObject),
			SourceHook::PassInfo::PassType_Object,
			SourceHook::PassInfo::PassFlag_ByRef,
			nullptr,
			nullptr,
			nullptr,
			nullptr);

		SourceHook::CProtoInfoBuilder structureReturn(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		structureReturn.SetReturnType(
			sizeof(UnsupportedObject),
			SourceHook::PassInfo::PassType_Object,
			SourceHook::PassInfo::PassFlag_ByVal |
				SourceHook::PassInfo::PassFlag_RetMem,
			nullptr,
			nullptr,
			nullptr,
			nullptr);

		SourceHook::CProtoInfoBuilder vafmt(
			SourceHook::ProtoInfo::CallConv_ThisCall |
				SourceHook::ProtoInfo::CallConv_HasVafmt);

		SourceHook::CProtoInfoBuilder typedVoid(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		typedVoid.SetReturnType(
			0,
			SourceHook::PassInfo::PassType_Basic,
			SourceHook::PassInfo::PassFlag_ByVal,
			nullptr,
			nullptr,
			nullptr,
			nullptr);

		char lifetimeMarker;
		SourceHook::CProtoInfoBuilder lifetime(
			SourceHook::ProtoInfo::CallConv_ThisCall);
		lifetime.SetReturnType(
			0,
			SourceHook::PassInfo::PassType_Unknown,
			SourceHook::PassInfo::PassFlag_ByVal,
			nullptr,
			nullptr,
			nullptr,
			&lifetimeMarker);

		bool rejected = true;
		rejected = Rejects(objectValue) && rejected;
		rejected = Rejects(objectReference) && rejected;
		rejected = Rejects(structureReturn) && rejected;
		rejected = Rejects(vafmt) && rejected;
		rejected = Rejects(typedVoid) && rejected;
		rejected = Rejects(lifetime) && rejected;

		if (!rejected)
			error = "Reject unsupported object, sret, vafmt, or lifetime signature";
		return rejected;
	}
}

bool TestHookManGenX64(std::string &error)
{
	GET_SHPTR(g_SHPtr);
	GET_HMAG(g_HMAGPtr, g_SHPtr);
	g_PLID = 64064;

	bool passed =
		TestFailClosed(error) &&
		TestZeroAndPreservation(error) &&
		TestIntegerArguments(error) &&
		TestDoubleArguments(error) &&
		TestFloatArgumentAndReturn(error) &&
		TestMixedArguments(error) &&
		TestScalarResults(error) &&
		TestReferenceResults(error) &&
		TestReentrant(error);

	Test_CompleteShutdown(g_SHPtr);
	return passed;
}
