unit RaftData ;

interface

uses
  SysUtils,
  MedusaUtils
  ;

const
  ADC_LOCAL_CNT     = 6 ;
  ADC_I2C_CNT       = 4 ;
  NMEApos_LEN       = 24 ;

  POLY_REG_ORDER    = 3 ;

type

  TBattery = (batLogic, batPower) ;
  TMotorSelector = (motLeft, motRight, motWinch) ;
  TPropellerMotor = motLeft..motRight ;

  TStatusFlag = (sfWinchUp, sfWinchDown) ;
  TStatusFlags = set of TStatusFlag ;

  THardwareFlag = (hfCompass, hfGPS, hfRemoteSensors, hfTempMotor1, hfTempMotor2) ;
  THardwareFlags = set of THardwareFlag ;

  TSensorType = (stLocal, stRemote) ;
  TLocalSensorId = 1..ADC_LOCAL_CNT ;
  TRemoteSensorId = 1..ADC_I2C_CNT ;

  TSystemData = record
    batteryLevel : array[TBattery] of byte ;
    motorTemp : array[TPropellerMotor] of byte ;
    statusFlags : byte ;
    hwFlags : byte ;
    winchTicks : Smallint ;
  end ;

  TDMS = record
    degrees : Smallint ;
    minutes : byte ;
    seconds : byte ;
  end ;

  TGeoPosition = record
    lon : TDMS ;
    lat : TDMS ;
  end ;

  TPositionData = record
    geoPos : TGeoPosition ;
    heading : word ;
  end ;

  TUserData = record
    adc_local : array[TLocalSensorId] of byte ;
    adc_i2c : array[TRemoteSensorId] of byte ;
  end ;

  TExtendedData = record
    positionData : TPositionData ;
    systemData : TSystemData ;
    userData : TUserData ;
  end ;

  TSensor = class(TObject)
  private
    FName : string ;
    FUnits : string ;
    FConversionPolynom : TPolynom ;
    FRawValue : Integer ;
    FValue : double ;
    FOwnsPolynom : boolean ;
    procedure SetRawValue(v : Integer) ; virtual ;
  public
    constructor Create ;
    destructor Destroy ; override ;

    procedure SetPolynomCoefficients(coeffs : array of double);

    property ConversionPolynom : TPolynom read FConversionPolynom write FConversionPolynom ;
    property Name : string read FName write FName ;
    property OwnsPolynom : boolean read FOwnsPolynom write FOwnsPolynom ;
    property RawValue : Integer read FRawValue write SetRawValue ;
    property Units : string read FUnits write FUnits ;
    property Value : double read FValue ;
  end ;

  TBatterySensor = class(TSensor)
  private
    procedure SetRawValue(v : Integer) ; override ;
  public
    constructor Create ;
  end ;

  TGPSPosition = class
  private
    FLongitude : TDMS ;
    FLatitude : TDMS ;
    FInvalidPositionString : string ;
    FLongitudeFix : Integer ;
    FLatitudeFix : Integer ;
    procedure Initialize ;
    procedure ApplyPositionFix ;
  public
    constructor Create(const NMEAstring : string = '') ; overload ;
    constructor Create(lon : TDMS ; lat : TDMS) ; overload ;

    function IsValid : boolean ;

    function AsString : string ;
    procedure FromString(const NMEAstring : string) ;
    procedure SetRawValue(lon : TDMS ; lat : TDMS) ;

    property InvalidPositionString : string read FInvalidPositionString write FInvalidPositionString ;
    property Latitude : TDMS read FLatitude ;
    property Longitude : TDMS read FLongitude ;

    property LatitudeFix : Integer read FLatitudeFix write FLatitudeFix ;
    property LongitudeFix : Integer read FLongitudeFix write FLongitudeFix ;
  end ;

  TRaftData = class
  private
    FLocalSensors : array[TLocalSensorId] of TSensor ;
    FRemoteSensors : array[TRemoteSensorId] of TSensor ;
    FHasRemoteSensors : boolean ;

    FBatterySensors : array[TBattery] of TBatterySensor ;
    FBatteryCriticalLevel : array[TBattery] of double ;
    FBatteryCriticalLevelReached : array[TBattery] of boolean ;

    FBatteryVoltage : array[TBattery] of double ;
    FMotorTemp : array[TMotorSelector] of Integer ;
    FStatusFlags : TStatusFlags ;
    FHardwareFlags : THardwareFlags ;
    FWinchedLength : Cardinal ;         // cm
    FWinchCmPerTick : Single ;

    FGPSPosition : TGPSPosition ;
    FHeading : Integer ;

    function GetBatteryCriticalLevel(bat : TBattery) : double ;
    procedure SetBatteryCriticalLevel(bat : TBattery ; const Value : double) ;
    function GetBatteryCriticalLevelReached(bat : TBattery) : boolean ;
    function GetBatterySensors(bat : TBattery) : TBatterySensor ;
    function GetBatteryVoltage(bat : TBattery) : double ;

    function GetLocalSensors(id : TLocalSensorId) : TSensor ;
    function GetRemoteSensors(id : TRemoteSensorId) : TSensor ;
    function GetMotorTemp(motor : TMotorSelector) : Integer ;
    procedure RawToStatusFlags(RawValue : byte ; var statusFlags : TStatusFlags) ;
    procedure RawToHardwareFlags(RawValue : byte ; var hwFlags : THardwareFlags) ;
  public
    constructor Create ;
    destructor Destroy ; override ;
    procedure AddSensor(aType : TSensorType ; anId : Integer ; const aName : string ; const Units : string ; const
        polyCoeffs : array of double);

    procedure ResetBatteryAlarms ;

    procedure ProcessExtendedFrame(const data : TExtendedData) ;

    property BatteryCriticalLevel[bat : TBattery] : double
    read GetBatteryCriticalLevel write SetBatteryCriticalLevel ;
    property BatteryCriticalLevelReached[bat : TBattery] : boolean read GetBatteryCriticalLevelReached ;
    property BatterySensors[bat : TBattery] : TBatterySensor read GetBatterySensors ;
    property BatteryVoltage[bat : TBattery] : double read GetBatteryVoltage ;
    property GPSPosition : TGPSPosition read FGPSPosition ;
    property HardwareFlags : THardwareFlags read FHardwareFlags ;
    property heading : Integer read FHeading ;
    property LocalSensors[id : TLocalSensorId] : TSensor read GetLocalSensors ;
    property statusFlags : TStatusFlags read FStatusFlags ;
    property motorTemp[motor : TMotorSelector] : Integer read GetMotorTemp ;
    property RemoteSensors[id : TRemoteSensorId] : TSensor read GetRemoteSensors ;
    property HasRemoteSensors : boolean read FHasRemoteSensors ;
    property WinchedLength : Cardinal read FWinchedLength ;
    property WinchCmPerTick : Single read FWinchCmPerTick write FWinchCmPerTick ;
  end ;

  RaftDataError = class(Exception) ;
  RDE_OutOfBounds = class(RaftDataError) ;
  RDE_IllegalArgument = class(RaftDataError) ;
  RDE_InvalidConversionRule = class(RaftDataError) ;

procedure DMSAddDelta(position : TDMS ; deltaSecs : Integer) ;
function DMSToSecs(position : TDMS) : Integer ;
function SecsToDMS(secs : Integer) : TDMS ;

implementation

uses
  Math,
  StrUtils ;

procedure DMSAddDelta(position : TDMS ; deltaSecs : Integer) ;
begin
  position := SecsToDMS(DMSToSecs(position) + deltaSecs) ;
end ;

function DMSToSecs(position : TDMS) : Integer ;
begin
  Result := position.degrees * 3600 + position.minutes * 60 + position.seconds ;
end ;

function SecsToDMS(secs : Integer) : TDMS ;
begin
  Result.degrees := secs div 3600 ;
  secs := secs mod 3600 ;
  Result.minutes := secs div 60 ;
  Result.seconds := secs mod 60 ;
end ;

//--------------------------------------------------------------------------------
// TSensor
//--------------------------------------------------------------------------------

constructor TSensor.Create ;
begin
  FOwnsPolynom := true ;
end ;

destructor TSensor.Destroy ;
begin
  if FOwnsPolynom then
    FConversionPolynom.Free ;
  inherited ;
end ;

procedure TSensor.SetPolynomCoefficients(coeffs : array of double);
begin
  if FOwnsPolynom then
    FConversionPolynom.Free ;
  FConversionPolynom := TPolynom.Create(coeffs) ;
  FOwnsPolynom := true ;
end ;

procedure TSensor.SetRawValue(v : Integer) ;
begin
  FRawValue := v ;
  FValue := FConversionPolynom.Evaluate(v) ;
end ;

//--------------------------------------------------------------------------------
// TBatterySensor
//--------------------------------------------------------------------------------

constructor TBatterySensor.Create ;
begin
  inherited ;
  Units := '%' ;
end ;

procedure TBatterySensor.SetRawValue(v : Integer) ;
begin
  inherited ;
  if FValue > 100 then
    FValue := 100
  else if FValue < 0 then
    FValue := 0 ;
end ;

//--------------------------------------------------------------------------------
// TRaftData
//--------------------------------------------------------------------------------

constructor TRaftData.Create ;
var
  i                 : Integer ;
begin
  {
    Création de deux capteurs spécifiques par la capture des mesures de tension des batteries.
  }
  FBatterySensors[batLogic] := TBatterySensor.Create ;
  FBatterySensors[batLogic].Name := 'batterie élec.' ;
  {
    L'assignation de la formule de conversion par défaut ici fait planter TExpress
    => ce sera fait pendant la phase de configuration, en définissant la valeur par défaut
    pour le read depuis le fichier de configuration.
  }

  FBatterySensors[batPower] := TBatterySensor.Create ;
  FBatterySensors[batPower].Name := 'batterie moteurs' ;

  for i := Low(FLocalSensors) to High(FLocalSensors) do
    FLocalSensors[i] := nil ;
  for i := Low(FRemoteSensors) to High(FRemoteSensors) do
    FRemoteSensors[i] := nil ;
  FHasRemoteSensors := false ;

  FGPSPosition := TGPSPosition.Create ;
end ;

destructor TRaftData.Destroy ;
var
  bat               : TBattery ;
  i                 : Integer ;
begin
  for bat := Low(TBattery) to High(TBattery) do begin
    FBatterySensors[bat].Free ;
  end ;

  for i := Low(FLocalSensors) to High(FLocalSensors) do
    FLocalSensors[i].Free ;
  for i := Low(FRemoteSensors) to High(FRemoteSensors) do
    FRemoteSensors[i].Free ;

  FGPSPosition.Free ;

  inherited ;
end ;

procedure TRaftData.AddSensor(aType : TSensorType ; anId : Integer ; const aName : string ; const Units : string ;
    const polyCoeffs : array of double);
var
  sensorDef         : TSensor ;

begin
  case aType of
    stLocal :
      if not (anId in [Low(TLocalSensorId)..High(TLocalSensorId)]) then
        raise RDE_OutOfBounds.Create('id capteur surface hors limites') ;

    stRemote :
      if not (anId in [Low(TRemoteSensorId)..High(TRemoteSensorId)]) then
        raise RDE_OutOfBounds.Create('id capteur fond hors limites') ;
  end ;

  if aName = '' then raise RDE_IllegalArgument.Create('nom de capteur invalide') ;

  sensorDef := TSensor.Create ;
  sensorDef.Name := aName ;
  sensorDef.Units := Units ;
  sensorDef.SetPolynomCoefficients(polyCoeffs) ;

  case aType of
    stLocal :
      FLocalSensors[anId] := sensorDef ;

    stRemote : begin
        FRemoteSensors[anId] := sensorDef ;
        FHasRemoteSensors := true ;
      end ;
  end ;
end ;

function TRaftData.GetBatteryCriticalLevel(bat : TBattery) : double ;
begin
  Result := FBatteryCriticalLevel[bat] ;
end ;

procedure TRaftData.SetBatteryCriticalLevel(bat : TBattery ;
  const Value : double) ;
begin
  FBatteryCriticalLevel[bat] := Value ;
  FBatteryCriticalLevelReached[bat] := false ;
end ;

function TRaftData.GetBatteryCriticalLevelReached(bat : TBattery) : boolean ;
begin
  Result := FBatteryCriticalLevelReached[bat] ;
end ;

function TRaftData.GetBatterySensors(bat : TBattery) : TBatterySensor ;
begin
  Result := FBatterySensors[bat] ;
end ;

procedure TRaftData.ResetBatteryAlarms ;
var
  bat               : TBattery ;
begin
  for bat := Low(TBattery) to High(TBattery) do
    FBatteryCriticalLevelReached[bat] := false ;
end ;

function TRaftData.GetBatteryVoltage(bat : TBattery) : double ;
begin
  Result := FBatteryVoltage[bat] ;
end ;

function TRaftData.GetLocalSensors(id : TLocalSensorId) : TSensor ;
begin
  Result := FLocalSensors[id] ;
end ;

function TRaftData.GetRemoteSensors(id : TRemoteSensorId) : TSensor ;
begin
  Result := FRemoteSensors[id] ;
end ;

function TRaftData.GetMotorTemp(motor : TMotorSelector) : Integer ;
begin
  Result := FMotorTemp[motor] ;
end ;

procedure TRaftData.ProcessExtendedFrame(const data : TExtendedData) ;
var
  bat               : TBattery ;
  level             : double ;
  mot               : TMotorSelector ;
  i                 : Integer ;
begin
  for bat := Low(TBattery) to High(TBattery) do begin
    FBatterySensors[bat].SetRawValue(data.systemData.batteryLevel[bat]) ;

    level := FBatterySensors[bat].Value ;

    if level <= FBatteryCriticalLevel[bat] then FBatteryCriticalLevelReached[bat] := true ;

    {
      Le niveau de la batterie logique doit être traité différemment, car lorque
      la tension descend de telle sorte que le VCC régulé passe sous les 5V, la conversion
      devient fausse, et la courbe de conversion s'inverse, donnant une indication
      du niveau de la batterie à l'inverse de la réalité.
      On définit donc un seuil de niveau qu'on ne peut franchir qu'en descendant. Lorsqu'il
      a été franchi, on inverse la courbe de conversion et on active la notification d'urgence.

      Le problème ne se pose pas pour les batteries moteur, car elle n'ont aucune incidence
      sur l'alimentation de la carte.
    }
    if (bat = batLogic) and FBatteryCriticalLevelReached[bat] then begin
      level := FBatteryCriticalLevel[bat] - abs(level - FBatteryCriticalLevel[bat]) ;
    end ;

    FBatteryVoltage[bat] := level ;
  end ;

  for mot := motLeft to motRight do
    FMotorTemp[mot] := data.systemData.motorTemp[mot] ;

  RawToStatusFlags(data.systemData.statusFlags, FStatusFlags) ;
  RawToHardwareFlags(data.systemData.hwFlags, FHardwareFlags) ;
  FWinchedLength := Round(data.systemData.winchTicks * FWinchCmPerTick) ;

  for i := Low(FLocalSensors) to High(FLocalSensors) do begin
    if Assigned(FLocalSensors[i]) then
      FLocalSensors[i].SetRawValue(data.userData.adc_local[i]) ;
  end ;

  for i := Low(FRemoteSensors) to High(FRemoteSensors) do begin
    if Assigned(FRemoteSensors[i]) then
      FRemoteSensors[i].SetRawValue(data.userData.adc_i2c[i]) ;
  end ;

  FGPSPosition.SetRawValue(data.positionData.geoPos.lon, data.positionData.geoPos.lat) ;
  FHeading := Round(data.positionData.heading / 10) mod 360 ;
end ;

procedure TRaftData.RawToStatusFlags(RawValue : byte ; var statusFlags : TStatusFlags) ;
var
  flag              : TStatusFlag ;
begin
  statusFlags := [] ;
  RawValue := RawValue and $0F ;        // seuls les 4 bits de poids faible sont utilisés
  for flag := Low(TStatusFlag) to High(TStatusFlag) do begin
    if (RawValue and $01) = 0 then      // logique négative pour les fins de course
      Include(statusFlags, flag) ;
    RawValue := RawValue shr 1 ;
  end ;
end ;

procedure TRaftData.RawToHardwareFlags(RawValue : byte ; var hwFlags : THardwareFlags) ;
var
  flag              : THardwareFlag ;
begin
  hwFlags := [] ;
  for flag := Low(THardwareFlag) to High(THardwareFlag) do begin
    if (RawValue and $01) <> 0 then
      Include(hwFlags, flag) ;
    RawValue := RawValue shr 1 ;
  end ;
end ;

{ TGPSPosition }

constructor TGPSPosition.Create(const NMEAstring : string) ;
begin
  Initialize ;
  if NMEAstring <> '' then
    FromString(NMEAstring) ;
end ;

constructor TGPSPosition.Create(lon, lat : TDMS) ;
begin
  Initialize ;
  SetRawValue(lon, lat) ;
end ;

procedure TGPSPosition.Initialize ;
begin
  FInvalidPositionString := '' ;
end ;

function TGPSPosition.IsValid() : boolean ;
begin
  Result := (Latitude.degrees <> 0) or (Longitude.degrees <> 0) ;
end ;

function TGPSPosition.AsString : string ;
const
  NS                : array[boolean] of char = ('S', 'N') ;
  EW                : array[boolean] of char = ('W', 'E') ;
begin
  if IsValid then
    Result := Format('%.2d°%.2d''%.2d" %s - %.3d°%.2d''%.2d" %s', [
      abs(Latitude.degrees), Latitude.minutes, Latitude.seconds, NS[Latitude.degrees >= 0],
        abs(Longitude.degrees), Longitude.minutes, Longitude.seconds, EW[Longitude.degrees >= 0]
        ])
  else
    Result := InvalidPositionString ;
end ;

procedure TGPSPosition.FromString(const NMEAstring : string) ;
{
ddmm.mmmm,N,dddmm.mmmm,E
}
var
  mn                : double ;
  oldSep            : char ;
begin
  if NMEAstring[1] in ['0'..'9'] then begin
    oldSep := DecimalSeparator ;
    DecimalSeparator := '.' ;
    try
      FLatitude.degrees := StrToInt(Copy(NMEAstring, 1, 2)) ;
      if NMEAstring[11] = 'S' then FLatitude.degrees := -FLatitude.degrees ;

      mn := StrToFloat(Copy(NMEAstring, 3, 7)) ;
      FLatitude.minutes := Floor(mn) ;
      FLatitude.seconds := Round(60 * (mn - FLatitude.minutes)) ;

      FLongitude.degrees := StrToInt(Copy(NMEAstring, 13, 3)) ;
      if NMEAstring[24] = 'W' then FLongitude.degrees := -FLongitude.degrees ;

      mn := StrToFloat(Copy(NMEAstring, 16, 7)) ;
      FLongitude.minutes := Floor(mn) ;
      FLongitude.seconds := Round(60 * (mn - FLongitude.minutes)) ;

      ApplyPositionFix ;

    finally
      DecimalSeparator := oldSep ;
    end ;
  end ;
end ;

procedure TGPSPosition.SetRawValue(lon, lat : TDMS) ;
begin
  FLongitude := lon ;
  FLatitude := lat ;
  ApplyPositionFix ;
end ;

procedure TGPSPosition.ApplyPositionFix ;
begin
  if FLongitudeFix <> 0 then
    DMSAddDelta(FLongitude, FLongitudeFix) ;
  if FLatitudeFix <> 0 then
    DMSAddDelta(FLatitude, FLatitudeFix) ;
end ;

end.

