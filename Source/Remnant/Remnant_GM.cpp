// Belongs to Anton Edvardsson, Martin Larsson and Katrine Olsen

#include "Remnant_GM.h"
#include "FP_Character.h"
#include "UObject/ConstructorHelpers.h"

ARemnant_GM::ARemnant_GM()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> player(TEXT("/Game/General_BPs/CH_Player"));
	DefaultPawnClass = player.Class;
}



