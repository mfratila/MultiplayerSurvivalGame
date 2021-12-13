// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Delegates/Delegate.h"
#include "InteractionComponent.generated.h"

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

		FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		bInteractHeld = false;
	}

	// The current interactable component we're viewing, if there is one
	UPROPERTY()
	class UInteractionComponent *ViewedInteractionComponent;

	//The Line when we last checked for an interactable
	UPROPERTY()
	float LastInteractionCheckTime;

	// Whether the local player is holding the interact key
	UPROPERTY()
	bool bInteractHeld;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class ASurvivalCharacter*, Character);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SURVIVALGAME_API UInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	UInteractionComponent();

	// The time the player must hold the interact key to interact with this object
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionTime;

	// The max distance the player can be away from this actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance;

	// The name that will come up when the player looks at the interactable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableNameText;

	// The verb that describes how the interaction works, ie "Sit" for a chair, "Eat" for foot, "Light" for a fireplace
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableActionText;

	// Whether we allow multiple players to interact with the item, or just one at any given time.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bAllowMultipleInteractors;

	// Call this to change the name of the interactable. Will also refresh the interaction widget.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractableNameText(const FText& NewNameText);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractableActionText(const FText& NewActionText);
	
	// Delegates

	//[local + server] Called when the player presses the interact key whilst focusing on this interactable actor
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;

	//[local + server] Called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndInteract OnEndInteract;

	//[local + server] Called when the player presses the interact key whilst focusing on this interactable actor
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;

	//[local + server] Called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndFocus OnEndFocus;

	//[local + server] Called when the player has interacted with the item for the required amount of time
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnInteract OnInteract;

protected:
	// Called when the game starts
	virtual void Deactivate() override;

	bool CanInteract(class ASurvivalCharacter* Character) const;

	// On the server, this will hold all the interactors. On the local player, this will just hold the local player (provided they are an interactor)
	UPROPERTY()
	TArray<class ASurvivalCharacter*> Interactors;


public:

	/*** Refresh the interaction widget and its custom widgets.
	* An example of when we'd use this is when we take 3 items out of a stack of 10, we need to update the widget
	* so it shows the stack as having 7 items left */
	void RefreshWidget();
	
    // Called on the client when the players interaction check begins/ends hitting this item
	void BeginFocus(class ASurvivalCharacter* Character);
	void EndFocus(class ASurvivalCharacter* Character);

    // Called on the client when the player begins/end interaction with the item
	void BeginInteract(class ASurvivalCharacter* Character);
	void EndInteract(class ASurvivalCharacter* Character);

	void Interact(class ASurvivalCharacter* Character);

	//Return a value from 0-1 denoting how far through the interact we are.
	//One server this is the first interactors percentage, on client this is the local interactors percentage
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetInteractPercentage();


};
