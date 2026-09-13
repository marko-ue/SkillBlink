// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "Components/SbPlayerControllerComponent.h"

// Sb
#include "Data/SbDataAsset.h"

// Bomber
#include "Controllers/BmrPlayerController.h"
#include "DalSubsystem.h"
#include "DataAssets/BmrInputMappingContext.h"
#include "GfpmUtils.h"
#include "MyUtilsLibraries/InputUtilsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbPlayerControllerComponent)

// Sets default values for this component's properties
USbPlayerControllerComponent::USbPlayerControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Returns Player Controller of this component
ABmrPlayerController* USbPlayerControllerComponent::GetPlayerController() const
{
	return GetOwner<ABmrPlayerController>();
}

ABmrPlayerController& USbPlayerControllerComponent::GetPlayerControllerChecked() const
{
	ABmrPlayerController* MyPlayerController = GetPlayerController();
	checkf(MyPlayerController, TEXT("%s: 'MyPlayerController' is null"), *FString(__FUNCTION__));
	return *MyPlayerController;
}

/*********************************************************************************************
 * Main methods
 ********************************************************************************************* */

// Sets up the input context for SkillBlink for the player controller
void USbPlayerControllerComponent::SetupBlinkInputContext() const
{
	// Lambda that sets up input contexts when the data asset becomes valid
	UDalSubsystem::Get().ListenForDataAsset<USbDataAsset>(this, [this](const USbDataAsset& DA)
	{
		const TArray<const UBmrInputMappingContext*> Contexts = {DA.GetBlinkInputContext()};
		GetPlayerControllerChecked().SetupInputContexts(Contexts);
	});
}

// Removes the input context for SkillBlink from the player controller
void USbPlayerControllerComponent::RemoveBlinkInputContextAndActions() const
{
	ABmrPlayerController* MyPC = GetPlayerController();

	const USbDataAsset* DataAsset = UDalSubsystem::GetDataAsset<USbDataAsset>();
	UBmrInputMappingContext* BlinkContext = DataAsset ? DataAsset->GetBlinkInputContext() : nullptr;
	if (MyPC && BlinkContext)
	{
		TArray<UInputAction*> ContextInputActions;
		UInputUtilsLibrary::GetAllActionsInContext(MyPC, BlinkContext, EInputActionInContextState::Any, /*out*/ ContextInputActions);
		UInputUtilsLibrary::UnbindInputActionsInContext(MyPC, BlinkContext);
		UGfpmUtils::UnloadAssets(ContextInputActions);
		MyPC->RemoveInputContexts({BlinkContext});
	}
}

/*********************************************************************************************
 * Overrides
 ********************************************************************************************* */

// Called when the owning Actor begins play or when the component is created if the Actor has already begun play
void USbPlayerControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetPlayerControllerChecked().IsLocalController())
	{
		SetupBlinkInputContext();
	}
}

// Clears all transient data created by this component
void USbPlayerControllerComponent::OnUnregister()
{
	if (GetPlayerControllerChecked().IsLocalController())
	{
		RemoveBlinkInputContextAndActions();
	}
	
	Super::OnUnregister();
}
