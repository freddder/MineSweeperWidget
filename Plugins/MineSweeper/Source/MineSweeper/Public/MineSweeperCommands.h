// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "MineSweeperStyle.h"

class FMineSweeperCommands : public TCommands<FMineSweeperCommands>
{
public:

	FMineSweeperCommands()
		: TCommands<FMineSweeperCommands>(TEXT("MineSweeper"), NSLOCTEXT("Contexts", "MineSweeper", "MineSweeper Plugin"), NAME_None, FMineSweeperStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
