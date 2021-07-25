#pragma once

#include SAMPCOMMANDHANDLER_PCH

#include <SAMPCommandHandler/CommandHandler.hpp>

namespace samp_cmd
{

///////////////////////////////
template <typename TPlayer>
bool CommandHandler<TPlayer>::handleCommandText(int32_t playerIdx_, std::string_view command_)
{
	std::string_view invocation;

	auto spacePos = command_.find_first_of(" \t\r\n");
	if (spacePos != std::string::npos)
		invocation = command_.substr(1, spacePos - 1);
	else
		invocation = command_.substr(1);

	auto it = commands.find(invocation);

	if (it != commands.end())
	{
		it->second->invoke( Player{playerIdx_}, CommandArgs(command_) );
		return true;
	}
	return false;
}

///////////////////////////////
template <typename TPlayer>
void CommandHandler<TPlayer>::add(std::vector<std::string_view> const& aliases_, std::shared_ptr<Cmd> cmd_)
{
	for(auto const& alias : aliases_)
		commands[alias] = cmd_;
}
	
///////////////////////////////
template <typename TPlayer>
void CommandHandler<TPlayer>::add(std::string_view invocationText_, std::shared_ptr<Cmd> cmd_)
{
	commands[invocationText_] = std::move(cmd_);
}


}