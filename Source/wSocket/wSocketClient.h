// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h"       // Socket definition
#include "wSocketClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoParaDelegateSign);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneParaDelegateSign, const FString&, _msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FThreeParaDelegateSign, int32, StatusCode, const FString&, Reason, bool, bWasClean);

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom))
class WSOCKET_API AwSocketClient : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable, Category = "WebSocket Events")
	FNoParaDelegateSign OnConnectedDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "WebSocket Events")
	FOneParaDelegateSign OnConnectionErrorDelegate;

	UPROPERTY(BlueprintAssignable, Category = "WebSocket Events")
	FThreeParaDelegateSign OnClosedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "WebSocket Events")
	FOneParaDelegateSign OnMessageDelegate;

	UPROPERTY(BlueprintAssignable, Category = "WebSocket Events")
	FOneParaDelegateSign OnRawMessageDelegate;

	UPROPERTY(BlueprintAssignable, Category = "WebSocket Events")
	FOneParaDelegateSign OnMessageSentDelegate;

private:
	//Web Socket module pointer
	TSharedPtr<IWebSocket> Socket = nullptr;
	FString ServerURL;
	FString ServerProtocol;

	//Handles for storing and unbinding event delegates
	FDelegateHandle DH_ONCON;
	FDelegateHandle DH_ONCER;
	FDelegateHandle DH_ONCLS;
	FDelegateHandle DH_ONMSG;
	FDelegateHandle DH_ONRMS;
	FDelegateHandle DH_ONMSS;

public:
	void BeginPlay();
	void Tick(float deltaTime);
	
	/**
	 * A public Blueprint-friendly interface for creating a web socket module.
	 * Assigns url and protocol and forwards to CreateSocket()
	 * @param _url - Server URL
	 * @param _protocol - Protocol to use
	 * @return returns true on success, false on failure
	 */
	UFUNCTION(BlueprintCallable)
	bool CreateSocket(const FString& _url, const FString& _protocol);

	/**
	 * @brief A public Blueprint-friendly interface for removing a web socket module.
	 * @brief Ends active connection and removes web socket module.
	 * @return returns true on success, false on failure
	 */
	UFUNCTION(BlueprintCallable)
	bool RemoveSocket();

	/**
	 * @brief Checks the status of current web socket.
	 * @return Only returns true if both bound and connected.
	 */
	UFUNCTION(BlueprintCallable)
	bool CheckStatus();

	/**
	 * @brief Connects currently active web socket module using stored URL and Protocol
	 */
	UFUNCTION(BlueprintCallable)
	void Connect();

	/**
	 * @brief This just forwards a call to CloseConnection().
	 * @brief I guess I just couldn't get used to "closing", also saves time searching for the method
	 */
	UFUNCTION(BlueprintCallable)
	void Disconnect();

	/**
	 * @brief Disconnects currently active web socket module.
	 * @brief Ends active connection but does not remove web socket module.
	 */
	UFUNCTION(BlueprintCallable)
	void CloseConnection();

	/**
	 * @brief Sends data to server. Can convert to raw version if requested.
	 * @param _data - data to send, in FString format
	 * @param _raw - data type option
	 */
	UFUNCTION(BlueprintCallable)
	void SendData(const FString& _data, const bool _raw);
	
private:
	/**
	 * @brief Creates an instance of IWebSocket and assigns it to this actor.
	 * @brief This uses ServerURL and ServerProtocol class variables
	 */
	bool CreateSocket();

	/**
	 * @brief Binds all relevant server events and stores handles for future unbinding
	 */
	void BindEvents();

	/**
	 * @brief Unbinds all relevant server events stored in handles
	 */
	void UnBindEvents();

	/**
	 * @brief This is the original Raw Message Event delegate, converts binary server input into FString
	 * @param Data - raw data array pointer, void*, but presumably char*
	 * @param Size - size of current array chunk
	 * @param BytesRemaining - leftover array chunk, sent in next message (probably, I'm not netwoks guy)
	 */
	void OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining);

	/**
	 * @brief Connection success event delegate, forwards event further
	 */
	void OnConnected();

	/**
	 * @brief Connection failure event delegate, forwards event further
	 * @param Error - error message response from server
	 */
	void OnConnectionError(const FString& Error);

	/**
	 * @brief Connection closure event delegate, forwards event further
	 * @param StatusCode - internal description
	 * @param Reason - human-readable description
	 * @param bWasClean - was it a clean disconnect
	 */
	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);

	/**
	 * @brief Server response event delegate, forwards event further
	 * @param Message - readable message
	 */
	void OnMessage(const FString& Message);

	/**
	 * @brief Server response event delegate, fires when server receives our message,
	 * @brief forwards event further
	 * @param MessageString - readable message
	 */
	void OnMessageSent(const FString& MessageString);
};
