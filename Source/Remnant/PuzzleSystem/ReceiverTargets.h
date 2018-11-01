// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReceiverTargets.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANT_API UReceiverTargets : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Receiver")
	TArray<AActor*> receiver_targets_;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Receiver")
	bool IncrementID();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Receiver")
	bool DecrementID();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Receiver")
	AActor* GetCurrentNode();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Receiver")
	int32 GetCurrentID() const { return current_id_; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Receiver")
	int32 GetNumNodes() const { return num_nodes_; }

protected:
	void BeginPlay() override;

private:
	int32 current_id_;
	int32 num_nodes_;
};
