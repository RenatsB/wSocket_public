// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "GameFramework/Actor.h"
#include "wSocketMapCellBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FThreePDelegateSign, const int32, _x, const int32, _y, FKey, _button);

UCLASS()
class WSOCKET_API AwSocketMapCellBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Minesweeper Events")
	FThreePDelegateSign CellClickDelegate;

	int32 m_x = 0;
	int32 m_y = 0;

	//if this is false, this means that cell was opened and no flagging can occur
	UPROPERTY(BlueprintReadOnly, Category = "CellValues")
	bool m_canFlag = true;

	//controls the flag state
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CellValues")
	bool m_isMarked = false;

	/**
	 * Based on the string filter in blueprints, a cell value can be:
	 * a single number [0-9] or a single "*" (mine).
	 * Numbers 0-4 are typically used to represent mine proximity, hence 8 is used as "closed".
	 * Number 9 is used to temporarily reassign all actual proximity zeroes during filtering.
	 * It is done because whitespace characters and newline character both hold a value of 0
	 * when converted to int. Additionally, number 7 is used to represent Empty value Error.
	 * To see how filtering works, check message filtering function in MainPanel Blueprint.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CellValues")
	FString m_value = "";


public:	
	// Sets default values for this actor's properties
	AwSocketMapCellBase();
	
	/**
	 * Forwards OnClick with altered broadcast parameters (X,Y and Button)
	 * @param TouchedActor - not really used, included just to match OnClick and be able to bind to it
	 * @param ButtonPressed - button ID that pressed on this actor
	 */
	UFUNCTION()
	void OnCellClicked(AActor* TouchedActor, FKey ButtonPressed);

	/**
	 * Blueprint-friendly and only blueprint-implementable method for updating any attached graphical elements
	 * Makes easier to change visual representation of a cell in Blueprint
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void updateCellGraphics();

	/**
	 * Sets this cell's values
	 * @param _x - cell X coordinate
	 * @param _y - cell Y coordinate
	 * @param _value - cell Display value
	 */
	UFUNCTION()
	void SetCell(const int32 _x, const int32 _y, const FString& _value);

	/**
	 * Sets only cell value. Typically used when X and Y were already assigned earlier.
	 */
	UFUNCTION()
	void SetValue(const FString& _value);

	/**
	 * Toggles cell flag state. Ignored if cell was opened before.
	 */
	UFUNCTION()
	void ToggleFlag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
