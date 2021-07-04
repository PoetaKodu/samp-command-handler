#include SAMPCOMMANDHANDLER_PCH

#include <SAMPCommandHandler/CommandHandler.hpp>

namespace samp_cmd
{

template class CommandBase<samp_cpp::Player>;
template class FunctionCommand<samp_cpp::Player>;
template class CommandHandler<samp_cpp::Player>;

}