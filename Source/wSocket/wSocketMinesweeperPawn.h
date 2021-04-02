// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "wSocketMapCellBase.h"
#include "GameFramework/Pawn.h"
#include "wSocketMinesweeperPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTwoParamDelegateSign, const int32, _x, const int32, _y);

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom))
class WSOCKET_API AwSocketMinesweeperPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable, Category = "Minesweeper Events")
	FTwoParamDelegateSign OpenAtDelegate;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AwSocketMapCellBase>> m_map;
	int32 m_sizeX = 0;
	int32 m_sizeY = 0;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MinesweeperCell")
	TSubclassOf<AwSocketMapCellBase> CellBPReference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MinesweeperCell")
	float m_cellSize = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MinesweeperCell")
	float m_cellOffset = 0.05f;

public:
	// Sets default values for this pawn's properties
	AwSocketMinesweeperPawn();
	~AwSocketMinesweeperPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/**
	 * Clears, resizes and re-assigns map cells using stored dimension values.
	 */
	void initMap();

	/**
	 * Clears and unbinds map cells, resizes map to 0.
	 */
	void eraseMap();

	/**
	 * Convenient internal version of SetA(X,Y,Value), since map is stored as a 1-dimensional array
	 * @param _idx - cell index in m_map array
	 * @param _value - value assigned to cell, tipycally 1 character
	 */
	void setAt(const int32 _idx, const FString& _value);

	/**
	 * Binds all cell OnClick delegates to this pawn's ReceiveCellClick(X,Y,Button)
	 */
	void bindCellEvents();

	/**
	 * Unbinds all cell OnClick delegates from this pawn's ReceiveCellClick(X,Y,Button)
	 */
	void unbindCellEvents();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * Clears and re-initializes map using provided dimensions. BP friendly.
	 * @param _mapSizeX - number of cells in X (horizontal)
	 * @param _mapSizeY - number of cells in Y (Vertical)
	 */
	UFUNCTION(BlueprintCallable)
	void ResetMap(const int32 _mapSizeX, const int32 _mapSizeY);

	/**
	 * Toggles cell flag at XY coordinates, similar to original game.
	 * @param _x - cell position in X (Horizontal)
	 * @param _y - cell position in Y (Vertical)
	 */
	UFUNCTION(BlueprintCallable)
	void FlagAt(const int32 _x, const int32 _y);

	/**
	 * A public Blueprint-friendly interface for creating a web socket module.
	 * Assigns url and protocol and forwards to CreateSocket()
	 * @param _x - cell X coordinate
	 * @param _y - cell Y coordinate
	 * @param _value - value to assign. Can be any legnth, but typically just 1 character.
	 */
	UFUNCTION(BlueprintCallable)
	void SetAt(const int32 _x, const int32 _y, const FString& _value);

	/**
	 * Sets map values or completely resets current map, based on parameters.
	 * @param _in - map data, should be filtered as an array of characters without spaces and escape characters
	 * @param _reset - used to toggle map reset
	 * @param _newSizeX - new map size in X
	 * @param _newSizeY - new map size in Y
	 */
	UFUNCTION(BlueprintCallable)
	void SetMap(const FString& _in, const bool _reset, const int32 _newSizeX, const int32 _newSizeY);

	/**
	 * OnClick delegate each cell is bound to, passing its coordinates. Used to communicate with BP modules.
	 * @param _x - calling cell X coordinate
	 * @param _y - calling cell Y coordinate
	 */
	UFUNCTION()
	void ReceiveCellClick(const int32 _x, const int32 _y, FKey _button);
};
