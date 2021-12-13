// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Delegates/Delegate.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

// Called when the inventory is changed and the UI needs an update . 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemsAdded UMETA(DisplayName = "No items added"),
	IAR_SomeItemsAdded UMETA(DisplayName = "Some items added"),
	IAR_AllItemsAdded UMETA(DisplayName = "All items added")
};

//Represents the result of adding an item to the inventory.
USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

public:

	FItemAddResult() {};
	FItemAddResult(int32 InItemQuantity) : AmountToGive(InItemQuantity), ActualAmountGiven(0) {};
	FItemAddResult(int32 InItemQuantity, int32 InQuantityAdded) : AmountToGive(InItemQuantity), ActualAmountGiven(InQuantityAdded) {};

	// The amount of the item that we tried to add
	UPROPERTY(BlueprintReadOnly, Category= "Item Add Result")
	int32 AmountToGive;

	// The amount of the item that was actually added in the end. Maybe we tried adding 10 items, but only 8 could be added because of capacity/weight?
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountGiven;

	// The result
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult Result;

	// If something went wrong, like we didn't have enough capacity or carrying too much weight this contains the reason why
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ErrorText;

	// Helpers
	static FItemAddResult AddedNone(const int32 InItemQuantity, const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult(InItemQuantity);
		AddedNoneResult.Result = EItemAddResult::IAR_NoItemsAdded;
		AddedNoneResult.ErrorText = ErrorText;

		return AddedNoneResult;
	}

	static FItemAddResult AddedSome(const int32 InItemQuantity, const int32 ActualAmountGiven, const FText& ErrorText)
	{
		FItemAddResult AddedSomeResult(InItemQuantity, ActualAmountGiven);
		AddedSomeResult.Result = EItemAddResult::IAR_SomeItemsAdded;
		AddedSomeResult.ErrorText = ErrorText;

		return AddedSomeResult;
	}

	static FItemAddResult AddedAll(const int32 InItemQuantity)
	{
		FItemAddResult AddedAllResult(InItemQuantity, InItemQuantity);

		AddedAllResult.Result = EItemAddResult::IAR_AllItemsAdded;
		
		return AddedAllResult;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

		friend class UItem;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	/** Add an item to the inventory.
	@param ErrorText the text to display if the item couldn't be added to the inventory
	@return the amount of the item that was added to the inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemAddResult TryAddItem(class UItem *Item);

	/** Add an item to the inventory using the item class instead of an item instance.
	@param ErrorText the text to display if the item couldn't be added to the inventory.
	@return the amount of the item that was added to the inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemAddResult TryAddItemFromClass(TSubclassOf<class UItem> ItemClass, const int32 Quantity);

	/** Take some quantity away from this item and remove it from the inventory when quantity reaches zero
	Useful for thing like eating food, using ammo etc */
	int32 ConsumeItem(class UItem* Item);
	int32 ConsumeItem(class UItem* Item, const int32 Quantity);

	/** Remove the item from the inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(class UItem *Item);

	/** Return true if we have a given amount of an item */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(TSubclassOf<UItem> ItemClass, const int32 Quantity = 1) const;

	/** Return the first item with the same class as a given Item */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItem *FindItem(class UItem *Item) const;

	/** Return the first item with the same class as ItemClass */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItem *FindItemByClass (TSubclassOf<UItem> ItemClass) const;

	/** Get all inventory items that are a child of ItemClass. Useful for grabbing all weapons, all food, etc.  */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<UItem*> FindItemsByClass(TSubclassOf<UItem> ItemClass) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetCurrentWeight() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetWeightCapacity(const float NewWeightCapacity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetCapacity(const int32 NewCapacity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE float GetWeightCapacity() const { return WeightCapacity; };

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE int32 GetCapacity() const { return Capacity; };

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FORCEINLINE TArray<class UItem*> GetItems() const {return Items; };

	UFUNCTION(Client, Reliable)
	void ClientRefreshInventory();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

protected:


	// The maximum weight the inventory can hold. For players, backpacks and other items increase this limit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	float WeightCapacity;

	// The maximum number of items the inventory can hold. For players, backpacks and other items increase this limit.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 0, ClampMax = 200))
	int32 Capacity;

	UPROPERTY(ReplicatedUsing = OnRep_Items, VisibleAnywhere, Category = "Inventory")
	TArray<class UItem*> Items;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;


private:

	/** Don't call Items.Add() directly, use this function instead, as it handles replication and ownership */
	UItem* AddItem(class UItem* Item);

	UFUNCTION()
	void OnRep_Items();

	UPROPERTY()
	int32 ReplicatedItemsKey;

	// Internal, non-BP exposed add item function. Don't call this directly, use TryAddItem(), or TryAddItemFromClass() instead.
	FItemAddResult TryAddItem_Internal(class UItem* Item);
};
