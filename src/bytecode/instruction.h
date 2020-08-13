#pragma once

template <typename T>
struct size_verification
{
	static_assert(sizeof(T) == 4);
};

#pragma pack(push, 4)

class instruction
{
	int opcode : 8;
};

class move_r8_r8 : instruction, size_verification<move_r8_r8>
{
	int dst : 8;
	int src : 8;
};

class move_r12_r12 : instruction, size_verification<move_r12_r12>
{
	int dst : 12;
	int src : 12;
};

class load_imm16_r8 : instruction, size_verification<load_imm16_r8>
{
	int imm : 16;
	int dst : 8;
};

static move_r12_r12 asd;

#pragma pack(pop)
