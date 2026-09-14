// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "Blueprint/UserWidget.h"

#include "SbBlinkResultWidget.generated.h"

class UTextBlock;

/**
 * Displays the reason for a failed blink attempt.
 */
UCLASS()
class SKILLBLINKRUNTIME_API USbBlinkResultWidget : public UUserWidget
{
	GENERATED_BODY()

	/*********************************************************************************************
	 * Protected properties
	 ********************************************************************************************* */
protected:
	/** Text block for displaying the Blink result. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = "[SkillBlink]", meta = (BlueprintProtected, BindWidget))
	TObjectPtr<UTextBlock> BlinkResultText = nullptr;
	
	/** Animation for fading out the blink result text. */
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> BlinkResultFadeOut = nullptr;

	/*********************************************************************************************
	 * Overrides
	 ********************************************************************************************* */
protected:
	/** Called after the underlying slate widget is constructed. */
	virtual void NativeConstruct() override;

	/** Called when the widget is removed from the viewport. */
	virtual void NativeDestruct() override;

	/*********************************************************************************************
	 * Events
	 ********************************************************************************************* */
protected:
	/** Called when the blink ability result is broadcast. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "[SkillBlink]", meta = (BlueprintProtected))
	void OnBlinkResult(const struct FGameplayEventData& Payload);
};
