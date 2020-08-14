#pragma once

#pragma pack(push, 4)

class instruction
{
	int opcode : 8;
};

class move_r8_r8 : instruction
{
	int dst : 8;
	int src : 8;
};

class move_r12_r12 : instruction
{
	int dst : 12;
	int src : 12;
};

class load_imm16_r8 : instruction
{
	int imm : 16;
	int dst : 8;
};

#pragma pack(pop)
