// Fill out your copyright notice in the Description page of Project Settings.

//Note: You can disable UE_LOG messages by commenting out Definitions.Add("UE_LOG_ENABLED"); in wSocket.Build.cs

#include "wSocketMinesweeperPawn.h"

// Sets default values
AwSocketMinesweeperPawn::AwSocketMinesweeperPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

AwSocketMinesweeperPawn::~AwSocketMinesweeperPawn()
{
	eraseMap();
}

// Called when the game starts or when spawned
void AwSocketMinesweeperPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AwSocketMinesweeperPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AwSocketMinesweeperPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AwSocketMinesweeperPawn::initMap()
{
	if (m_map.Num() > 0)
	{
		eraseMap();
	}
	int32 numItems = m_sizeX * m_sizeY;
	m_map.SetNum(numItems);
	for (int32 y = 0; y < m_sizeY; ++y)
	{
		for (int32 x = 0; x < m_sizeX; ++x)
		{
			//since Unreal's coordinate system is different from traditional, we have to do some magic here
			float posX = y * m_cellOffset - (m_sizeY * 0.5f * m_cellOffset - m_cellOffset * 0.5f);
			float posY = (m_sizeX * 0.5f * m_cellOffset - m_cellOffset * 0.5f) - x * m_cellOffset;
			//in normal coordinate system, UE X is Z, UE Y is X, UE Z is Y. but here this works I guess
			FVector Location(posX, posY, 40.f);
			FRotator Rotation(0.0f, 0.0f, 0.0f);
			FActorSpawnParameters SpawnInfo;
			TWeakObjectPtr<AwSocketMapCellBase> obj = GetWorld()->SpawnActor<AwSocketMapCellBase>(CellBPReference, Location, Rotation, SpawnInfo);
			int32 idx = y * m_sizeX + x;
			//a little note here - I am using "8" instead of saying "not opened", check wSocketCellBase for full description
			obj->SetCell(x, y, "8");
			m_map[idx] = obj;
		}
	}
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("InitMap() returned map with size %d"), m_map.Num());
#endif
	bindCellEvents();
}

void AwSocketMinesweeperPawn::eraseMap()
{
	if (m_map.Num() > 0)
	{
		//unbind before deleting, otherwise residue bindings might stay
		unbindCellEvents();
		for (auto &&cell : m_map)
		{
			if (cell.IsValid())
			{
				//this call can also crash so I first checked if it is not null
				if (!cell->IsPendingKill())
				{
					cell->Reset(); //since it is a tweakobjectptr
					cell->Destroy(); //apparently it is really hard to instantly destroy an object in UE
					cell = nullptr; //so to be sure I am doing it in 3 steps
				}
			}
		}
		m_map.Empty();
	}
}

void AwSocketMinesweeperPawn::bindCellEvents()
{
	for (auto&& cell : m_map)
	{
		if(cell.IsValid())
			cell->CellClickDelegate.AddDynamic(this, &AwSocketMinesweeperPawn::ReceiveCellClick);
	}
}

void AwSocketMinesweeperPawn::unbindCellEvents()
{
	for (auto&& cell : m_map)
	{
		//it appears that sometimes it can try to call methods on objects with pending kill, so here is a sanity check
		if (cell.IsValid())
		{
			//this call can also crash on its own so I first checked if it is not null
			if (!cell->IsPendingKill())
			{
				cell->CellClickDelegate.RemoveDynamic(this, &AwSocketMinesweeperPawn::ReceiveCellClick);
			}
		}
	}
}

void AwSocketMinesweeperPawn::ResetMap(const int32 _mapSizeX, const int32 _mapSizeY)
{
	m_sizeX = _mapSizeX;
	m_sizeY = _mapSizeY;
	initMap();
}

void AwSocketMinesweeperPawn::SetMap(const FString& _in, const bool _reset, const int32 _newSizeX, const int32 _newSizeY)
{
#ifdef UE_LOG_ENABLED
	if (_reset)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitMap(): _in size: %d, _reset: true, _newSizeX: %d, _newSizeY: %d"), _in.Len(), _newSizeX, _newSizeY);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitMap(): _in size: %d, _reset: false, _newSizeX: %d, _newSizeY: %d"), _in.Len(), _newSizeX, _newSizeY);
	}
#endif
	
	if (_reset)
	{
		eraseMap();
		ResetMap(_newSizeX, _newSizeY);
#ifdef UE_LOG_ENABLED
		UE_LOG(LogTemp, Warning, TEXT("InitMap() returned map with size %d, of mapsize: X = %d, Y = %d"), m_map.Num(), m_sizeX, m_sizeY);
#endif
	}
	if (_in.Len() < m_map.Num())
	{
		//size mismatch
#ifdef UE_LOG_ENABLED
		UE_LOG(LogTemp, Warning, TEXT("WARNING: SetMap -> _in (%d) < map (%d)!"), _in.Len(), m_map.Num());
#endif
		return;
	}
	if (_in.Len() > m_map.Num())
	{
#ifdef UE_LOG_ENABLED
		UE_LOG(LogTemp, Warning, TEXT("WARNING: SetMap -> _in (%d) > map (%d)!"), _in.Len(), m_map.Num());
#endif
		return;
	}
		
	FString local = _in;
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("Setting map..."));
#endif
	for (int32 i=0; i<m_map.Num(); ++i)
	{
		//Mid is a really weird name for what should just be "At()"
		//but I guess UE has very strange naming and counter-intuitive elements all over the place
		FString charAt = local.Mid(i, 1);
		if (charAt.IsEmpty())
		{
			charAt = "7"; //just to make it different and still reflect the error visually ([0-9] will be drawn)
		}
#ifdef UE_LOG_ENABLED
		UE_LOG(LogTemp, Warning, TEXT("Setting cell number: %d, with value %s"), i, *charAt);
#endif
		setAt(i, charAt);
	}
}

void AwSocketMinesweeperPawn::ReceiveCellClick(const int32 _x, const int32 _y, FKey _button)
{
	if (_button.GetDisplayName().EqualTo(EKeys::LeftMouseButton.GetDisplayName()))
	{
		OpenAtDelegate.Broadcast(_x, _y);
		return;
	}
	
	if (_button.GetDisplayName().EqualTo(EKeys::RightMouseButton.GetDisplayName()))
	{
		FlagAt(_x, _y);
		return;
	}
}

void AwSocketMinesweeperPawn::FlagAt(const int32 _x, const int32 _y)
{
	m_map[_y * m_sizeX + _x]->ToggleFlag();
}

void AwSocketMinesweeperPawn::setAt(const int32 _idx, const FString& _value)
{
	m_map[_idx]->SetValue(_value);
}

void AwSocketMinesweeperPawn::SetAt(const int32 _x, const int32 _y, const FString& _value)
{
	m_map[_y * m_sizeX + _x]->SetValue(_value);
}

