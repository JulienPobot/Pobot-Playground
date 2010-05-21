unit InputDevMapping ;

interface

uses
  IniFiles,
  epJoystick
  ;

type

  TInputDeviceMapping = class
  private
    FInputDev : TEPJoystick ;
    FDeadZoneSize : Integer ;

    FThrottle : Integer ;
    FSteering : Integer ;
    FWinchPower : Integer ;
    FRelayButtons : array[1..8] of boolean ;

    FThrottleJSAxis : Byte ;
    FSteeringJSAxis : Byte ;
    FWinchPowerJSAxis : Byte ;
    FWinchUpJSButton : Byte ;
    FWinchDownJSButton : Byte ;
    FRelayJSButtons : array[1..8] of Byte ;

    function GetRelayButtons(Index : Integer) : boolean ;
    procedure SetDefaultMapping ;
    function SettingToJSAxis(const setting : string) : Byte ;
    procedure SetDeadZoneSize(const Value : Integer) ;
  public
    constructor Create(inputDev : TEPJoystick) ;
    procedure LoadSettings(cfgFile : TIniFile ; const sectName : string) ;
    procedure CaptureInputs ;

    property Throttle : Integer read FThrottle ;
    property Steering : Integer read FSteering ;
    property WinchPower : Integer read FWinchPower ;
    property RelayButtons[Index : Integer] : boolean read GetRelayButtons ;

    property DeadZoneSize : Integer read FDeadZoneSize ;
  end ;

implementation

uses
  SysUtils
  ;

{ TInputDeviceMapping }

constructor TInputDeviceMapping.Create(inputDev : TEPJoystick) ;
begin
  if inputDev = nil then raise Exception.Create('inputDev cannot be nil') ;

  FInputDev := inputDev ;
  SetDeadZoneSize(10) ;

  SetDefaultMapping ;
end ;

procedure TInputDeviceMapping.SetDefaultMapping ;
var
  i                 : Integer ;
begin
  FThrottleJSAxis := Ord(jaAxis1) ;
  FSteeringJSAxis := Ord(jaAxis2) ;
  FWinchPowerJSAxis := Ord(jaAxis3) ;

  for i := Low(FRelayJSButtons) to High(FRelayJSButtons) do
    FRelayJSButtons[i] := 0 ;
end ;

procedure TInputDeviceMapping.SetDeadZoneSize(const Value : Integer) ;
begin
  if Value > 0 then
    FDeadZoneSize := Value ;
end ;

procedure TInputDeviceMapping.LoadSettings(cfgFile : TIniFile ;
  const sectName : string) ;
var
  i                 : Integer ;
  opts              : TAxisOptions ;
begin
  if cfgFile = nil then raise Exception.Create('cfgFile cannot be nil') ;
  if sectName = '' then raise Exception.Create('sectName cannot be an empty string') ;

  SetDeadZoneSize(cfgFile.ReadInteger(sectName, 'dead-zone-size', 0)) ;

  FThrottleJSAxis := SettingToJSAxis(cfgFile.ReadString(sectName, 'throttle-axis', 'y')) ;
  FSteeringJSAxis := SettingToJSAxis(cfgFile.ReadString(sectName, 'steering-axis', 'x')) ;

  FWinchPowerJSAxis := SettingToJSAxis(cfgFile.ReadString(sectName, 'winch-power-axis', '')) ;
  if FWinchPowerJSAxis = 0 then begin
    FWinchDownJSButton := cfgFile.ReadInteger(sectName, 'winch-down-button', 1) ;
    FWinchUpJSButton := cfgFile.ReadInteger(sectName, 'winch-up-button', 2) ;
  end
  else begin
    FWinchDownJSButton := cfgFile.ReadInteger(sectName, 'winch-down-button', 0) ;
    FWinchUpJSButton := cfgFile.ReadInteger(sectName, 'winch-up-button', 0) ;

    if (FWinchDownJSButton <> 0) and (FWinchUpJSButton = 0) then
      raise Exception.Create('missing winch-up-button setting') ;
    if (FWinchDownJSButton = 0) and (FWinchUpJSButton <> 0) then
      raise Exception.Create('missing winch-down-button setting') ;

    opts := FInputDev.AxisOptions ;
    if FWinchDownJSButton = 0 then
      Include(opts.AsArray[TJoystickAxis(FWinchPowerJSAxis)], aoCentered)
    else
      Exclude(opts.AsArray[TJoystickAxis(FWinchPowerJSAxis)], aoCentered) ;
    FInputDev.AxisOptions := opts ;
  end ;

  for i := Low(FRelayJSButtons) to High(FRelayJSButtons) do begin
    FRelayJSButtons[i] := cfgFile.ReadInteger(sectName, 'relay-btn.' + IntToStr(i), 0) ;
  end ;
end ;

function TInputDeviceMapping.SettingToJSAxis(const setting : string) : Byte ;
var
  s                 : string ;
  axis              : TJoystickAxis ;
const
  AxisValidIds      : array[TJoystickAxis] of string = (
    '/x/X/1/', '/y/Y/2/', '/z/Z/3/', '/r/R/4/', '/u/U/5/', '/v/V/6/'
    ) ;

begin
  if setting = '' then raise Exception.Create('SettingToJSAxis called with empty argument') ;

  s := '/' + setting + '/' ;
  for axis := Low(AxisValidIds) to High(AxisValidIds) do
    if pos(s, AxisValidIds[axis]) <> 0 then begin
      Result := Ord(axis) ;
      Exit ;
    end ;

  raise Exception.Create('invalid setting value : ' + setting) ;
end ;

procedure TInputDeviceMapping.CaptureInputs ;
var
  btn               : Integer ;
  i                 : Integer ;
begin
  // Propulsion et direction
  FThrottle := FInputDev.AxisPositions.AsArray[TJoystickAxis(FThrottleJSAxis)] ;
  if abs(FThrottle) < FDeadZoneSize then FThrottle := 0 ;

  FSteering := FInputDev.AxisPositions.AsArray[TJoystickAxis(FSteeringJSAxis)] ;
  if abs(FSteering) < FDeadZoneSize then FSteering := 0 ;

  // Commande du treuil

  // - commande avec réglage de la vitesse
  if FWinchPowerJSAxis <> 0 then begin
    if FWinchUpJSButton = 0 then
      FWinchPower := FInputDev.AxisPositions.AsArray[TJoystickAxis(FWinchPowerJSAxis)]
    else begin
      if FInputDev.IsButtonPressed(TJoystickButton(FWinchUpJSButton)) then
        FWinchPower := -abs(FInputDev.AxisPositions.AsArray[TJoystickAxis(FWinchPowerJSAxis)])
      else if FInputDev.IsButtonPressed(TJoystickButton(FWinchDownJSButton)) then
        FWinchPower := abs(FInputDev.AxisPositions.AsArray[TJoystickAxis(FWinchPowerJSAxis)])
      else
        FWinchPower := 0 ;

    end ;
    if abs(FWinchPower) < FDeadZoneSize then FWinchPower := 0 ;
  end

    // - commande tout-ou-rien
  else begin
    if FInputDev.IsButtonPressed(TJoystickButton(FWinchUpJSButton)) then
      FWinchPower := -100
    else if FInputDev.IsButtonPressed(TJoystickButton(FWinchDownJSButton)) then
      FWinchPower := 100
    else
      FWinchPower := 0 ;
  end ;

  // Boutons

  for i := Low(FRelayJSButtons) to High(FRelayJSButtons) do begin
    btn := FRelayJSButtons[i] ;
    if btn <> 0 then
      FRelayButtons[i] := FInputDev.IsButtonPressed(TJoystickButton(btn)) ;
  end ;
end ;

function TInputDeviceMapping.GetRelayButtons(Index : Integer) : boolean ;
begin
  Result := FRelayButtons[Index] ;
end ;

end.

