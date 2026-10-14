// Copyright VELOCITY X Racing. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#if __has_include("ChaosVehicle.h")
#include "ChaosVehicle.h"
#endif
#if __has_include("WheeledVehiclePawn.h")
#include "WheeledVehiclePawn.h"
#ifndef AWheeledVehicle
#define AWheeledVehicle AWheeledVehiclePawn
#endif
#elif __has_include("WheeledVehicle.h")
#include "WheeledVehicle.h"
#elif __has_include("ChaosVehicle.h")
#ifndef AWheeledVehicle
#define AWheeledVehicle AChaosVehicle
#endif
#else
#include "GameFramework/Pawn.h"
#ifndef AWheeledVehicle
#define AWheeledVehicle APawn
#endif
#endif

#include "Curves/CurveFloat.h"
#include "BaseCar.generated.h"

// Delegates for Audio, VFX, and UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTireSlipEvent, int32, WheelIndex, float, SlipAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSuspensionBottomOutEvent, int32, WheelIndex, float, CompressionNormalized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGearChangeEvent, int32, NewGear);

class UAudioComponent;
class USpotLightComponent;
class UPointLightComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;

// ============================================================================
// ENUMS
// ============================================================================

/** Track surface types influencing tire friction and grip */
UENUM(BlueprintType)
enum class ESurfaceType : uint8
{
	DryAsphalt UMETA(DisplayName = "Dry Asphalt"),
	WetAsphalt UMETA(DisplayName = "Wet Asphalt"),
	TrackRubber UMETA(DisplayName = "Track Rubber"),
	Dirt UMETA(DisplayName = "Dirt / Gravel"),
	Grass UMETA(DisplayName = "Grass"),
	Ice UMETA(DisplayName = "Ice / Snow"),
	Curb UMETA(DisplayName = "Kerb / Rumble Strip")
};

/** Differential mechanical configuration */
UENUM(BlueprintType)
enum class EDifferentialType : uint8
{
	Open UMETA(DisplayName = "Open Differential"),
	LimitedSlip UMETA(DisplayName = "Limited Slip Differential (LSD)"),
	Locked UMETA(DisplayName = "Fully Locked Spool"),
	ActiveTorqueVectoring UMETA(DisplayName = "Active Torque Vectoring")
};

/** Drivetrain drive layout */
UENUM(BlueprintType)
enum class EDriveType : uint8
{
	RWD UMETA(DisplayName = "Rear-Wheel Drive (RWD)"),
	AWD UMETA(DisplayName = "All-Wheel Drive (AWD)"),
	FWD UMETA(DisplayName = "Front-Wheel Drive (FWD)")
};

/** Transmission shifting mode */
UENUM(BlueprintType)
enum class ETransmissionMode : uint8
{
	Automatic UMETA(DisplayName = "Automatic Transmission"),
	Manual UMETA(DisplayName = "Manual Transmission")
};

// ============================================================================
// STRUCTS
// ============================================================================

/** Engine parameters including torque curve, power-to-weight, and redline */
USTRUCT(BlueprintType)
struct FEnginePhysicsConfig
{
	GENERATED_BODY()

	/** Maximum peak engine horsepower (HP) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float MaxHorsepower = 500.0f;

	/** Maximum engine torque in Newton-meters (N*m) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float MaxTorque = 470.0f;

	/** Idle RPM of the engine (e.g. 800 - 1000 RPM) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float IdleRPM = 900.0f;

	/** Maximum redline RPM before rev limiter cuts fuel */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float RedlineRPM = 9000.0f;

	/** Rotational inertia of the flywheel and rotating engine assembly (kg*m^2) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float EngineMomentOfInertia = 0.28f;

	/** Rate at which engine absorbs rotational energy on throttle lift (engine braking factor) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float EngineBrakingRate = 0.22f;

	/** Fuel cut duration in seconds when bouncing on redline rev limiter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float RevLimiterCutDuration = 0.08f;

	/** Calculated power-to-weight ratio in Horsepower per Metric Tonne (HP/tonne) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float PowerToWeightRatio = 333.3f;
};

/** Transmission and drivetrain gear ratios */
USTRUCT(BlueprintType)
struct FTransmissionPhysicsConfig
{
	GENERATED_BODY()

	/** Operating mode: Automatic or Manual */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	ETransmissionMode TransmissionMode = ETransmissionMode::Automatic;

	/** Drive layout: RWD, AWD, or FWD */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	EDriveType DriveType = EDriveType::RWD;

	/** Forward gear ratios: 1st, 2nd, 3rd, 4th, 5th, 6th, 7th... */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	TArray<float> ForwardGearRatios = { 3.75f, 2.38f, 1.72f, 1.34f, 1.11f, 0.96f, 0.84f };

	/** Reverse gear ratio */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float ReverseGearRatio = 3.42f;

	/** Final drive differential reduction ratio */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float FinalDriveRatio = 3.97f;

	/** Time in seconds taken to execute a gear shift (e.g. 0.06s for dual-clutch DCT) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float GearChangeTime = 0.08f;

	/** RPM percentage threshold relative to redline to trigger automatic upshift */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float AutoUpshiftRPMPercent = 0.94f;

	/** RPM percentage threshold to trigger automatic downshift */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float AutoDownshiftRPMPercent = 0.35f;

	/** Type of differential mechanism installed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	EDifferentialType DifferentialType = EDifferentialType::LimitedSlip;

	/** Locking ratio of the differential (0.0 = completely open, 1.0 = fully locked spool) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float DifferentialLockRatio = 0.65f;

	/** Front-to-rear torque distribution split for AWD (0.0 = 100% rear, 1.0 = 100% front) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float FrontTorqueSplit = 0.0f;
};

/** Suspension spring rates, damping, ride heights and geometry */
USTRUCT(BlueprintType)
struct FSuspensionPhysicsConfig
{
	GENERATED_BODY()

	/** Front spring rate / stiffness in Newtons per meter (N/m) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SpringRateFront = 80000.0f;

	/** Rear spring rate / stiffness in Newtons per meter (N/m) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SpringRateRear = 110000.0f;

	/** Front shock absorber bump/compression damping in N*s/m */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float DampingCompressionFront = 4500.0f;

	/** Rear shock absorber bump/compression damping in N*s/m */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float DampingCompressionRear = 5500.0f;

	/** Front shock absorber rebound damping in N*s/m */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float DampingReboundFront = 7000.0f;

	/** Rear shock absorber rebound damping in N*s/m */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float DampingReboundRear = 8500.0f;

	/** Equilibrium ride height / resting clearance from chassis to wheel axle (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float RideHeight = 11.0f;

	/** Maximum suspension bump / compression stroke travel (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SuspensionMaxRaise = 8.0f;

	/** Maximum suspension droop / rebound extension travel (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SuspensionMaxDrop = 8.0f;

	/** Front anti-roll sway bar stiffness in N/m to resist chassis body roll */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float AntiRollBarStiffnessFront = 15000.0f;

	/** Rear anti-roll sway bar stiffness in N/m */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float AntiRollBarStiffnessRear = 18000.0f;

	/** Front static negative camber in degrees (negative tilts inward for cornering grip) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float CamberFront = -2.2f;

	/** Rear static negative camber in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float CamberRear = -1.8f;
};

/** Tire parameters including friction coefficients, slip limits, and dimensions */
USTRUCT(BlueprintType)
struct FTirePhysicsConfig
{
	GENERATED_BODY()

	/** Baseline peak tire friction coefficient (mu) on dry racing asphalt */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float FrictionMultiplier = 1.65f;

	/** Cornering stiffness parameter (B*C in Pacejka lateral tire model) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float CorneringStiffness = 18.0f;

	/** Longitudinal stiffness parameter governing traction build-up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float LongitudinalStiffness = 22.0f;

	/** Coefficient of rolling resistance (C_rr) resisting rolling motion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float RollingResistance = 0.014f;

	/** Front tire section width in millimeters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float TireWidthFront = 275.0f;

	/** Rear tire section width in millimeters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float TireWidthRear = 335.0f;

	/** Front wheel outer rolling radius in centimeters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float WheelRadiusFront = 34.5f;

	/** Rear wheel outer rolling radius in centimeters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float WheelRadiusRear = 36.0f;

	/** Optimal slip angle in degrees where peak lateral cornering grip is attained */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float OptimalSlipAngle = 7.5f;

	/** Optimal longitudinal slip ratio (0.0 to 1.0) where peak acceleration/braking occurs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tires")
	float OptimalSlipRatio = 0.12f;
};

/** Aerodynamic drag, downforce, balance, and active wing aerodynamics */
USTRUCT(BlueprintType)
struct FAerodynamicPhysicsConfig
{
	GENERATED_BODY()

	/** Aerodynamic drag coefficient (Cd) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float DragCoefficient = 0.39f;

	/** Frontal cross-sectional area of the car in square meters (m^2) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float FrontalArea = 2.15f;

	/** Downforce coefficient (Cl) generating vertical negative lift */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float DownforceCoefficient = 1.85f;

	/** Percentage of aerodynamic downforce exerted on the front axle (e.g. 0.40 = 40% front) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float AeroBalanceFront = 0.40f;

	/** Air density in kg/m^3 (1.225 at sea level standard conditions) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float AirDensity = 1.225f;

	/** Whether the vehicle features active movable aerodynamic surfaces (wing/flaps/airbrake) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	bool bHasActiveAero = true;

	/** Drag multiplier applied when high-speed airbrake is fully deployed under hard braking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float AirbrakeDragMultiplier = 2.4f;

	/** Downforce multiplier applied when airbrake is deployed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	float AirbrakeDownforceMultiplier = 1.6f;
};

/** Active driver assistance systems including ABS, TCS, and brake distribution */
USTRUCT(BlueprintType)
struct FVehicleAssistanceConfig
{
	GENERATED_BODY()

	/** Whether Anti-lock Braking System (ABS) is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	bool bABSEnabled = true;

	/** Wheel slip ratio threshold triggering ABS brake pressure modulation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	float ABSSlipThreshold = 0.14f;

	/** ABS pulsing frequency in Hertz (Hz) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	float ABSPulseFrequency = 18.0f;

	/** Whether Traction Control System (TCS) is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	bool bTCSEnabled = true;

	/** Wheel slip ratio threshold triggering TCS engine torque cutting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	float TCSSlipThreshold = 0.11f;

	/** TCS intervention aggressiveness (0.0 to 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	float TCSAggressiveness = 0.75f;

	/** Whether Electronic Stability Program (ESP) yaw control is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	bool bESPEnabled = true;

	/** Maximum braking clamp force in Newtons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	float MaxBrakeForce = 18000.0f;

	/** Handbrake clamping force on rear wheels in Newtons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	float MaxHandbrakeForce = 12000.0f;

	/** Brake balance bias front (e.g. 0.62 = 62% front brake pressure, 38% rear) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assistance")
	float BrakeBiasFront = 0.62f;
};

/** Complete master physics configuration for a vehicle */
USTRUCT(BlueprintType)
struct FVehiclePhysicsConfig
{
	GENERATED_BODY()

	/** Name of this vehicle model */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FString VehicleName = TEXT("Base Car");

	/** Total curb mass in kilograms (kg) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float TotalMassKg = 1500.0f;

	/** Static front axle weight distribution fraction (e.g. 0.39 = 39% front, 61% rear) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float WeightDistributionFront = 0.40f;

	/** Wheelbase distance between front and rear axles in centimeters (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float WheelbaseCm = 245.7f;

	/** Track width between left and right wheels in centimeters (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float TrackWidthCm = 166.5f;

	/** Center of Mass (COM) height from ground plane in centimeters (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float CenterOfMassHeightCm = 38.0f;

	/** Maximum front steering wheel lock angle in degrees */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float MaxSteeringAngle = 36.0f;

	/** Manufacturer rated top speed in kilometers per hour (km/h) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float RatedTopSpeedKmh = 320.0f;

	/** Manufacturer rated 0-100 km/h acceleration benchmark in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	float Rated0To100TimeSeconds = 3.5f;

	/** Engine physics parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	FEnginePhysicsConfig Engine;

	/** Transmission physics parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	FTransmissionPhysicsConfig Transmission;

	/** Suspension physics parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	FSuspensionPhysicsConfig Suspension;

	/** Tire physics parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	FTirePhysicsConfig Tires;

	/** Aerodynamic physics parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	FAerodynamicPhysicsConfig Aerodynamics;

	/** Active assistance & braking parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	FVehicleAssistanceConfig Assistance;
};

/** Individual wheel dynamic state during physics simulation */
USTRUCT(BlueprintType)
struct FWheelPhysicsState
{
	GENERATED_BODY()

	/** Wheel Index: 0 = Front-Left, 1 = Front-Right, 2 = Rear-Left, 3 = Rear-Right */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	int32 WheelIndex = 0;

	/** Wheel rotational speed in radians per second (rad/s) */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float AngularVelocity = 0.0f;

	/** Cumulative wheel visual pitch rotation in degrees */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float VisualRotationDegrees = 0.0f;

	/** Current steer angle applied to wheel in degrees */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SteerAngleDegrees = 0.0f;

	/** Current suspension travel displacement from resting height in centimeters (cm) */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SuspensionDisplacementCm = 0.0f;

	/** Normal vertical downward force Fz acting on this tire in Newtons (N) */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float VerticalLoadN = 0.0f;

	/** Longitudinal tire slip ratio (-1.0 to +1.0) */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SlipRatio = 0.0f;

	/** Lateral tire slip angle in degrees */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SlipAngleDegrees = 0.0f;

	/** Current surface contact type */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	ESurfaceType ContactSurface = ESurfaceType::DryAsphalt;

	/** Surface friction multiplier */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float SurfaceFriction = 1.0f;

	/** Longitudinal tractive / braking force Fx generated in Newtons (N) */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float ForceX = 0.0f;

	/** Lateral cornering force Fy generated in Newtons (N) */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	float ForceY = 0.0f;

	/** True if tire has exceeded grip threshold and is actively sliding laterally */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	bool bIsSkidding = false;

	/** True if wheel is locked under heavy braking (ABS intervention target) */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	bool bIsLocked = false;

	/** True if wheel is spinning faster than ground speed under power */
	UPROPERTY(BlueprintReadOnly, Category = "Wheel")
	bool bIsSpinning = false;
};

/** Telemetry snapshot for HUD displays, logs, and analytics */
USTRUCT(BlueprintType)
struct FVehicleTelemetryData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float SpeedKmh = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float SpeedMph = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float EngineRPM = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	int32 CurrentGear = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float ThrottleInput = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float BrakeInput = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float SteeringInput = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float LongitudinalGForce = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float LateralGForce = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float TotalDownforceNewtons = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float TotalDragNewtons = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	bool bABSActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	bool bTCSActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	bool bAirbrakeActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float ChassisPitchDegrees = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Telemetry")
	float ChassisRollDegrees = 0.0f;
};

// ============================================================================
// BASE CAR CLASS
// ============================================================================

/**
 * ABaseCar
 * Comprehensive Chaos Wheeled Vehicle Physics implementation for VELOCITY X.
 * Provides realistic powertrain, transmission, tire slip, aerodynamic downforce,
 * weight transfer, differential locking, active driver aids (ABS/TCS), and visual physics.
 */
UCLASS()
class VELOCITYX_API ABaseCar : public AWheeledVehicle
{
	GENERATED_BODY()

public:
	ABaseCar();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// --- Physics Configuration Access ---

	/** Return vehicle physics settings struct */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Physics")
	virtual FVehiclePhysicsConfig GetPhysicsSettings() const { return PhysicsConfig; }

	/** Apply updated vehicle physics configuration */
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Physics")
	virtual void SetPhysicsSettings(const FVehiclePhysicsConfig& NewConfig);

	/** Get current vehicle telemetry snapshot */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	FVehicleTelemetryData GetTelemetry() const { return Telemetry; }

	/** Get wheel physics state by index (0=FL, 1=FR, 2=RL, 3=RR) */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	FWheelPhysicsState GetWheelState(int32 WheelIndex) const;

	// --- Control & Driver Input ---

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	virtual void ApplyThrottle(float Value);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	virtual void ApplyBrake(float Value);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	virtual void ApplySteering(float Value);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	virtual void ApplyHandbrake(bool bActive);

	// Direct input setters matching Team interface
	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	void SetThrottle(float Value) { ApplyThrottle(Value); }

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	void SetBrake(float Value) { ApplyBrake(Value); }

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	void SetSteering(float Value) { ApplySteering(Value); }

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Input")
	void SetHandbrake(bool bActive) { ApplyHandbrake(bActive); }

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Transmission")
	virtual void ShiftUp();

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Transmission")
	virtual void ShiftDown();

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Transmission")
	virtual void ToggleTransmissionMode();

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Transmission")
	virtual void SetAutomaticTransmission(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Utility")
	virtual void ToggleHeadlights();

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Utility")
	virtual void ResetVehicle();

	UFUNCTION(BlueprintCallable, Category = "VELOCITY X|Utility")
	virtual void ResetVehicleAtTransform(const FTransform& TargetTransform);

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Utility")
	bool AreHeadlightsOn() const { return bHeadlightsOn; }

	// --- HUD & Telemetry Direct Getters (Team Hooks) ---

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetSpeedKPH() const { return Telemetry.SpeedKmh; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetSpeedMPH() const { return Telemetry.SpeedMph; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetEngineRPM() const { return CurrentRPM; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetRedlineRPM() const { return PhysicsConfig.Engine.RedlineRPM; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	int32 GetCurrentGear() const { return CurrentGear; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	FString GetGearName() const;

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetWheelSlipRatio(int32 WheelIndex) const;

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetLongitudinalG() const { return Telemetry.LongitudinalGForce; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	float GetLateralG() const { return Telemetry.LateralGForce; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	bool IsTCSActive() const { return Telemetry.bTCSActive; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	bool IsABSActive() const { return Telemetry.bABSActive; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Telemetry")
	bool IsAirbrakeActive() const { return Telemetry.bAirbrakeActive; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|State")
	bool IsBraking() const { return EffectiveBrake > 0.05f; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|State")
	bool IsReversing() const { return CurrentGear == -1; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Suspension")
	float GetSuspensionCompression(int32 WheelIndex) const;

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Chassis")
	float GetChassisPitch() const { return ChassisPitch; }

	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Chassis")
	float GetChassisRoll() const { return ChassisRoll; }

	// Standard Socket Names for Meshes and Cameras
	static const FName SocketCockpitCamera;
	static const FName SocketHoodCamera;
	static const FName SocketChaseBoom;
	static const FName SocketExhaustLeft;
	static const FName SocketExhaustRight;

	// Dynamic Audio & VFX Callbacks
	UPROPERTY(BlueprintAssignable, Category = "VELOCITY X|Events")
	FOnTireSlipEvent OnTireSlip;

	UPROPERTY(BlueprintAssignable, Category = "VELOCITY X|Events")
	FOnSuspensionBottomOutEvent OnSuspensionBottomOut;

	UPROPERTY(BlueprintAssignable, Category = "VELOCITY X|Events")
	FOnGearChangeEvent OnGearChange;

	// --- Physics Simulation Subsystems ---

	/** Calculates surface friction multiplier for a given surface type */
	UFUNCTION(BlueprintPure, Category = "VELOCITY X|Physics")
	static float GetSurfaceFrictionCoefficient(ESurfaceType Surface);

	/** Computes weight transfer in pitch (braking/acceleration) and roll (cornering) */
	virtual void SimulateWeightTransfer(float DeltaTime, float LongitudinalAccel, float LateralAccel);

	/** Evaluates wheel slip angles and ratios based on tire velocities and ground contact */
	virtual void CalculateWheelSlip(float DeltaTime);

	/** Computes tire traction forces using modified Pacejka Magic Formula curve */
	virtual void CalculateSurfaceTraction(int32 WheelIndex, ESurfaceType Surface);

	/** Simulates engine torque curve, RPM build-up, and gear ratio torque multiplication */
	virtual void SimulateEngineAndDrivetrain(float DeltaTime);

	/** Simulates differential locking mechanism across left and right driven wheels */
	virtual void SimulateDifferential(float DeltaTime, float EngineDriveTorque);

	/** Simulates engine braking deceleration on throttle lift */
	virtual void SimulateEngineBraking(float DeltaTime);

	/** Applies Anti-lock Braking (ABS) and Traction Control (TCS) interventions */
	virtual void ApplyABSAndTractionControl(float DeltaTime);

	/** Calculates aerodynamic drag, high-speed downforce, and active airbrake surfaces */
	virtual void SimulateAerodynamics(float DeltaTime);

	// --- Visual Physics & Presentation ---

	/** Rotates wheels in pitch based on rotational angular speed and steers front wheels */
	virtual void UpdateVisualWheelRotation(float DeltaTime);

	/** Animates suspension compression/extension and chassis body pitch & roll */
	virtual void UpdateVisualSuspension(float DeltaTime);

	/** Updates headlights, brake lights, reverse lights, and active braking strobes */
	virtual void UpdateVisualLights(float DeltaTime);

	/** Modulates audio pitch and volume crossfade according to engine RPM */
	virtual void UpdateAudioPitchAndVolume(float DeltaTime);

protected:
	// --- Config Data ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VELOCITY X|Config")
	FVehiclePhysicsConfig PhysicsConfig;

	// --- Live Driver Inputs ---
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Input")
	float RawThrottle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Input")
	float RawBrake = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Input")
	float RawSteering = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Input")
	bool bRawHandbrake = false;

	// Modulated inputs after driver aids (ABS / TCS)
	float EffectiveThrottle = 0.0f;
	float EffectiveBrake = 0.0f;

	// --- Drivetrain & Powertrain State ---
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Powertrain")
	float CurrentRPM = 900.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Powertrain")
	int32 CurrentGear = 1; // -1 = Reverse, 0 = Neutral, 1..N = Forward gears

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Powertrain")
	bool bIsShifting = false;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Powertrain")
	float ShiftTimer = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Powertrain")
	bool bRevLimiterCutting = false;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Powertrain")
	float RevLimiterTimer = 0.0f;

	// --- Wheel Dynamic States (0=FL, 1=FR, 2=RL, 3=RR) ---
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Wheels")
	FWheelPhysicsState Wheels[4];

	// --- Visual & Chassis State ---
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Chassis")
	float ChassisPitch = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Chassis")
	float ChassisRoll = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Chassis")
	float ActiveAeroAngle = 0.0f; // Degrees

	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Lights")
	bool bHeadlightsOn = false;

	// --- Telemetry Snapshot ---
	UPROPERTY(BlueprintReadOnly, Category = "VELOCITY X|Telemetry")
	FVehicleTelemetryData Telemetry;

	// --- Dynamic Timers for Pulse modulation ---
	float ABSPulseTimer = 0.0f;
	bool bABSPulseHigh = false;

	// --- Visual Components ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Camera")
	USpringArmComponent* ChaseCameraArm = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Camera")
	UCameraComponent* ChaseCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Audio")
	UAudioComponent* EngineAudioComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lights")
	USpotLightComponent* HeadlightLeft = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lights")
	USpotLightComponent* HeadlightRight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lights")
	UPointLightComponent* TaillightLeft = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lights")
	UPointLightComponent* TaillightRight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VELOCITY X|Lights")
	UPointLightComponent* ReverseLight = nullptr;

	// Acceleration measurement helpers
	FVector PreviousVelocity = FVector::ZeroVector;
	FVector CurrentAcceleration = FVector::ZeroVector;

	/** Internal helper to look up engine torque at a given RPM */
	float CalculateTorqueAtRPM(float RPM) const;

	/** Detect surface below wheel using line trace query */
	ESurfaceType DetectSurfaceUnderWheel(int32 WheelIndex) const;
};
