#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "DialogText.generated.h"

USTRUCT()
struct FDialogTextData
{
	GENERATED_BODY()

public:
	FDialogTextData() {};

	FDialogTextData(const FString& inText, int inStart, int inEnd)
		: Text(inText), StartIndex(inStart), EndIndex(inEnd) {};

	UPROPERTY()
	FString Text;

	UPROPERTY()
	int StartIndex;

	UPROPERTY()
	int EndIndex;
};

UCLASS()
class TESTDIALOGS_API UDialogText : public URichTextBlock
{
	GENERATED_BODY()

public:
	virtual void SetText(const FText& InText) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Show Variables")
	FString HiddenTextName = "Hidden";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Show Variables")
	float TextSpeed = 0.05;

protected:

private:
	void TypeWritterEffect(FString &InString, int32 &CurrentIndex, TArray<FDialogTextData>& StyleReferences);
	FString RemoveStyleReferences(const FText InText, TArray<FDialogTextData> &outStyleReferences);
	void EndTypeWritter(FString InString);

	FTimerHandle TextTimer;
};
