#pragma once

#include <stdint.h>

// These classes define interface classes wich are used to communicate with the client.
// They use only native types and noy dynamic containers. The lifetime of provided pointers
// must be assumed to be static and the client is responsible to ensure that.

class ClDbgByte
{
public:
	uint8_t value;
	bool safe:1;
};

class ClDbgStatusFlag
{
public:
	ClDbgStatusFlag(const char *Name = nullptr, uint64_t Bitmask = (uint64_t)-1, bool Unused = false)
	: name(Name)
	, bitmask(Bitmask)
	, unused(Unused)
	{
	}

	const char *name;
	uint64_t bitmask;
	bool unused : 1;
};

class ClDbgCPURegister
{
public:
	typedef enum
	{
		UNKNOWN					= 0
		, DATA					= (1 << 0)
		, ADDRESS				= (1 << 1)
		, DATA_ADDRESS			= DATA | ADDRESS
		, INSTRUCTION_POINTER	= (1 << 2)
		, STACK_POINTER			= (1 << 3)
		, STATUS_FLAGS			= (1 << 4)
	} RegisterType;

public:
	ClDbgCPURegister()
	: name(nullptr)
	, type(UNKNOWN)
	, byteWidth(0)
	{
	}

	const char *name;
	RegisterType type;
	uint16_t byteWidth;			// Width of the register in bytes
};

class ClDbgCPUCore
{
public:
	ClDbgCPUCore()
	: name(nullptr)
	, model(nullptr)
	, type(0)
	, registers(0)
	, statusFlags(0)
	, isVirtual(false)
	{
	}

	const char *name;
	const char *model;
	uint32_t type;				// Type to identify the disassembler (To be defined)
	uint16_t registers;
	uint16_t statusFlags;
	bool isVirtual : 1;
};

class ClDbgMachine
{
public:
	ClDbgMachine()
	: name(nullptr)
	, model(nullptr)
	, cores(0)
	{
	}

	const char *name;
	const char *model;
	uint16_t cores;
};
