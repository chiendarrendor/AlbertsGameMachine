// Standalone game-process executable for MerchantOfVenus -- Phase 1B of
// .claude/server_game_interface_spec.md. Everything real lives in
// gamecommon/GameServerMain.hpp; see that header's own top comment for
// what this is doing and why.

#define GAME_SERVER_NAME() MerchantOfVenus
#define GAME_SERVER_AUTORECURSION_DEPTH 100
#define GAME_SERVER_MAIN
#include "GameServerMain.hpp"
