// Copyright 2015-2018 Piperift. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Layout/Visibility.h"
#include "SListView.h"
#include "Input/Reply.h"
#include "PropertyHandle.h"
#include "EditorUndoClient.h"


class IDetailsView;
class IDetailLayoutBuilder;
class UFactionsSettings;

struct FFactionInfoMapItem {
	FFactionInfoMapItem() : Id{ INDEX_NONE }, Name{}, MapProperty{} {}
	FFactionInfoMapItem(int32 Id, FName Name, const TSharedPtr<IPropertyHandle>& Property)
		: Id{Id}, Name{Name}, MapProperty{Property}
	{}

	int32 Id;
	FName Name;
	TSharedPtr<IPropertyHandle> MapProperty;

	bool IsValid() const { return Id != INDEX_NONE && MapProperty.IsValid(); }

	TSharedPtr<IPropertyHandle> GetProperty() const {
		return MapProperty.IsValid() ? MapProperty->GetChildHandle(Id) : nullptr;
	}
};

class FFactionsSettingsDetails : public IDetailCustomization, public FEditorUndoClient
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	//~ End FEditorUndoClient Interface

	virtual ~FFactionsSettingsDetails();

private:

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void CustomizeFactionsDetails(IDetailLayoutBuilder& DetailBuilder);

	/** Make the widget for a row entry in the data table row list view */
	TSharedRef<ITableRow> MakeFactionWidget(const TSharedPtr<FFactionInfoMapItem> FactionName, const TSharedRef<STableViewBase>& OwnerTable);

	void SetFactionSelection(TSharedPtr<FFactionInfoMapItem> InNewSelection, ESelectInfo::Type InSelectInfo = ESelectInfo::Direct);

	void RefreshFactions();

	void OnFactionFilterChanged(const FText& Text);

	FReply OnNewFaction();
	FReply OnClearFactions();

public:

	void OnFactionIdChange(const FText& NewId, ETextCommit::Type CommitInfo, const TSharedPtr<FFactionInfoMapItem> Faction);
	FReply OnDeleteFaction(const TSharedPtr<FFactionInfoMapItem> Faction);


private:

	TWeakObjectPtr<UFactionsSettings> Settings;
	TSharedPtr<IPropertyHandle> FactionsMapHandle;


	/** Array of the relations that are available for editing */
	TArray<TSharedPtr<FFactionInfoMapItem>> AvailableFactions;

	/** Array of the relations that are available for editing */
	TArray<TSharedPtr<FFactionInfoMapItem>> VisibleFactions;

	/** List view responsible for showing the rows in VisibleRows for each entry in Factions */
	TSharedPtr<SListView<TSharedPtr<FFactionInfoMapItem>>> FactionsListView;

	TSharedPtr<SVerticalBox> FactionContainer;

	TWeakPtr<FFactionInfoMapItem> CurrentSelection;

	FString SearchFilter;

public:

	static const FName ColumnSelect;
	static const FName ColumnId;
	static const FName ColumnColor;
	static const FName ColumnDelete;
};
