// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "wSocketGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WSOCKET_API UwSocketGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
