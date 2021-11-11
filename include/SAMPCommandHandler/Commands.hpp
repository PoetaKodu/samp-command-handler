#pragma once

#include SAMPCOMMANDHANDLER_PCH

#include <SAMPCommandHandler/CommandArgs.hpp>

namespace samp_cmd
{

/// <summary>A base command wrapper class.</summary>
/// <tparam name="TPlayer">SAMP player class</tparam>
template <typename TPlayer = samp_cpp::Player>
class CommandBase
{
public:
	virtual ~CommandBase() {}
	virtual void invoke(TPlayer player_, CommandArgs args_) { }
};

/// <summary>A wrapper for function command with no arguments.</summary>
/// <tparam name="TPlayer">SAMP player class</tparam>
template <typename TPlayer = samp_cpp::Player>
class FunctionCommandNoArgs : public CommandBase<TPlayer>
{
public:
	using Func = std::function<void(TPlayer)>;

	FunctionCommandNoArgs() = default;

	FunctionCommandNoArgs(Func func_) : func(std::move(func_))
	{
	}

	virtual void invoke(TPlayer player_, CommandArgs args_) { func(player_); }

	Func func;
};

/// <summary>A wrapper for function command.</summary>
/// <tparam name="TPlayer">SAMP player class</tparam>
template <typename TPlayer = samp_cpp::Player>
class FunctionCommand : public CommandBase<TPlayer>
{
public:
	using Func = std::function<void(TPlayer, CommandArgs)>;

	FunctionCommand() = default;

	FunctionCommand(Func func_)
		: func(std::move(func_))
	{
	}

	virtual void invoke(TPlayer player_, CommandArgs args_) {
		args_.tryParse(1);
		func(player_, args_);
	}

	Func func;
};


}