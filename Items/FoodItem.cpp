// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/FoodItem.h"

#define  LOCTEXT_NAMESPACE "FoodItem"


UFoodItem::UFoodItem()
{
	healAmount = 20.f;
	UseActionText = LOCTEXT("ItemUseActionText", "Consume");
}

void UFoodItem::Use(class ASurvivalCharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("Nom Nom, I ate some food!"));
}

#undef LOCTEXT_NAMESPACE