// Copyright 2015-2020 Piperift. All Rights Reserved.
#pragma once

#include <EditorUndoClient.h>
#include <IPropertyTypeCustomization.h>
#include <IStructureDetailsView.h>
#include <PropertyHandle.h>
#include <Widgets/Views/SListView.h>


struct FFactionListItem
{
	FFactionListItem() : Id{INDEX_NONE} {}
	FFactionListItem(int32 Id, const TSharedPtr<IPropertyHandle>& Property) : Id{Id}, Property{Property} {}

	int32 Id;
	TSharedPtr<IPropertyHandle> Property;


	TSharedPtr<IPropertyHandle> GetKeyProperty() const
	{
		return Property->GetKeyHandle();
	}

	TSharedPtr<IPropertyHandle> GetValueProperty() const
	{
		return Property;
	}

	TSharedPtr<IPropertyHandle> GetSelfAttitudeProperty() const
	{
		return Property->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionDescriptor, SelfAttitude));
	}

	TSharedPtr<IPropertyHandle> GetExternalAttitudeProperty() const
	{
		return Property->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionDescriptor, ExternalAttitude));
	}

	TSharedPtr<IPropertyHandle> GetColorProperty() const
	{
		return Property->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionDescriptor, Color));
	}

	FName GetName() const
	{
		FName Name;
		GetKeyProperty()->GetValue(Name);
		return Name;
	}

	FString GetDisplayName() const
	{
		FString DisplayName;
		Property->GetChildHandle(GET_MEMBER_NAME_CHECKED(FFactionDescriptor, DisplayName))
			->GetValue(DisplayName);
		return DisplayName;
	}

	bool IsValid() const
	{
		return Id != INDEX_NONE && Property.IsValid();
	}
};

using FFactionListItemPtr = TSharedPtr<FFactionListItem>;


class FFactionTableCustomization : public IPropertyTypeCustomization, public FEditorUndoClient
{
public:
	FFactionTableCustomization() : Filter{""} {};

	/**
	 * Creates a new instance.
	 *
	 * @return A new struct customization for Anchor Type.
	 */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
		class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	~FFactionTableCustomization();

	//~ Begin FEditorUndoClient Interface
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	//~ End FEditorUndoClient Interface

private:
	void SetSelection(
		FFactionListItemPtr InNewSelection, ESelectInfo::Type InSelectInfo = ESelectInfo::Direct);

	void RefreshList();
	TSharedRef<ITableRow> MakeListRow(FFactionListItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnFilterChanged(const FText& Text);


	void OnNewFaction();

public:
	FReply OnDeleteFaction(FFactionListItemPtr Faction);
	void OnFactionIdChange(
		const FText& NewIdText, ETextCommit::Type CommitInfo, const FFactionListItemPtr& Item);

private:
	void OnClearFactions();

	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);

	UObject* GetOuter() const;

	FText GetHeaderValueText() const;

	bool UpdateSelectedCopy();


	/** Handle to the struct properties being customized */
	TSharedPtr<IPropertyHandle> StructHandle;
	TSharedPtr<IPropertyHandle> DescriptorsHandle;


	/** Array of the factions that are available for editing */
	TArray<FFactionListItemPtr> AvailableFactions;

	/** Array of the factions that are available for editing */
	TArray<FFactionListItemPtr> VisibleFactions;

	/** List view responsible for showing the rows in VisibleRows for each entry in Factions */
	TSharedPtr<SListView<FFactionListItemPtr>> ListView;

	TWeakPtr<FFactionListItem> CurrentSelection;

	FString Filter;

	TSharedPtr<IStructureDetailsView> DescriptorDetailsView;
	// We hold a copied struct for editing, then assign it after changes
	// This is due to StructureDetailsView not notifying the owner object of changes
	TSharedPtr<FStructOnScope> SelectedDescriptorCopy;
	FSimpleDelegate OnSelectedDescriptorChanged;

	FSimpleDelegate OnItemsNumChanged;

public:
	static const FName ColumnDelete;
	static const FName ColumnId;
	static const FName ColumnSelfAttitude;
	static const FName ColumnExternalAttitude;
	static const FName ColumnColor;
};
