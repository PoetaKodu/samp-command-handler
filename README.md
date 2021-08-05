# SAMP Command Handler

A command handler for SAMP in C++.

## 🔽 Installation

Use [**PACC**](https://github.com/PoetaKodu/pacc) package manager.

### Add to dependencies

Add `samp-command-handler@0.1.0` to `dependencies` field of your `cpackage.json`:
```json
"dependencies": [
	"samp-command-handler@0.1.0"
]
```

### Install packages

Run the following shell command:
```
pacc install
```

## 🎯 Usage

### 1. Initial configuration

Include the header file:

```cpp
#include <SAMPCommandHandler/Everything.hpp>
```

You probably want to add the following aliases:

```cpp
namespace samp = samp_cpp; // typical samp_cpp alias

using samp_cmd::CommandArgs;
using samp_cmd::CommandHandler;
```

Add `CommandHandler` instance to your game mode (the singleton is simply for the example purpose):

```cpp
// In "GameMode.hpp" - the declaration:
class GameMode
{
public:
	CommandHandler cmds;
};

GameMode& gameMode();

// In "GameMode.cpp" - the definition:
GameMode& gameMode()
{
	static GameMode instance;
	return instance;
}
```

Then inside `OnPlayerCommandText` handle the commands:

```cpp
PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandText(int playerid, const char *cmdtext_)
{										
	std::string_view cmdText = cmdtext_;

	auto& gm = gameMode();

	if (!gm.cmds.handleCommandText(playerid, cmdText))
		return false;

	return true;
}
```

### 2. Adding commands

Once you've configured the handler, you can add a command:

```cpp
// Declared before:

/// No additional parameters
void spawnInfernus(samp::Player player_);

/// With additional parameters
void sendPrivMsg(samp::Player player_, CommandArgs args_);

// The class definition:
class GameMode
{
public:
	GameMode() {
		this->setupCommands();
	}

	void setupCommands()
	{
		cmds.add( "infernus", 	spawnInfernus );
		cmds.add( "pm", 		sendPrivMsg );
	}

	CommandHandler cmds;
};
```

Then provide the command definitions:

```cpp
// For convenience:
using samp::colored;
using samp::colors::red;
using samp::colors::white;
using samp::colors::yellow;
using samp::colors::orange;

////////////////////////////////
void spawnInfernus(Player player_)
{
	if (player_.isInAnyVehicle())
	{
		player_.getVehicle().destroy();
	}

	auto veh = Vehicle::create(Vehicle::Infernus, player_.pos(), player_.rot(), 3, 4, 5000);

	player_.putInVehicle(veh, 0);
	player_.msg(colored(yellow,"You've spawned  ",white,"{}",yellow," for free!"), veh.prettyName());
}

////////////////////////////////
void sendPrivMsg(samp::Player player_, CommandArgs args_)
{
	if (args_.tryParse(1) != 1)
		return player_.msg(red,"Usage: /pm [id] [message content]");

	auto id = args_.get<int>(1); // returns optional<int>
	if (!id.has_value())
		return player_.msg(red,"Usage: /pm [id] [message content]");

	samp::Player other{*id};
	if (!other.valid())
		return player_.msg(colored(red,"Player ",white,"{}",red," is not connected"), *id);


	// Message other player:
	{
		other.msg(colored(yellow,"PRIVATE MESSAGE from ",white,"{} (id: {})"), player_.name(), player_.id());
		// tail() returns rest of the argument string
		// without spaces at the beginning
		other.msg(colored(white,"{}"), args_.tail());
	}

	// Inform self:
	{
		player_.msg(colored(orange,"PRIVATE MESSAGE to ",white,"{} (id: {})"), other.name(), other.id());
		player_.msg(colored(white,"{}"), args_.tail());
	}


	// PLEASE NOTE:
	// This command function is only an example.
	// A couple more checks should be performed.
}
```

### Commands from lambda functions:

```cpp
cmds.add( "healme",
	[](samp::Player p) {
		p.setHealth(100.f);
		p.msg(samp::colors::green, "Healed");
	}
);
```
## 👱‍♂️ Authors

Paweł Syska