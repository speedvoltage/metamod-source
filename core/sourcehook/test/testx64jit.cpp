#include <cstring>
#include <string>

#include "sh_asm_x86_64.h"
#include "testevents.h"

bool TestX64JitWriter(std::string &error)
{
	using namespace SourceHook;
	using namespace SourceHook::Asm;

	CPageAlloc allocator;
	x64JitWriter writer(&allocator);

	writer.xor_reg(rax, rax);
	writer.xor_reg(r9, r9);

	const unsigned char expected[] = {
		0x48, 0x31, 0xc0,
		0x4d, 0x31, 0xc9,
	};

	CHECK_COND(writer.GetSize() == sizeof(expected), "Encoded size");
	CHECK_COND(std::memcmp(writer.GetData(), expected, sizeof(expected)) == 0, "Encoded bytes");

	return true;
}
