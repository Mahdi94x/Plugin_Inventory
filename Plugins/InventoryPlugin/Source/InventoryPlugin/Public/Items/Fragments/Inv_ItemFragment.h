#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemFragment.generated.h"

class UInv_CompositeBase;
class APlayerController;
/*====================================================================================================================*/
USTRUCT(BlueprintType)
struct FInv_ItemFragmentBase
{
	GENERATED_BODY()
	
	FInv_ItemFragmentBase() {}
	/*copy operation*/
	FInv_ItemFragmentBase(const FInv_ItemFragmentBase&) = default;
	FInv_ItemFragmentBase& operator=(const FInv_ItemFragmentBase&) = default;
	/*move operation*/
	FInv_ItemFragmentBase(FInv_ItemFragmentBase&&) = default;
	FInv_ItemFragmentBase& operator=(FInv_ItemFragmentBase&&) = default;
	virtual ~FInv_ItemFragmentBase() {}
	
	FGameplayTag GetFragmentTag() const {return this->FragmentTag;}
	void SetGameplayTag(const FGameplayTag Tag) {this->FragmentTag = Tag;}
	
	virtual void FragmentManifest() {}
	
private:
	UPROPERTY(EditAnywhere, category = "Inventory", meta = (Categories = "FragmentTags"))
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};
/*====================================================================================================================*/
USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragmentBase
{
	GENERATED_BODY()
	
	FIntPoint GetGridSize() const {return this->GridSize;}
	void SetGridSize(const FIntPoint Size) {this->GridSize = Size;}
	
	float GetGridPadding() const {return this->GridPadding;}
	void SetGridPadding(const float Padding) {this->GridPadding = Padding;}
	
private:
	
	UPROPERTY(EditAnywhere, category = "Inventory")
	FIntPoint GridSize{1,1};
	
	UPROPERTY(EditAnywhere, category = "Inventory")
	float GridPadding{0.f};
};
/*====================================================================================================================*/
USTRUCT(BlueprintType)
struct FInv_StackableFragment : public FInv_ItemFragmentBase
{
	GENERATED_BODY()
	
	int32 GetMaxStackSize() const {return this->MaxStackSize;}
	int32 GetStackCount() const {return this->StackCount;}
	void SetStackCount(const int32 NewCount) {this->StackCount = NewCount;}
	
private:
	
	UPROPERTY(EditAnywhere, category = "Inventory")
	int32 MaxStackSize{1};
	
	UPROPERTY(EditAnywhere, category = "Inventory")
	int32 StackCount{1};
};
/*====================================================================================================================*/
/*
 * FInv_InventoryItemFragment can assimilate into a widget
 * CompositeBase
 */
USTRUCT(BlueprintType)
struct FInv_InventoryItemFragment : public FInv_ItemFragmentBase
{
	GENERATED_BODY()
	virtual void Assimilate(UInv_CompositeBase* Composite) const;
	
protected:
	bool MatchesWidgetTag(const UInv_CompositeBase* Composite) const;
};
/*====================================================================================================================*/
USTRUCT(BlueprintType)
struct FInv_IconFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	UTexture2D* GetIcon() const {return this->IconTexture;}
	
private:
	
	UPROPERTY(EditAnywhere, category = "Inventory")
	TObjectPtr<UTexture2D> IconTexture{nullptr};
	
	UPROPERTY(EditAnywhere, category = "Inventory")
	FVector2D IconDimension{44.f,44.f};
};
/*====================================================================================================================*/
USTRUCT(BlueprintType)
struct FInv_TextFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	void SetText(const FText& Text) {this->FragmentText = Text;}
	
private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;
};
/*====================================================================================================================*/
USTRUCT(BlueprintType)
struct FInv_LabeledNumberFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual void FragmentManifest() override;
	float GetValue() const {return this->Value;}
	
	/*When manifesting for the first time only, this fragment will randomize the value*/
	bool bRandomizeOnManifest{true};
	
private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText Text_Label{};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bShouldCollapseLabel{false};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MinValue{0.f};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MaxValue{0.f};
	
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{0.f};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bShouldCollapseValue{false};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{1};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{1};
};
/*====================================================================================================================*/
/*
 *Consume Modifier "Sub_Fragment" "Leaf"
 */
USTRUCT(BlueprintType)
struct FInv_ConsumeModifier : public FInv_LabeledNumberFragment
{
	GENERATED_BODY()
	virtual void OnConsume(APlayerController* PC)
	{
	}
};
/*====================================================================================================================*/
/*
 *FInv_ConsumableFragment "Composite, has an array of leaves"
 */
USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	virtual void OnConsume(APlayerController* PC);
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual void FragmentManifest() override;

private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	TArray<TInstancedStruct<FInv_ConsumeModifier>> ConsumeModifiers;
};
/*====================================================================================================================*/
USTRUCT(BlueprintType)
struct FInv_HealthPotionFragment : public FInv_ConsumeModifier
{
	GENERATED_BODY()
	virtual void OnConsume(APlayerController* PC) override;
};
/*====================================================================================================================*/
USTRUCT(BlueprintType)
struct FInv_ManaPotionFragment : public FInv_ConsumeModifier
{
	GENERATED_BODY()
	virtual void OnConsume(APlayerController* PC) override;
};
/*====================================================================================================================*/