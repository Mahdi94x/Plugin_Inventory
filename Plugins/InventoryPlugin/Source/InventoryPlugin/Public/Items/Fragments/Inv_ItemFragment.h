#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inv_ItemFragment.generated.h"

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
	
private:
	UPROPERTY(EditAnywhere, category = "Inventory")
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};


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

USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_ItemFragmentBase
{
	GENERATED_BODY()
	
	UTexture2D* GetIcon() {return this->Icon;}
	
private:
	
	UPROPERTY(EditAnywhere, category = "Inventory")
	TObjectPtr<UTexture2D> Icon{nullptr};
	
	UPROPERTY(EditAnywhere, category = "Inventory")
	FVector2D IconDimension{44.f,44.f};
};

