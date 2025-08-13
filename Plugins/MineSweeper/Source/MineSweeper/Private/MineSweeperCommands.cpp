// Copyright Epic Games, Inc. All Rights Reserved.

#include "MineSweeperCommands.h"

#define LOCTEXT_NAMESPACE "FMineSweeperModule"

void FMineSweeperCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MineSweeper", "Execute MineSweeper action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
