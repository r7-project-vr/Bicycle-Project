#pragma once

#include "CoreMinimal.h"
#include "WuBranch/Enum/TutorialType.h"
#include "TutorialData.generated.h"

/**
 * チュートリアルのデータ
 */
USTRUCT(BlueprintType)
struct BICYCLEPROJECT_API FTutorialData : public FTableRowBase
{
	GENERATED_BODY()

	/// <summary>
	/// チュートリアルのタイプ
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ETutorialType Type;

	/// <summary>
	/// ボタンに表示する文字
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ButtonText;

	/// <summary>
	/// プレイヤーとの距離
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Distance;
};