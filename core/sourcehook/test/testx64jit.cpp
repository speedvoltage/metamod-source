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
	writer.movss(xmm0, rbp(-4));
	writer.movss(rbp(-4), xmm0);

	const unsigned char expected[] = {
		0x48, 0x31, 0xc0,
		0x4d, 0x31, 0xc9,
		0xf3, 0x0f, 0x10, 0x45, 0xfc,
		0xf3, 0x0f, 0x11, 0x45, 0xfc,
	};

	CHECK_COND(writer.GetSize() == sizeof(expected), "Encoded size");
	CHECK_COND(std::memcmp(writer.GetData(), expected, sizeof(expected)) == 0, "Encoded bytes");

	return true;
}
