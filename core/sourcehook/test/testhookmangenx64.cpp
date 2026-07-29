#include <cstdint>
#include <string>

#include "sourcehook.h"
#include "sourcehook_test.h"
#include "testevents.h"
#include "sourcehook_pibuilder.h"

extern "C" std::uintptr_t SourceHookTestHookManX64StackAlignment();
extern "C" void SourceHookTestHookManX64Invoke(void *object);

__asm__(
	".text\n"
	".type SourceHookTestHookManX64StackAlignment,@function\n"
	"SourceHookTestHookManX64StackAlignment:\n"
	"movq %rsp, %rax\n"
	"andq $15, %rax\n"
	"ret\n"
	".size SourceHookTestHookManX64StackAlignment,.-SourceHookTestHookManX64StackAlignment\n"
	".type SourceHookTestHookManX64Invoke,@function\n"
	"SourceHookTestHookManX64Invoke:\n"
	"movq (%rdi), %rax\n"
	"jmp *(%rax)\n"
	".size SourceHookTestHookManX64Invoke,.-SourceHookTestHookManX64Invoke\n");

namespace
{
	SourceHook::ISourceHook *g_SHPtr;
	SourceHook::Plugin g_PLID;
	SourceHook::IHookManagerAutoGen *g_HMAGPtr;

	enum Event
	{
		Event_PreOne,
		Event_PreTwo,
		Event_Original,
		Event_Post
	};

	Event g_Events[8];
	int g_EventCount;
	int g_OriginalCalls;
	bool g_PreOneResultOrder;
	bool g_PreTwoResultOrder;
	bool g_PostResultOrder;
	bool g_PreOneAligned;
	bool g_PreTwoAligned;
	bool g_OriginalAligned;
	bool g_PostAligned;

	void Record(Event event)
	{
		if (g_EventCount < static_cast<int>(sizeof(g_Events) / sizeof(g_Events[0])))
			g_Events[g_EventCount] = event;
		++g_EventCount;
	}

	bool StackAligned()
	{
		return SourceHookTestHookManX64StackAlignment() == 8;
	}

	void ResetState()
	{
		g_EventCount = 0;
		g_OriginalCalls = 0;
		g_PreOneResultOrder = false;
		g_PreTwoResultOrder = false;
		g_PostResultOrder = false;
		g_PreOneAligned = false;
		g_PreTwoAligned = false;
		g_OriginalAligned = true;
		g_PostAligned = false;
	}

	class Target
	{
	public:
		virtual void Run()
		{
			Record(Event_Original);
			++g_OriginalCalls;
			g_OriginalAligned = g_OriginalAligned && StackAligned();
		}
	};

	class Delegate final : public SourceHook::ISHDelegate
	{
	public:
		enum Kind
		{
			PreOne,
			PreTwo,
			Post
		};

		explicit Delegate(Kind kind)
			: kind_(kind)
		{
		}

		bool IsEqual(SourceHook::ISHDelegate *other) override
		{
			return other == this;
		}

		void DeleteThis() override
		{
			delete this;
		}

		virtual void Call()
		{
			switch (kind_)
			{
			case PreOne:
				Record(Event_PreOne);
				g_PreOneAligned = StackAligned();
				g_PreOneResultOrder =
					META_RESULT_PREVIOUS == MRES_IGNORED &&
					META_RESULT_STATUS == MRES_IGNORED;
				RETURN_META(MRES_HANDLED);
			case PreTwo:
				Record(Event_PreTwo);
				g_PreTwoAligned = StackAligned();
				g_PreTwoResultOrder =
					META_RESULT_PREVIOUS == MRES_HANDLED &&
					META_RESULT_STATUS == MRES_HANDLED;
				RETURN_META(MRES_IGNORED);
			case Post:
				Record(Event_Post);
				g_PostAligned = StackAligned();
				g_PostResultOrder =
					META_RESULT_PREVIOUS == MRES_IGNORED &&
					META_RESULT_STATUS == MRES_HANDLED;
				RETURN_META(MRES_IGNORED);
			}
		}

	private:
		Kind kind_;
	};

	int AddHook(Target *target, SourceHook::HookManagerPubFunc hookman, Delegate::Kind kind, bool post)
	{
		Delegate *delegate = new Delegate(kind);
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
		SourceHook::HookManagerPubFunc hookman = g_HMAGPtr->MakeHookMan(proto, 0, 0);
		if (hookman == nullptr)
			return true;
		g_HMAGPtr->ReleaseHookMan(hookman);
		return false;
	}
}

bool TestHookManGenX64(std::string &error)
{
	using namespace SourceHook;

	GET_SHPTR(g_SHPtr);
	GET_HMAG(g_HMAGPtr, g_SHPtr);
	CProtoInfoBuilder proto(ProtoInfo::CallConv_ThisCall);

	g_PLID = 64064;
	ResetState();

	CProtoInfoBuilder parameter_proto(ProtoInfo::CallConv_ThisCall);
	parameter_proto.AddParam(
		sizeof(int),
		PassInfo::PassType_Basic,
		PassInfo::PassFlag_ByVal,
		nullptr,
		nullptr,
		nullptr,
		nullptr);

	CProtoInfoBuilder return_proto(ProtoInfo::CallConv_ThisCall);
	return_proto.SetReturnType(
		sizeof(int),
		PassInfo::PassType_Basic,
		PassInfo::PassFlag_ByVal,
		nullptr,
		nullptr,
		nullptr,
		nullptr);

	CProtoInfoBuilder vafmt_proto(
		ProtoInfo::CallConv_ThisCall | ProtoInfo::CallConv_HasVafmt);

	CProtoInfoBuilder typed_void_proto(ProtoInfo::CallConv_ThisCall);
	typed_void_proto.SetReturnType(
		0,
		PassInfo::PassType_Basic,
		PassInfo::PassFlag_ByVal,
		nullptr,
		nullptr,
		nullptr,
		nullptr);

	char lifetime_marker;
	CProtoInfoBuilder lifetime_proto(ProtoInfo::CallConv_ThisCall);
	lifetime_proto.SetReturnType(
		0,
		PassInfo::PassType_Unknown,
		PassInfo::PassFlag_ByVal,
		nullptr,
		nullptr,
		nullptr,
		&lifetime_marker);

	bool rejected =
		Rejects(parameter_proto) &&
		Rejects(return_proto) &&
		Rejects(vafmt_proto) &&
		Rejects(typed_void_proto) &&
		Rejects(lifetime_proto);
	if (!rejected)
	{
		Test_CompleteShutdown(g_SHPtr);
		error = "Reject unsupported SysV signatures";
		return false;
	}

	HookManagerPubFunc hookman = g_HMAGPtr->MakeHookMan(proto, 0, 0);
	if (hookman == nullptr)
	{
		Test_CompleteShutdown(g_SHPtr);
		error = "Generate zero-parameter void hook manager";
		return false;
	}

	Target target;
	int pre_one = AddHook(&target, hookman, Delegate::PreOne, false);
	int pre_two = AddHook(&target, hookman, Delegate::PreTwo, false);
	int post = AddHook(&target, hookman, Delegate::Post, true);

	if (pre_one == 0 || pre_two == 0 || post == 0)
	{
		Test_CompleteShutdown(g_SHPtr);
		g_HMAGPtr->ReleaseHookMan(hookman);
		error = "Install generated hooks";
		return false;
	}

	SourceHookTestHookManX64Invoke(&target);

	bool hooked_order =
		g_EventCount == 4 &&
		g_Events[0] == Event_PreOne &&
		g_Events[1] == Event_PreTwo &&
		g_Events[2] == Event_Original &&
		g_Events[3] == Event_Post;
	bool result_order =
		g_PreOneResultOrder &&
		g_PreTwoResultOrder &&
		g_PostResultOrder;
	bool alignment =
		g_PreOneAligned &&
		g_PreTwoAligned &&
		g_OriginalAligned &&
		g_PostAligned;
	bool original_called = g_OriginalCalls == 1;

	bool removed_pre_one = g_SHPtr->RemoveHookByID(pre_one);
	bool removed_pre_two = g_SHPtr->RemoveHookByID(pre_two);
	bool removed_post = g_SHPtr->RemoveHookByID(post);

	g_EventCount = 0;
	SourceHookTestHookManX64Invoke(&target);

	bool unhooked =
		g_EventCount == 1 &&
		g_Events[0] == Event_Original &&
		g_OriginalCalls == 2 &&
		g_OriginalAligned;

	Test_CompleteShutdown(g_SHPtr);
	g_HMAGPtr->ReleaseHookMan(hookman);

	CHECK_COND(hooked_order, "Pre/original/post order");
	CHECK_COND(result_order, "Previous/status result ordering");
	CHECK_COND(alignment, "Delegate/original stack alignment");
	CHECK_COND(original_called, "Original call count");
	CHECK_COND(removed_pre_one && removed_pre_two && removed_post, "Hook removal");
	CHECK_COND(unhooked, "Invocation after hook removal");

	return true;
}
