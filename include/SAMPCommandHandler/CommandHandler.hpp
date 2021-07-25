#pragma once

#include SAMPCOMMANDHANDLER_PCH

#include <SAMPCommandHandler/Helper/CaseInsensitiveCompare.hpp>

namespace samp_cmd
{

constexpr size_t MaxCommandArgs = 256;

struct CommandArgs
	: public std::array<std::string_view, MaxCommandArgs>
{
	using Super = std::array<std::string_view, MaxCommandArgs>;

	std::string_view params;

	CommandArgs() = default;
	explicit CommandArgs(std::string_view params_)
		: params(params_)
	{
	}

	static CommandArgs parse(std::string_view params_)
	{
		CommandArgs args(params_);
		args.parseAll();
		return args;
	}

	bool parseNext()
	{
		if (_lastParsedPos >= params.size())
			return false;
		
		while( std::isspace(static_cast<int>( params[_lastParsedPos])) )
		{
			++_lastParsedPos;

			if (_lastParsedPos >= params.size())
				return false;
		}

		size_t startPos = _lastParsedPos;
		while(
				_lastParsedPos < params.size() &&
				!std::isspace(static_cast<int>( params[_lastParsedPos]))
			)
		{
			++_lastParsedPos;
		}

		(*this)[_numParsed++] = std::string_view(params.data() + startPos, _lastParsedPos - startPos);
		return true;
	}

	std::string_view rest() const {
		if (_lastParsedPos >= params.size())
			return {};
		return std::string_view(params.data() + _lastParsedPos, params.size() - _lastParsedPos);
	}
	
	std::string_view tail() const {
		
		if (_lastParsedPos >= params.size())
			return {};
		size_t tailStart = _lastParsedPos;
		while( std::isspace(static_cast<int>( params[tailStart])) )
		{
			++tailStart;

			if (tailStart >= params.size())
				return {};
		}
		
		return std::string_view(params.data() + tailStart, params.size() - tailStart);
	}

	// size_t size() const
	// {
	// 	return _numParsed;
	// }

	// auto begin() const 	{ return Super::begin(); }
	// auto end() const 	{ return Super::begin() + this->size(); }

	// auto begin() 		{ return Super::begin(); }
	// auto end() 			{ return Super::begin() + this->size(); }

	size_t tryParse(size_t num_) {
		size_t base = _numParsed;
		for(size_t i = base; i < base + num_; ++i)
		{
			if (!this->parseNext())
				return i - base;
		}
		
		return num_;
	}

	// template <typename T>
	// auto get() const {
	// 	return converter
	// }

	size_t parseAll()
	{
		while(parseNext());

		return this->numParsed();
	}

	size_t numParsed() const {
		return _numParsed;
	}

	operator std::string_view() 	const { return params; }
	operator std::string() 			const { return std::string(params); }

private:
	size_t _numParsed = 0;
	size_t _lastParsedPos = 0;
};


template <typename TPlayer = samp_cpp::Player>
class CommandBase
{
public:
	virtual ~CommandBase() {}
	virtual void invoke(TPlayer player_, CommandArgs args_) { }
};


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

template <typename TPlayer = samp_cpp::Player>
class FunctionCommand : public CommandBase<TPlayer>
{
public:
	using Func = std::function<void(TPlayer, CommandArgs)>;

	FunctionCommand() = default;

	FunctionCommand(Func func_) : func(std::move(func_))
	{
	}

	virtual void invoke(TPlayer player_, CommandArgs args_) { func(player_, args_); }

	Func func;
};



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

