// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#include "Widgets/Input/SNumericEntryBox.h"

class FToolBarBuilder;
class FMenuBuilder;

struct FMineFieldTile
{
	TSharedPtr<STextBlock> Text;
	bool IsRevealed;
	bool HasMine;
};

class FMineSweeperModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void OpenMineSweepWindow();
	
private:

	void RegisterMenus();

	TSharedRef<SDockTab> CreateTab(const FSpawnTabArgs& SpawnTabArgs);

	TSharedPtr<class FUICommandList> PluginCommands;


private:
	const int32 MAX_GRID_HEIGHT_AND_WIDTH = 30;
	const int32 DEFAULT_GRID_HEIGHT = 20;
	const int32 DEFAULT_GRID_WIDTH = 20;
	const int32 DEFAULT_NUMBER_OF_MINES = 50;

	int32 CurrGridHeight;
	int32 CurrGridWidth;

	TSharedPtr<STextBlock> GameOverMessage;
	TSharedPtr<SSpinBox<int32>> HeightSpinBox;
	TSharedPtr<SSpinBox<int32>> WidthSpinBox;
	TSharedPtr<SSpinBox<int32>> MinesNumSpinBox;

	TSharedPtr<SGridPanel> ButtonGrid;
	TArray<FMineFieldTile> MineFieldData;

	void UpdateMaxNumberOfMines();
	void GenerateMinefield();
	TSharedRef<SWidget> MakeMineButton(const int32& Index);

	bool RevealAtIndex(const int32 Index);
};
