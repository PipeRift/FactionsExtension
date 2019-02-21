// Copyright 2015-2019 Piperift. All Rights Reserved.
#pragma once

#include <IPropertyTypeCustomization.h>
#include <PropertyHandle.h>
#include <EditorUndoClient.h>
#include <Widgets/Views/SListView.h>


struct FFactionViewItem {
	FFactionViewItem() : Id{ INDEX_NONE }, ItemProperty{} {}
	FFactionViewItem(int32 Id, const TSharedPtr<IPropertyHandle>& Property)
		: Id{ Id }, ItemProperty{ Property }
	{}

	int32 Id;
	TSharedPtr<IPropertyHandle> ItemProperty;


	TSharedPtr<IPropertyHandle> GetKeyProperty() const {
		return ItemProperty->GetKeyHandle();
	}

	TSharedPtr<IPropertyHandle> GetValueProperty() const {
		return ItemProperty;
	}

	TSharedPtr<IPropertyHandle> GetColorProperty() const {
		return ItemProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, Color));
	}

	FName GetName() const {
		FName Name;
		GetKeyProperty()->GetValue(Name);
		return Name;
	}

	FString GetDisplayName() const {
		FString DisplayName;
		ItemProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionInfo, DisplayName))->GetValue(DisplayName);
		return DisplayName;
	}

	bool IsValid() const { return Id != INDEX_NONE && ItemProperty.IsValid(); }
};

using FFactionViewItemPtr = TSharedPtr<FFactionViewItem>;


class FFactionTableCustomization : public IPropertyTypeCustomization, public FEditorUndoClient
{
public:

	FFactionTableCustomization()
		: Filter{ "" }
	{};

	/**
	* Creates a new instance.
	*
	* @return A new struct customization for Anchor Type.
	*/
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	~FFactionTableCustomization();

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	//~ End FEditorUndoClient Interface

private:

	TSharedRef<SWidget> CreateFactionWidget(TSharedRef<IPropertyHandle> PropertyHandle);

	/** Make the widget for a row entry in the data table row list view */
	TSharedRef<ITableRow> MakeFactionWidget(FFactionViewItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnSelected(FFactionViewItemPtr InNewSelection, ESelectInfo::Type InSelectInfo = ESelectInfo::Direct);

	void SetSelection(FFactionViewItemPtr InNewSelection, ESelectInfo::Type InSelectInfo = ESelectInfo::Direct);


	void RefreshView();

	void OnFilterChanged(const FText& Text);


	FReply OnNewFaction();

public:

	FReply OnDeleteFaction(FFactionViewItemPtr Faction);
	void OnFactionIdChange(const FText& NewIdText, ETextCommit::Type CommitInfo, const FFactionViewItemPtr& Item);

private:

	FReply OnClearFactions();

	UObject* GetOuter() const;

	FText GetHeaderValueText() const;


	/** Handle to the struct properties being customized */
	TSharedPtr<IPropertyHandle> StructHandle;
	TSharedPtr<IPropertyHandle> ItemsHandle;
	TSharedPtr<IPropertyHandleMap> ItemsHandleMap;


	/** Array of the factions that are available for editing */
	TArray<FFactionViewItemPtr> AvailableFactions;

	/** Array of the factions that are available for editing */
	TArray<FFactionViewItemPtr> VisibleFactions;

	/** List view responsible for showing the rows in VisibleRows for each entry in Factions */
	TSharedPtr<SListView<FFactionViewItemPtr>> ListView;

	TWeakPtr<FFactionViewItem> CurrentSelection;

	FString Filter;

	TSharedPtr<SVerticalBox> FactionInfoContainer;

	FSimpleDelegate OnItemsNumChanged;

public:

	static const FName ColumnDelete;
	static const FName ColumnId;
	static const FName ColumnColor;
};

