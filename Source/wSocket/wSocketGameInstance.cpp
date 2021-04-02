// Fill out your copyright notice in the Description page of Project Settings.
#include "wSocketGameInstance.h"
#include "WebSocketsModule.h"

void UwSocketGameInstance::Init()
{
    Super::Init();

    FModuleManager& fMoMan = FModuleManager::Get();
    // Load the WebSockets module. An assertion will fail if it isn't found.
    IModuleInterface Module = fMoMan.LoadModuleChecked(TEXT("WebSockets"));
}