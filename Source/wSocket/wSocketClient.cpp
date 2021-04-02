// Fill out your copyright notice in the Description page of Project Settings.

//Note: You can disable UE_LOG messages by commenting out Definitions.Add("UE_LOG_ENABLED"); in wSocket.Build.cs

#include "wSocketClient.h"

void AwSocketClient::BeginPlay()
{
	Super::BeginPlay();
}

void AwSocketClient::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

bool AwSocketClient::CheckStatus()
{
	if (Socket != nullptr)
	{
		if (!Socket->IsConnected())
		{
			return false;
		}
		//true only if socket is valid and connected
		return true;
	}
	return false;
}

bool AwSocketClient::CreateSocket()
{
	if (Socket != nullptr)
	{
		if (Socket->IsConnected())
		{
			CloseConnection();
		}
		Socket.Reset();
	}
	if (ServerURL.IsEmpty() || ServerProtocol.IsEmpty())
	{
		return false;
	}
	Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
	if (Socket != nullptr)
	{
		BindEvents();
		return true;
	}
	return false;
}

bool AwSocketClient::RemoveSocket()
{
	if (Socket != nullptr)
	{
		if (Socket->IsConnected())
		{
			CloseConnection();
		}
		UnBindEvents();
		Socket.Reset();
		if (Socket != nullptr)
		{
			return false;
		}
		return true;
	}
	return true;
}

bool AwSocketClient::CreateSocket(const FString& _url, const FString& _protocol)
{
	ServerURL = _url;
	ServerProtocol = _protocol;
	return CreateSocket();
}

void AwSocketClient::BindEvents()
{
	// We bind all available events and save handles to unbind when we're done
	DH_ONCON = Socket->OnConnected().AddUObject(this, &AwSocketClient::OnConnected);
	DH_ONCER = Socket->OnConnectionError().AddUObject(this, &AwSocketClient::OnConnectionError);
	DH_ONCLS = Socket->OnClosed().AddUObject(this, &AwSocketClient::OnClosed);
	DH_ONMSG = Socket->OnMessage().AddUObject(this, &AwSocketClient::OnMessage);
	DH_ONRMS = Socket->OnRawMessage().AddUObject(this, &AwSocketClient::OnRawMessage);
	DH_ONMSS = Socket->OnMessageSent().AddUObject(this, &AwSocketClient::OnMessageSent);
}

void AwSocketClient::UnBindEvents()
{
	// Unbind all bound events using save handles
	Socket->OnConnected().Remove(DH_ONCON);
	Socket->OnConnectionError().Remove(DH_ONCER);
	Socket->OnClosed().Remove(DH_ONCLS);
	Socket->OnMessage().Remove(DH_ONMSG);
	Socket->OnRawMessage().Remove(DH_ONRMS);
	Socket->OnMessageSent().Remove(DH_ONMSS);
}

void AwSocketClient::OnConnected()
{
	//This code will run once connected.
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("Successfully connected to host."));
#endif
	OnConnectedDelegate.Broadcast();
}

void AwSocketClient::OnConnectionError(const FString& Error)
{
	// This code will run if the connection failed. Check Error to see what happened.
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("Connection error: %s"), *Error);
#endif
	OnConnectionErrorDelegate.Broadcast(Error);
}

void AwSocketClient::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	// This code will run when the connection to the server has been terminated.
	// Because of an error or a call to Socket->Close().
#ifdef UE_LOG_ENABLED
	if (bWasClean)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection closed with code: %d, Reason: %s, Clean."), StatusCode, *Reason);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection closed with code: %d, Reason: %s, NOT Clean."), StatusCode, *Reason);
	}
#endif
	OnClosedDelegate.Broadcast(StatusCode, Reason, bWasClean);
}

void AwSocketClient::OnMessage(const FString& Message)
{
	// This code will run when we receive a string message from the server.
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("Message from host: %s"), *Message);
#endif
	OnMessageDelegate.Broadcast(Message);
}

void AwSocketClient::OnMessageSent(const FString& MessageString)
{
	// This code is called after we sent a message to the server.
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("Message from host: %s"), *MessageString);
#endif
	OnMessageSentDelegate.Broadcast(MessageString);
}

void AwSocketClient::OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
	// This code will run when we receive a raw (binary) message from the server.
	// For ease of use I'm going to convert it straight away
	FString formatted;
	char* _c = static_cast<char*>(const_cast<void*>(Data));
	for (int32 i = 0; i < Size; i++)
	{
		formatted.AppendChar(_c[i]);
	}
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("Message from host (RAW): %s"), *formatted);
#endif
	//free(_c);
	OnRawMessageDelegate.Broadcast(formatted);
}

void AwSocketClient::Connect()
{
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("Establishing connection..."));
#endif
	Socket->Connect();
}
void AwSocketClient::CloseConnection()
{
#ifdef UE_LOG_ENABLED
	UE_LOG(LogTemp, Warning, TEXT("Closing connection."));
#endif
	Socket->Close();
}

void AwSocketClient::Disconnect()
{
	if (Socket != nullptr)
	{
		if (Socket->IsConnected())
		{
			CloseConnection();
		}
	}
}

void AwSocketClient::SendData(const FString& _data, const bool _raw)
{
	if (Socket == nullptr)
	{
		// Don't send if socket is empty.
		return;
	}

	if (!Socket->IsConnected())
	{
		// Don't send if we're not connected.
		return;
	}

	if (_raw)
	{
#ifdef UE_LOG_ENABLED
		UE_LOG(LogTemp, Warning, TEXT("Sending raw data to server..."));
#endif
		TArray<char> binaryMessage;
		binaryMessage.AddUninitialized(_data.Len());
		for(int32 c=0; c<_data.Len(); c++)
		{
			binaryMessage[c] = _data[c];
		}
		Socket->Send(binaryMessage.GetData(), sizeof(uint8) * binaryMessage.Num());
	}
	else
	{
#ifdef UE_LOG_ENABLED
		UE_LOG(LogTemp, Warning, TEXT("Sending data to server..."));
#endif
		Socket->Send(_data);
	}
}