// Copyright VELOCITY X Racing. All Rights Reserved.

#include "AVelocityXChaosVehicle.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

AVelocityXChaosVehicle::AVelocityXChaosVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default 4-Wheel Configuration (0: FL, 1: FR, 2: RL, 3: RR)
	// Front Wheels: Steerable
	WheelsConfig[0].WheelRadius = 34.5f;
	WheelsConfig[0].TireWidth = 275.0f;
	WheelsConfig[0].FrictionMultiplier = 1.75f;
	WheelsConfig[0].CorneringStiffness = 20.0f;
	WheelsConfig[0].LongitudinalStiffness = 24.0f;
	WheelsConfig[0].SuspensionStiffness = 88000.0f;
	WheelsConfig[0].DampingCompression = 4800.0f;
	WheelsConfig[0].DampingRebound = 7200.0f;
	WheelsConfig[0].SuspensionMaxRaise = 8.0f;
	WheelsConfig[0].SuspensionMaxDrop = 8.0f;
	WheelsConfig[0].bSteerable = true;
	WheelsConfig[0].bDriven = false;

	WheelsConfig[1] = WheelsConfig[0];

	// Rear Wheels: Driven
	WheelsConfig[2].WheelRadius = 36.0f;
	WheelsConfig[2].TireWidth = 335.0f;
	WheelsConfig[2].FrictionMultiplier = 1.80f;
	WheelsConfig[2].CorneringStiffness = 22.0f;
	WheelsConfig[2].LongitudinalStiffness = 26.0f;
	WheelsConfig[2].SuspensionStiffness = 115000.0f;
	WheelsConfig[2].DampingCompression = 5800.0f;
	WheelsConfig[2].DampingRebound = 8800.0f;
	WheelsConfig[2].SuspensionMaxRaise = 7.5f;
	WheelsConfig[2].SuspensionMaxDrop = 7.5f;
	WheelsConfig[2].bSteerable = false;
	WheelsConfig[2].bDriven = true;

	WheelsConfig[3] = WheelsConfig[2];

	for (int32 i = 0; i < 4; ++i)
	{
		WheelsState[i].WheelIndex = i;
		WheelsState[i].AngularVelocity = 0.0f;
		WheelsState[i].RotationDegrees = 0.0f;
		WheelsState[i].SteerAngleDegrees = 0.0f;
		WheelsState[i].SuspensionCompressionCm = 0.0f;
		WheelsState[i].VerticalLoadN = (VehicleMassKg * 9.81f) * 0.25f;
		WheelsState[i].SlipRatio = 0.0f;
		WheelsState[i].SlipAngleDegrees = 0.0f;
		WheelsState[i].TractionForceX = 0.0f;
		WheelsState[i].CorneringForceY = 0.0f;
		WheelsState[i].bIsSkidding = false;
	}

	CurrentRPM = IdleRPM;
	CurrentGear = 1;
	CurrentSpeedKph = 0.0f;
	bIsShifting = false;
	ShiftCooldownTimer = 0.0f;
	PreviousFrameVelocity = FVector::ZeroVector;
}

void AVelocityXChaosVehicle::BeginPlay()
{
	Super::BeginPlay();

	CurrentRPM = IdleRPM;
	CurrentGear = 1;
	PreviousFrameVelocity = GetVelocity();

	UE_LOG(LogTemp, Display, TEXT("[VelocityXChaosVehicle] Initialized with %d HP, %d Nm Peak Torque"),
		FMath::RoundToInt(Horsepower), FMath::RoundToInt(PeakTorqueNm));
}

void AVelocityXChaosVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DeltaTime <= 0.0f)
	{
		return;
	}

	// 1. Vehicle velocity and G-Forces
	CalculateVehicleDynamics(DeltaTime);

	// 2. Engine RPM and Throttle response
	SimulateEngine(DeltaTime);

	// 3. Transmission and gear shifting
	SimulateTransmission(DeltaTime);

	// 4. Wheels, Suspension and Pacejka Friction
	SimulateWheelsAndSuspension(DeltaTime);
}

void AVelocityXChaosVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AVelocityXChaosVehicle::SetThrottle(float Value)
{
	RawThrottle = FMath::Clamp(Value, 0.0f, 1.0f);
}

void AVelocityXChaosVehicle::SetBrake(float Value)
{
	RawBrake = FMath::Clamp(Value, 0.0f, 1.0f);
}

void AVelocityXChaosVehicle::SetSteering(float Value)
{
	RawSteering = FMath::Clamp(Value, -1.0f, 1.0f);
}

void AVelocityXChaosVehicle::SetHandbrake(bool bActive)
{
	bHandbrakeActive = bActive;
}

void AVelocityXChaosVehicle::ShiftUp()
{
	if (bIsShifting)
	{
		return;
	}

	if (CurrentGear < ForwardGearRatios.Num())
	{
		CurrentGear++;
		bIsShifting = true;
		ShiftCooldownTimer = GearShiftTime;
		CurrentRPM = FMath::Max(IdleRPM, CurrentRPM * 0.74f);
		UE_LOG(LogTemp, Log, TEXT("[VelocityXChaosVehicle] Upshift to gear %d"), CurrentGear);
	}
}

void AVelocityXChaosVehicle::ShiftDown()
{
	if (bIsShifting)
	{
		return;
	}

	if (CurrentGear > -1)
	{
		CurrentGear--;
		bIsShifting = true;
		ShiftCooldownTimer = GearShiftTime;
		CurrentRPM = FMath::Min(RedlineRPM, CurrentRPM * 1.32f);
		UE_LOG(LogTemp, Log, TEXT("[VelocityXChaosVehicle] Downshift to gear %d"), CurrentGear);
	}
}

FVelocityXWheelRuntimeState AVelocityXChaosVehicle::GetWheelRuntimeState(int32 WheelIndex) const
{
	if (WheelIndex >= 0 && WheelIndex < 4)
	{
		return WheelsState[WheelIndex];
	}
	return FVelocityXWheelRuntimeState();
}

void AVelocityXChaosVehicle::CalculateVehicleDynamics(float DeltaTime)
{
	FVector CurrentVel = GetVelocity();
	FVector ForwardVec = GetActorForwardVector();
	FVector RightVec = GetActorRightVector();

	float SpeedCmS = FVector::DotProduct(CurrentVel, ForwardVec);
	CurrentSpeedKph = FMath::Abs((SpeedCmS * 0.01f) * 3.6f);

	FVector Acceleration = (CurrentVel - PreviousFrameVelocity) / DeltaTime;
	PreviousFrameVelocity = CurrentVel;

	float LongAccel = FVector::DotProduct(Acceleration, ForwardVec) * 0.01f;
	float LatAccel = FVector::DotProduct(Acceleration, RightVec) * 0.01f;

	LongitudinalG = LongAccel / 9.81f;
	LateralG = LatAccel / 9.81f;
}

float AVelocityXChaosVehicle::GetTorqueAtRPM(float RPM) const
{
	if (RPM < IdleRPM)
	{
		return PeakTorqueNm * 0.45f;
	}

	float NormalizedRPM = (RPM - IdleRPM) / (RedlineRPM - IdleRPM);
	NormalizedRPM = FMath::Clamp(NormalizedRPM, 0.0f, 1.0f);

	// Realistic NFS-style torque curve: rapid rise, flat plateau, top end taper
	float TorqueScale;
	if (NormalizedRPM < 0.20f)
	{
		TorqueScale = FMath::Lerp(0.60f, 0.92f, NormalizedRPM / 0.20f);
	}
	else if (NormalizedRPM < 0.75f)
	{
		TorqueScale = FMath::Lerp(0.92f, 1.0f, (NormalizedRPM - 0.20f) / 0.55f);
	}
	else
	{
		TorqueScale = FMath::Lerp(1.0f, 0.86f, (NormalizedRPM - 0.75f) / 0.25f);
	}

	return PeakTorqueNm * TorqueScale;
}

void AVelocityXChaosVehicle::SimulateEngine(float DeltaTime)
{
	if (bIsShifting)
	{
		ShiftCooldownTimer -= DeltaTime;
		if (ShiftCooldownTimer <= 0.0f)
		{
			bIsShifting = false;
		}
	}

	float SpeedMps = CurrentSpeedKph / 3.6f;
	float GearRatio = 1.0f;

	if (CurrentGear > 0 && CurrentGear <= ForwardGearRatios.Num())
	{
		GearRatio = ForwardGearRatios[CurrentGear - 1];
	}
	else if (CurrentGear == -1)
	{
		GearRatio = ReverseGearRatio;
	}

	float TotalRatio = GearRatio * FinalDriveRatio;
	float RearWheelRadiusM = WheelsConfig[2].WheelRadius * 0.01f;
	float WheelRPM = (SpeedMps / (2.0f * PI * RearWheelRadiusM)) * 60.0f;
	float MechanicalRPM = WheelRPM * TotalRatio;

	if (bIsShifting || CurrentGear == 0)
	{
		float TargetRev = (RawThrottle > 0.05f) ? (RedlineRPM * RawThrottle) : IdleRPM;
		CurrentRPM = FMath::FInterpTo(CurrentRPM, TargetRev, DeltaTime, ThrottleResponseRate);
	}
	else
	{
		float TargetRPM = FMath::Max(MechanicalRPM, IdleRPM);
		if (RawThrottle > 0.05f && MechanicalRPM < IdleRPM * 1.4f)
		{
			// Low speed clutch slip response
			TargetRPM = FMath::Max(TargetRPM, IdleRPM + (RawThrottle * 2800.0f));
		}

		CurrentRPM = FMath::FInterpTo(CurrentRPM, TargetRPM, DeltaTime, ThrottleResponseRate);
	}

	CurrentRPM = FMath::Clamp(CurrentRPM, IdleRPM, RedlineRPM);
}

void AVelocityXChaosVehicle::SimulateTransmission(float DeltaTime)
{
	if (!bAutomaticTransmission || bIsShifting)
	{
		return;
	}

	int32 MaxGears = ForwardGearRatios.Num();
	float UpshiftThreshold = RedlineRPM * 0.94f;
	float DownshiftThreshold = RedlineRPM * 0.36f;

	if (CurrentGear > 0 && CurrentGear < MaxGears && CurrentRPM >= UpshiftThreshold && RawThrottle > 0.35f)
	{
		ShiftUp();
	}
	else if (CurrentGear > 1 && CurrentRPM <= DownshiftThreshold && RawBrake < 0.15f)
	{
		ShiftDown();
	}
	else if (CurrentGear == 1 && RawBrake > 0.5f && CurrentSpeedKph < 2.0f)
	{
		CurrentGear = -1; // Reverse
	}
	else if (CurrentGear == -1 && RawThrottle > 0.5f && CurrentSpeedKph < 2.0f)
	{
		CurrentGear = 1;
	}
}

void AVelocityXChaosVehicle::SimulateWheelsAndSuspension(float DeltaTime)
{
	float SpeedMps = CurrentSpeedKph / 3.6f;
	float QuarterMassWeight = (VehicleMassKg * 9.81f) * 0.25f;

	// Dynamic weight transfer
	float WheelbaseM = WheelbaseCm * 0.01f;
	float PitchLoadDelta = (VehicleMassKg * (LongitudinalG * 9.81f) * 0.36f) / WheelbaseM;

	for (int32 i = 0; i < 4; ++i)
	{
		const FVelocityXWheelConfig& Cfg = WheelsConfig[i];
		FVelocityXWheelRuntimeState& State = WheelsState[i];

		// Steer angle for front wheels with high-speed sensitivity scaling
		if (Cfg.bSteerable)
		{
			float SteerSpeedScale = FMath::GetMappedRangeValueClamped(FVector2D(50.0f, 300.0f), FVector2D(1.0f, 0.32f), CurrentSpeedKph);
			State.SteerAngleDegrees = RawSteering * MaxSteerAngle * SteerSpeedScale;
		}
		else
		{
			State.SteerAngleDegrees = 0.0f;
		}

		// Vertical normal load with pitch transfer
		float AxleLoadShift = (i < 2) ? -PitchLoadDelta * 0.5f : PitchLoadDelta * 0.5f;
		State.VerticalLoadN = FMath::Max(100.0f, QuarterMassWeight + AxleLoadShift);

		// Suspension displacement x = F / k in cm
		float DeltaForce = State.VerticalLoadN - QuarterMassWeight;
		float TargetDispCm = -(DeltaForce / Cfg.SuspensionStiffness) * 100.0f;
		TargetDispCm = FMath::Clamp(TargetDispCm, -Cfg.SuspensionMaxDrop, Cfg.SuspensionMaxRaise);
		State.SuspensionCompressionCm = FMath::FInterpTo(State.SuspensionCompressionCm, TargetDispCm, DeltaTime, 14.0f);

		// Wheel angular speed and rotation
		float RadiusM = Cfg.WheelRadius * 0.01f;
		float WheelLinearSpeed = SpeedMps;

		if (Cfg.bDriven && RawThrottle > 0.1f)
		{
			// Extra spin under heavy throttle
			WheelLinearSpeed += (RawThrottle * 3.5f);
		}
		if (bHandbrakeActive && !Cfg.bSteerable)
		{
			// Handbrake locks rear wheels
			WheelLinearSpeed = 0.0f;
		}

		State.AngularVelocity = WheelLinearSpeed / RadiusM;
		float RotDegreesDelta = FMath::RadiansToDegrees(State.AngularVelocity * DeltaTime);
		State.RotationDegrees = FMath::Fmod(State.RotationDegrees + RotDegreesDelta, 360.0f);

		// Slip calculation (longitudinal & lateral)
		float MaxV = FMath::Max(FMath::Abs(SpeedMps), 0.1f);
		State.SlipRatio = FMath::Clamp((WheelLinearSpeed - SpeedMps) / MaxV, -1.0f, 1.0f);
		State.SlipAngleDegrees = (i < 2) ? (State.SteerAngleDegrees - (LateralG * 5.0f)) : -(LateralG * 4.0f);

		// Pacejka Magic Formula Traction
		float Mu = Cfg.FrictionMultiplier;
		float MaxForce = Mu * State.VerticalLoadN;

		float LongForce = MaxForce * FMath::Sin(1.65f * FMath::Atan(Cfg.LongitudinalStiffness * State.SlipRatio));
		float LatForce = MaxForce * FMath::Sin(1.35f * FMath::Atan(Cfg.CorneringStiffness * FMath::DegreesToRadians(State.SlipAngleDegrees)));

		State.TractionForceX = LongForce;
		State.CorneringForceY = LatForce;

		State.bIsSkidding = (FMath::Abs(State.SlipRatio) > 0.22f || FMath::Abs(State.SlipAngleDegrees) > 12.0f);
	}
}
