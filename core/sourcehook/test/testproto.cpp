#include <string>

#include "sourcehook_impl_cproto.h"
#include "sourcehook_pibuilder.h"
#include "testevents.h"

bool TestProtoIdentity(std::string &error)
{
	using namespace SourceHook;
	using namespace SourceHook::Impl;

	char copyCtor1;
	char copyCtor2;

	IntPassInfo first = {};
	first.size = 12;
	first.type = PassInfo::PassType_Object;
	first.flags = PassInfo::PassFlag_ByVal | PassInfo::PassFlag_CCtor;
	first.pCopyCtor = &copyCtor1;

	IntPassInfo second = first;
	CHECK_COND(first == second, "Identical pass info");
	second.pCopyCtor = &copyCtor2;
	CHECK_COND(first != second, "Copy constructor pass identity");

	CProtoInfoBuilder returnBuilder1(ProtoInfo::CallConv_ThisCall);
	returnBuilder1.SetReturnType(
		12, PassInfo::PassType_Object, PassInfo::PassFlag_ByVal,
		nullptr, &copyCtor1, nullptr, nullptr);
	CProtoInfoBuilder returnBuilder2(ProtoInfo::CallConv_ThisCall);
	returnBuilder2.SetReturnType(
		12, PassInfo::PassType_Object, PassInfo::PassFlag_ByVal,
		nullptr, &copyCtor2, nullptr, nullptr);

	CProto returnProto1(returnBuilder1);
	CProto returnProto2(returnBuilder2);
	CHECK_COND(!returnProto1.ExactlyEqual(returnProto2), "Copy constructor return identity");

	CProtoInfoBuilder paramBuilder1(ProtoInfo::CallConv_ThisCall);
	paramBuilder1.AddParam(
		12, PassInfo::PassType_Object, PassInfo::PassFlag_ByVal,
		nullptr, &copyCtor1, nullptr, nullptr);
	CProtoInfoBuilder paramBuilder2(ProtoInfo::CallConv_ThisCall);
	paramBuilder2.AddParam(
		12, PassInfo::PassType_Object, PassInfo::PassFlag_ByVal,
		nullptr, &copyCtor2, nullptr, nullptr);

	CProto paramProto1(paramBuilder1);
	CProto paramProto2(paramBuilder2);
	CHECK_COND(!paramProto1.ExactlyEqual(paramProto2), "Copy constructor parameter identity");

	return true;
}
