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
	void add(std::vector<std::string_view> const& aliases_, typename TCmd<TPlayer>::Func cmdFunc_)
	{
		return this->add(aliases_, std::make_shared< TCmd<TPlayer> >( std::move(cmdFunc_) ) );
	}

	////////////////////////////////////
	template <	template <typename TPlayerCmd> typename TCmd,
				typename = std::enable_if_t< std::is_base_of_v<Cmd, TCmd<TPlayer> > > >
	void add(std::vector<std::string_view> const& aliases_, TCmd<TPlayer> cmd_)
	{
		return this->add(aliases_, std::make_shared<TCmd>( std::move(cmd_) ) );
	}

	////////////////////////////////////
	void add(std::vector<std::string_view> const& aliases_, typename FunctionCommandNoArgs<Player>::Func cmdFunc_)
	{
		return this->add<FunctionCommandNoArgs>( aliases_, std::move(cmdFunc_) );
	}

	////////////////////////////////////
	void add(std::vector<std::string_view> const& aliases_, typename FunctionCommand<Player>::Func cmdFunc_)
	{
		return this->add<FunctionCommand>( aliases_, std::move(cmdFunc_) );
	}

	////////////////////////////////////
	// Single invocation method:
	////////////////////////////////////
	template <	template <typename TPlayerCmd> typename TCmd,
				typename = std::enable_if_t< std::is_base_of_v<Cmd, TCmd<TPlayer> > > >
	void add(std::string_view invocationText_, typename TCmd<TPlayer>::Func cmdFunc_)
	{
		return this->add( invocationText_, std::make_shared< TCmd<TPlayer> >( std::move(cmdFunc_) ) );
	}

	////////////////////////////////////
	template <	template <typename TPlayerCmd> typename TCmd,
				typename = std::enable_if_t< std::is_base_of_v<Cmd, TCmd<TPlayer> > > >
	void add(std::string_view invocationText_, TCmd<TPlayer> cmd_)
	{
		return this->add( invocationText_, std::make_shared<TCmd>( std::move(cmd_) ) );
	}

	////////////////////////////////////
	void add(std::string_view invocationText_, typename FunctionCommandNoArgs<Player>::Func cmdFunc_)
	{
		return this->add<FunctionCommandNoArgs>( invocationText_, std::move(cmdFunc_) );
	}

	////////////////////////////////////
	void add(std::string_view invocationText_, typename FunctionCommand<Player>::Func cmdFunc_)
	{
		return this->add<FunctionCommand>( invocationText_, std::move(cmdFunc_) );
	}


	////////////////////////////////////
	// Base add methods:
	////////////////////////////////////
	void add(std::vector<std::string_view> const& aliases_, std::shared_ptr<Cmd> cmd_);

	void add(std::string_view invocationText_, std::shared_ptr<Cmd> cmd_);

	std::map<std::string_view, std::shared_ptr<Cmd>, IgnoreCaseLess> commands;
};

}

#include <SAMPCommandHandler/CommandHandler.inl>

