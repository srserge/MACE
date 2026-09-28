// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"

#include "MaceCollisionComponent.generated.h"

class USkeletalMeshComponent;

/** One target entered the swept volume. Fires once per actor per swing. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMaceCollisionHitSignature, AActor*, HitActor, const FHitResult&, Hit);

/**
 * UMaceCollisionComponent
 *
 * Detects what a moving limb or blade passes through while an attack is live.
 *
 * It sweeps spheres along the path each traced socket travelled since the last
 * frame. Sweeping the path rather than testing the current position is the
 * whole point: a fist crosses tens of centimetres between two frames, and a
 * test taken at one instant misses a target that was standing in between.
 *
 * The set of actors already hit lives here, on the attacker, because that is
 * whose swing it is. It cannot live on the notify state that opens the window:
 * notify objects are shared by everyone playing the same montage, so two
 * characters punching at once would share one another's hit list.
 *
 * WHAT THIS IS NOT FOR. This detects swept shapes, which is melee, armed or
 * unarmed. A bullet is not a swept shape: it is one instantaneous trace from a
 * muzzle, with no window, no continuity between frames and no need to remember
 * anything, so it belongs in the ranged ability instead. What the two share is
 * everything after the hit - the damage effect, the meta attribute funnel and
 * the confirmed hit event - and that is already common to both.
 *
 * The component carries no damage of its own. It reports what was touched and
 * the ability decides what that costs, which is what keeps one detector usable
 * by an attack, a shove and a trap.
 */
UCLASS(ClassGroup = (Mace), Meta = (BlueprintSpawnableComponent, DisplayName = "Mace Collision"))
class MACECORE_API UMaceCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UMaceCollisionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** The collision component on an actor, or null when it has none. */
	UFUNCTION(BlueprintPure, Category = "Mace|Collision", Meta = (DefaultToSelf = "Actor"))
	static UMaceCollisionComponent* FindCollisionComponent(const AActor* Actor);

	/**
	 * Starts detecting along these sockets until EndWindow.
	 *
	 * Mesh is which skeletal mesh the socket names belong to: the character's
	 * own for a punch, the equipped weapon's for a sword. Null means the
	 * owner's first skeletal mesh, which is the unarmed case.
	 *
	 * Sockets are read as a chain, so a blade is described by its base and its
	 * tip rather than by one point at the end of it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mace|Collision")
	void BeginWindow(USkeletalMeshComponent* Mesh, const TArray<FName>& Sockets, float Radius);

	/** Stops detecting. Leaves the hit list alone, so a second window in the
	 *  same attack cannot hit the same target twice. */
	UFUNCTION(BlueprintCallable, Category = "Mace|Collision")
	void EndWindow();

	/** Forgets who has been hit, so the next window counts as a new swing. */
	UFUNCTION(BlueprintCallable, Category = "Mace|Collision")
	void ResetHitActors();

	UFUNCTION(BlueprintPure, Category = "Mace|Collision")
	bool IsWindowOpen() const { return bWindowOpen; }

	/** True when this actor is worth reporting: not us, alive, and a combatant. */
	UFUNCTION(BlueprintPure, Category = "Mace|Collision")
	virtual bool IsValidTarget(AActor* Target) const;

	/** Every confirmed hit. Bound by whatever turns hits into consequences. */
	UPROPERTY(BlueprintAssignable, Category = "Mace|Collision")
	FMaceCollisionHitSignature OnHitDetected;

	/**
	 * Detect only where the game is authoritative.
	 *
	 * Damage is a server decision in Mace, so a client detecting hits would
	 * only be telling itself something it is not allowed to act on.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace|Collision")
	bool bAuthorityOnly = true;

	/** Whether opening a window starts a new swing. Off chains several windows
	 *  into one swing, so a two-part slash still only hits once. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace|Collision")
	bool bClearHitsOnWindowBegin = true;

	/**
	 * Extra sample points inserted between consecutive sockets.
	 *
	 * Zero suits a fist, where one socket is the whole weapon. A blade needs
	 * them: with only its base and its tip sampled, the middle of the edge
	 * passes through a target without touching it.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace|Collision", Meta = (ClampMin = "0", ClampMax = "16"))
	int32 SegmentSubdivisions = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace|Collision")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	/** Draws the swept spheres. Compiled out of shipping builds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace|Collision|Debug")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace|Collision|Debug", Meta = (ClampMin = "0.0"))
	float DebugDrawDuration = 2.0f;

protected:

	//~ Begin UActorComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent

	/** Where every sample point is right now: the sockets and what fills the
	 *  gaps between them. Order is stable, so a point can be compared with
	 *  itself one frame earlier. */
	void BuildSamplePoints(TArray<FVector>& OutPoints) const;

	void SweepBetweenFrames(const TArray<FVector>& CurrentPoints);

private:

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> TracedMesh;

	/** Socket names, already checked against the mesh when the window opened. */
	TArray<FName> TracedSockets;

	float TraceRadius = 0.0f;

	bool bWindowOpen = false;

	TArray<FVector> PreviousPoints;

	/** Weak, because a target can be destroyed in the middle of the swing that
	 *  is still holding a reference to it. */
	TArray<TWeakObjectPtr<AActor>> HitActors;
};
