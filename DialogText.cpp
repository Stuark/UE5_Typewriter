// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogText.h"
#include "Engine/World.h"

namespace UDialogText_Text
{
	const FString RichTextBlockStyle = "</>";
	const TCHAR LessThanCharacter = '<';
	const TCHAR MoreThanCharacter = '>';
	constexpr int StyleInstigatorsSize = 2;
}

void UDialogText::SetText(const FText& InText)
{
	if (InText.IsEmpty())
	{
		URichTextBlock::SetText(InText);
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TextTimer);

	const int SizeText = InText.ToString().Len();
	TArray<FDialogTextData> StyleReferences;
	FString InString = RemoveStyleReferences(InText, StyleReferences);
	InString = "<" + HiddenTextName + ">" + InString + "</>";
	int32 IndexLoop = 0;

	GetWorld()->GetTimerManager().SetTimer(TextTimer, [this, IndexLoop, InString, StyleReferences, SizeText]() mutable {
		this->TypeWritterEffect(InString, IndexLoop, StyleReferences);
		if (IndexLoop >= SizeText)
		{
			EndTypeWritter(InString);
		}
	}, TextSpeed, true);
}

void UDialogText::TypeWritterEffect(FString &InString, int32 &CurrentIndex, TArray<FDialogTextData>& StyleReferences)
{
	const int HiddenLenght = HiddenTextName.Len() + UDialogText_Text::StyleInstigatorsSize;
	const FString letter = InString.Mid(CurrentIndex + HiddenLenght, 1);

	InString.RemoveAt(CurrentIndex + HiddenLenght);
	const bool MidStyleText = (StyleReferences.Num() > 0 && CurrentIndex > StyleReferences[0].StartIndex && CurrentIndex <= StyleReferences[0].EndIndex);
	InString.InsertAt((MidStyleText ? CurrentIndex - UDialogText_Text::RichTextBlockStyle.Len() : CurrentIndex), letter);
	Super::SetText(FText::FromString(InString));
	CurrentIndex++;

	if (StyleReferences.Num() > 0)
	{
		if (StyleReferences[0].StartIndex == CurrentIndex)
		{
			InString.InsertAt(CurrentIndex, StyleReferences[0].Text);
			CurrentIndex += StyleReferences[0].Text.Len();
			InString.InsertAt(CurrentIndex, UDialogText_Text::RichTextBlockStyle);
			CurrentIndex += UDialogText_Text::RichTextBlockStyle.Len();
		}
		else if (CurrentIndex > StyleReferences[0].EndIndex)
		{
			StyleReferences.RemoveAt(0);
		}
	}
}

FString UDialogText::RemoveStyleReferences(const FText InText, TArray<FDialogTextData>& outStyleReferences)
{
	FString ToParseText = InText.ToString();
	bool StartAdding = false;
	FString Word;
	int index = 0;
	for (TCHAR Character : ToParseText)
	{
		if (Character == UDialogText_Text::LessThanCharacter || StartAdding == true)
		{
			if (!StartAdding && (outStyleReferences.Num() <= 0 || outStyleReferences.Last().EndIndex != 0))
			{
				outStyleReferences.Add(FDialogTextData("", index, 0));
			}
			StartAdding = true;
			Word += FString(1, &Character);
			if (Character == UDialogText_Text::MoreThanCharacter)
			{
				StartAdding = false;
				if (outStyleReferences.Last().Text == "")
				{
					outStyleReferences.Last().Text = Word;
				}
				else
				{
					outStyleReferences.Last().EndIndex = index;
				}
				Word = "";
			}
		}
		index++;
	}

	index = 0;
	for (FDialogTextData DataRef : outStyleReferences)
	{
		ToParseText.RemoveAt(DataRef.StartIndex - index, DataRef.Text.Len());
		index += DataRef.Text.Len();
		ToParseText.RemoveAt(DataRef.EndIndex - index - (UDialogText_Text::RichTextBlockStyle.Len() - 1), UDialogText_Text::RichTextBlockStyle.Len());
		index += UDialogText_Text::RichTextBlockStyle.Len();
	}
	return ToParseText;
}

void UDialogText::EndTypeWritter(FString InString)
{
	GetWorld()->GetTimerManager().ClearTimer(TextTimer);
	InString.RemoveFromEnd("</>");
	InString.RemoveFromEnd("<" + HiddenTextName + ">");
	Super::SetText(FText::FromString(InString));
}