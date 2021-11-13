#pragma once

#include SAMPCOMMANDHANDLER_PCH

#include <SAMPCommandHandler/Commands.hpp>
#include <SAMPCommandHandler/Helper/CaseInsensitiveCompare.hpp>

namespace samp_cmd
{


template <typename TPlayer = samp_cpp::Player>
class CommandHandler
{
public:
	using Player 	= TPlayer;
	using Cmd 		= CommandBase<Player>;

	bool handleCommandText(int32_t playerIdx_, std::string_view command_);

	////////////////////////////////////
	template <	template <typename TPlayerCmd> typename TCmd,
				typename = std::enable_if_t< std::is_base_of_v<Cmd, TCmd<TPlayer> > > >
	void add(std::vector<std::string> aliases_, typename TCmd<TPlayer>::Func cmdFunc_)
	{
		return this->add(std::move(aliases_), std::make_shared< TCmd<TPlayer> >( std::move(cmdFunc_) ) );
	}

	////////////////////////////////////
	template <	template <typename TPlayerCmd> typename TCmd,
				typename = std::enable_if_t< std::is_base_of_v<Cmd, TCmd<TPlayer> > > >
	void add(std::vector<std::string> aliases_, TCmd<TPlayer> cmd_)
	{
		return this->add(std::move(aliases_), std::make_shared<TCmd>( std::move(cmd_) ) );
	}

	////////////////////////////////////
	void add(std::vector<std::string> aliases_, typename FunctionCommandNoArgs<Player>::Func cmdFunc_)
	{
		return this->add<FunctionCommandNoArgs>( std::move(aliases_), std::move(cmdFunc_) );
	}

	////////////////////////////////////
	void add(std::vector<std::string> aliases_, typename FunctionCommand<Player>::Func cmdFunc_)
	{
		return this->add<FunctionCommand>( std::move(aliases_), std::move(cmdFunc_) );
	}

	////////////////////////////////////
	// Single invocation method:
	////////////////////////////////////
	template <	template <typename TPlayerCmd> typename TCmd,
				typename = std::enable_if_t< std::is_base_of_v<Cmd, TCmd<TPlayer> > > >
	void add(std::string invocationText_, typename TCmd<TPlayer>::Func cmdFunc_)
	{
		return this->add( std::move(invocationText_), std::make_shared< TCmd<TPlayer> >( std::move(cmdFunc_) ) );
	}

	////////////////////////////////////
	template <	template <typename TPlayerCmd> typename TCmd,
				typename = std::enable_if_t< std::is_base_of_v<Cmd, TCmd<TPlayer> > > >
	void add(std::string invocationText_, TCmd<TPlayer> cmd_)
	{
		return this->add( std::move(invocationText_), std::make_shared<TCmd>( std::move(cmd_) ) );
	}

	////////////////////////////////////
	void add(std::string invocationText_, typename FunctionCommandNoArgs<Player>::Func cmdFunc_)
	{
		return this->add<FunctionCommandNoArgs>( std::move(invocationText_), std::move(cmdFunc_) );
	}

	////////////////////////////////////
	void add(std::string invocationText_, typename FunctionCommand<Player>::Func cmdFunc_)
	{
		return this->add<FunctionCommand>( std::move(invocationText_), std::move(cmdFunc_) );
	}


	////////////////////////////////////
	// Base add methods:
	////////////////////////////////////
	void add(std::vector<std::string> aliases_, std::shared_ptr<Cmd> cmd_);

	void add(std::string invocationText_, std::shared_ptr<Cmd> cmd_);

	std::map<std::string, std::shared_ptr<Cmd>, IgnoreCaseLess> commands;
};

}

#include <SAMPCommandHandler/CommandHandler.inl>

