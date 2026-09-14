// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "Widgets/SbBlinkResultWidget.h"

// Sb
#include "SbGameplayTags.h"

// Bomber
#include "Subsystems/GlobalMessageSubsystem.h"

// UE
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbBlinkResultWidget)

/*********************************************************************************************
 * Overrides
 ********************************************************************************************* */

// Called after the underlying slate widget is constructed
void USbBlinkResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

	// Bind all possible blink outcome events 
    UGlobalMessageSubsystem::CallOrStartListeningForGlobalMessage(SbGameplayTags::Event::BlinkFailed_InvalidCell, this, &ThisClass::OnBlinkResult);
    UGlobalMessageSubsystem::CallOrStartListeningForGlobalMessage(SbGameplayTags::Event::BlinkFailed_Occupied, this, &ThisClass::OnBlinkResult);
    UGlobalMessageSubsystem::CallOrStartListeningForGlobalMessage(SbGameplayTags::Event::BlinkSucceeded, this, &ThisClass::OnBlinkResult);

    if (BlinkResultText)
    {
        BlinkResultText->SetVisibility(ESlateVisibility::Hidden);
    }
}

// Called when the widget is removed from the viewport
void USbBlinkResultWidget::NativeDestruct()
{
	// Unbind from both events
    UGlobalMessageSubsystem::StopListeningForAllGlobalMessages(this);

    Super::NativeDestruct();
}

/*********************************************************************************************
 * Events
 ********************************************************************************************* */

// Called when the blink ability result is broadcast
// If blink failed, updates the text block with the failure reason depending on what the ability broadcasts, otherwise hides the text block
void USbBlinkResultWidget::OnBlinkResult_Implementation(const FGameplayEventData& Payload)
{
	if (Payload.EventTag == SbGameplayTags::Event::BlinkSucceeded)
	{
		BlinkResultText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	FText FailureReason;
	if (Payload.EventTag == SbGameplayTags::Event::BlinkFailed_InvalidCell)
	{
		FailureReason = NSLOCTEXT("SkillBlink", "InvalidCell", "Cannot blink: target cell is not valid");
	}
	else if (Payload.EventTag == SbGameplayTags::Event::BlinkFailed_Occupied)
	{
		FailureReason = NSLOCTEXT("SkillBlink", "Occupied", "Cannot blink: cell is occupied");
	}

	BlinkResultText->SetText(FailureReason);
	BlinkResultText->SetVisibility(ESlateVisibility::Visible);
}
