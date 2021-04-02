// Fill out your copyright notice in the Description page of Project Settings.

//Note: You can disable UE_LOG messages by commenting out Definitions.Add("UE_LOG_ENABLED"); in wSocket.Build.cs

#include "wSocketMapCellBase.h"

// Sets default values
AwSocketMapCellBase::AwSocketMapCellBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AwSocketMapCellBase::BeginPlay()
{
	Super::BeginPlay();
	OnClicked.AddDynamic(this, &AwSocketMapCellBase::OnCellClicked);
}

// Called every frame
void AwSocketMapCellBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AwSocketMapCellBase::OnCellClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	if (m_canFlag)
	{
#ifdef UE_LOG_ENABLED
		UE_LOG(LogTemp, Warning, TEXT("Cell %s received click at X: %d, Y: %d"), *(this->GetName()), m_x, m_y);
#endif
		CellClickDelegate.Broadcast(m_x, m_y, ButtonPressed);
	}
}

void AwSocketMapCellBase::SetCell(const int32 _x, const int32 _y, const FString& _value)
{
	m_x = _x;
	m_y = _y;
	m_value = _value;
	updateCellGraphics();
}

void AwSocketMapCellBase::SetValue(const FString& _value)
{
	if (_value != "8") //8 is used as "not open", see class declaration for more info
	{
		m_isMarked = false;
		m_canFlag = false;
		m_value = _value;
	}
	updateCellGraphics();
}

void AwSocketMapCellBase::ToggleFlag()
{
	if (m_canFlag)
	{
		m_isMarked = !m_isMarked;
	}
	updateCellGraphics();
}