// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class WSOCKET_API wSocketMapReader
{

	

public:
	wSocketMapReader();
	~wSocketMapReader();

	static TArray<FString> readMap(const FString& _map, int& _mapSizeOut);
};
