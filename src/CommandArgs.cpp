#include SAMPCOMMANDHANDLER_PCH

#include <SAMPCommandHandler/CommandArgs.hpp>

namespace samp_cmd
{


///////////////////////////////////////////////////////////////////////
CommandArgs CommandArgs::parse(std::string_view params_)
{
	CommandArgs args(params_);
	args.parseAll();
	return args;
}

///////////////////////////////////////////////////////////////////////
bool CommandArgs::parseNext()
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

///////////////////////////////////////////////////////////////////////
std::string_view CommandArgs::rest() const
{
	if (_lastParsedPos >= params.size())
		return {};
	return std::string_view(params.data() + _lastParsedPos, params.size() - _lastParsedPos);
}

///////////////////////////////////////////////////////////////////////
std::string_view CommandArgs::tail() const
{
	
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

///////////////////////////////////////////////////////////////////////
size_t CommandArgs::tryParse(size_t num_)
{
	size_t base = _numParsed;
	for(size_t i = base; i < base + num_; ++i)
	{
		if (!this->parseNext())
			return i - base;
	}
	
	return num_;
}

///////////////////////////////////////////////////////////////////////
size_t CommandArgs::parseAll()
{
	while(parseNext());

	return this->numParsed();
}

}