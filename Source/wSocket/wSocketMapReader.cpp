// Fill out your copyright notice in the Description page of Project Settings.


#include "wSocketMapReader.h"

wSocketMapReader::wSocketMapReader()
{
}

wSocketMapReader::~wSocketMapReader()
{
}

TArray<FString> wSocketMapReader::readMap(const FString& _map, int& _mapSizeOut)
{
	TArray<FString> ret;
	ret.Reserve(_map.Len()); //we may not have the same number of elements at the end, but this will make resizing faster
	int it = 0;
	int sizeOut = 0;
	for (auto i=0; i<_map.Len(); ++i)
	{
		int current = (int)_map.Mid(i, 1)[0];
		if (current == 0 && i>0)
		{
			if (sizeOut == 0)
			{
				sizeOut = it;
			}
		}
		else
		{
			ret.Add(_map.Mid(i, 1));
			++it;
		}
	}
	return ret;
}