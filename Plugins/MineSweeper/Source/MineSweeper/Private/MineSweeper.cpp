// Copyright Epic Games, Inc. All Rights Reserved.

#include "MineSweeper.h"
#include "MineSweeperStyle.h"
#include "MineSweeperCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "Algo/RandomShuffle.h"

static const FName MineSweeperTabName("MineSweeper");

#define LOCTEXT_NAMESPACE "FMineSweeperModule"

void FMineSweeperModule::StartupModule()
{
	// Register tab spawner
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner("MineSweeper",
		FOnSpawnTab::CreateRaw(this, &FMineSweeperModule::CreateTab))
		.SetDisplayName(FText::FromString("Mine Sweeper"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMineSweeperStyle::Initialize();
	FMineSweeperStyle::ReloadTextures();

	FMineSweeperCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMineSweeperCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMineSweeperModule::OpenMineSweepWindow),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMineSweeperModule::RegisterMenus));
}

void FMineSweeperModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("MineSweeper");

	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMineSweeperStyle::Shutdown();

	FMineSweeperCommands::Unregister();
}

void FMineSweeperModule::OpenMineSweepWindow()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FTabId("MineSweeper"));
}

TSharedRef<SDockTab> FMineSweeperModule::CreateTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
                .Padding(5)
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Width: "))
                        ]
                        + SHorizontalBox::Slot()
                        .MaxWidth(100)
                        [
                            SAssignNew(WidthSpinBox, SSpinBox<int32>)
                                .MinValue(1)
                                .MaxValue(MAX_GRID_HEIGHT_AND_WIDTH)
                                .Value(DEFAULT_GRID_WIDTH)
                                .OnValueChanged_Lambda([this](int32 NewValue) {
                                    UpdateMaxNumberOfMines();
                                })
                        ]
                ]
            + SVerticalBox::Slot()
                .Padding(5)
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Height: "))
                        ]
                        + SHorizontalBox::Slot()
                        .MaxWidth(100)
                        [
                            SAssignNew(HeightSpinBox, SSpinBox<int32>)
                                .MinValue(1)
                                .MaxValue(MAX_GRID_HEIGHT_AND_WIDTH)
                                .Value(DEFAULT_GRID_HEIGHT)
                                .OnValueChanged_Lambda([this](int32 NewValue) {
                                    UpdateMaxNumberOfMines();
                                })
                        ]
                ]
            + SVerticalBox::Slot()
                .Padding(5)
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
                                .Text(FText::FromString("Number of Mines: "))
                        ]
                        + SHorizontalBox::Slot()
                        .MaxWidth(100)
                        [
                            SAssignNew(MinesNumSpinBox, SSpinBox<int32>)
                                .MinValue(1)
                                .MaxValue(DEFAULT_GRID_HEIGHT * DEFAULT_GRID_WIDTH - 1)
                                .Value(DEFAULT_NUMBER_OF_MINES)
                        ]
                ]
            + SVerticalBox::Slot()
                .Padding(5)
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                                .Text(FText::FromString("Generate"))
                                .OnClicked_Lambda([this]() {
                                    GenerateMinefield();
                                    return FReply::Handled();
                                })
                        ]
                ]
            + SVerticalBox::Slot()
                .Padding(5)
                .AutoHeight()
                [
                    SAssignNew(ButtonGrid, SGridPanel)
                    .Visibility(EVisibility::Collapsed)
                ]
            + SVerticalBox::Slot()
                .Padding(5)
                .AutoHeight()
                [
                    SAssignNew(GameOverMessage, STextBlock)
                        .Text(FText::FromString("Game Over"))
                        .ColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
                        .Visibility(EVisibility::Collapsed)
                ]
		];
}

void FMineSweeperModule::UpdateMaxNumberOfMines()
{
    int32 InputWidth = WidthSpinBox->GetValue();
    int32 InputHeight = HeightSpinBox->GetValue();
    int32 MaxMineNum = InputWidth * InputHeight - 1;

    MinesNumSpinBox->SetMaxSliderValue(MaxMineNum);
    MinesNumSpinBox->SetMaxValue(MaxMineNum);

    if (MinesNumSpinBox->GetValue() > MaxMineNum)
    {
        MinesNumSpinBox->SetValue(MaxMineNum);
    }
}

void FMineSweeperModule::GenerateMinefield()
{
    GameOverMessage->SetVisibility(EVisibility::Collapsed);
    ButtonGrid->ClearChildren();
    MineFieldData.Empty();

    CurrGridWidth = WidthSpinBox->GetValue();
    CurrGridHeight = HeightSpinBox->GetValue();

    TArray<int32> MineIndexes;
    for (int32 y = 0; y < CurrGridHeight; y++)
    {
        for (int32 x = 0; x < CurrGridWidth; x++)
        {
            int Index = CurrGridWidth * y + x;
            MineIndexes.Add(Index);
            ButtonGrid->AddSlot(x, y)
            [
                MakeMineButton(Index)
            ];
        }
    }

    // Add mines
    Algo::RandomShuffle(MineIndexes);
    int32 NumOfMines = MinesNumSpinBox->GetValue();
    for (int32 i = 0; i < NumOfMines; i++)
    {
        MineFieldData[MineIndexes[i]].HasMine = true;
    }

    ButtonGrid->SetVisibility(EVisibility::Visible);
}

TSharedRef<SWidget> FMineSweeperModule::MakeMineButton(const int32& Index)
{
    FMineFieldTile& NewTile = MineFieldData.AddDefaulted_GetRef();
    NewTile.HasMine = false;
    NewTile.IsRevealed = false;

    return SNew(SBox)
        [
            SNew(SButton)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                .OnClicked_Lambda([this, Index]() {
                    RevealAtIndex(Index);
                    return FReply::Handled();
                })
                [
                    SAssignNew(NewTile.Text, STextBlock)
                        .Text(FText::FromString(" "))
                ]
        ];
}

bool FMineSweeperModule::RevealAtIndex(const int32 Index)
{
    if (Index < 0 || Index >= CurrGridHeight * CurrGridWidth)
        return false;

    FMineFieldTile& TileToReveal = MineFieldData[Index];

    if (TileToReveal.IsRevealed)
        return TileToReveal.HasMine;
    UE_LOG(LogTemp, Warning, TEXT("%d"), Index);

    TileToReveal.IsRevealed = true;

    // If clicked on mine
    if (TileToReveal.HasMine)
    {
        TileToReveal.Text->SetText(FText::FromString("X"));
        GameOverMessage->SetVisibility(EVisibility::Visible);
        return true;
    }

    // Check if there is mines around tile
    int32 AdjecentMinesCounter = 0;
    bool IsOnTopEdge = Index < CurrGridWidth;
    bool IsOnBottomEdge = Index > CurrGridWidth * (CurrGridHeight - 1) - 1;
    bool IsOnLeftEdge = Index % CurrGridWidth == 0;
    bool IsOnRightEdge = Index % CurrGridWidth == CurrGridWidth - 1;

    for (int32 OffsetX = -1; OffsetX <= 1; OffsetX++)
    {
        for (int32 OffsetY = -1; OffsetY <= 1; OffsetY++)
        {
            if (OffsetX == 0 && OffsetY == 0) continue;

            int32 IndexToCheck = Index;

            // Left
            if (OffsetX == -1)
            {
                if (IsOnLeftEdge) continue;
                else IndexToCheck -= 1;
            }

            // Right
            if (OffsetX == 1)
            {
                if (IsOnRightEdge) continue;
                else IndexToCheck += 1;
            }

            // Top
            if (OffsetY == -1)
            {
                if (IsOnTopEdge) continue;
                else IndexToCheck -= CurrGridWidth;
            }

            // Bottom
            if (OffsetY == 1)
            {
                if (IsOnBottomEdge) continue;
                else IndexToCheck += CurrGridWidth;
            }

            if (MineFieldData[IndexToCheck].HasMine) 
                AdjecentMinesCounter++;
        }
    }

    if (AdjecentMinesCounter != 0)
    {
        TileToReveal.Text->SetText(FText::AsNumber(AdjecentMinesCounter));
        return false;
    }

    // No mines around. Reveal tiles top, bottom, left and right
    TileToReveal.Text->SetText(FText::FromString("-"));

    if (!IsOnLeftEdge) RevealAtIndex(Index - 1);
    if (!IsOnRightEdge) RevealAtIndex(Index + 1);
    if (!IsOnTopEdge) RevealAtIndex(Index - CurrGridWidth);
    if (!IsOnBottomEdge) RevealAtIndex(Index + CurrGridWidth);

    return false;
}

void FMineSweeperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMineSweeperCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMineSweeperCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMineSweeperModule, MineSweeper)