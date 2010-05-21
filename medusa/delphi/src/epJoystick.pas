unit epJoystick ;

interface

uses
  Classes,
  SysUtils,
  ExtCtrls,
  MMSystem
  ;

const
  MAX_AXIS          = 6 ;

type

  TepJoystick = class ;

  TJoystickId = (idJoystick1, idJoystick2) ;

  TAxisNum = 1..MAX_AXIS ;
  TJoystickAxis = (jaAxis1 = 1, jaAxis2, jaAxis3, jaAxis4, jaAxis5, jaAxis6) ;
  TJoystickAxisSet = set of TJoystickAxis ;

  TButtonNum = 1..32 ;
  TJoystickButton = (
    jbButton1 = 1, jbButton2, jbButton3, jbButton4, jbButton5, jbButton6, jbButton7, jbButton8, jbButton9,
    jbButton11, jbButton12, jbButton13, jbButton14, jbButton15, jbButton16, jbButton17, jbButton18, jbButton19,
    jbButton21, jbButton22, jbButton23, jbButton24, jbButton25, jbButton26, jbButton27, jbButton28, jbButton29,
    jbButton31, jbButton32
    ) ;
  TJoystickButtonSet = set of TJoystickButton ;

  TAxisValues = record case boolean of
      false : (
        AsArray : array[TJoystickAxis] of Integer ;
        ) ;
      true : (
        X : Integer ;
        Y : Integer ;
        Z : Integer ;
        R : Integer ;
        U : Integer ;
        V : Integer ;
        ) ;
  end ;

  TAxisScales = record case boolean of
      false : (
        AsArray : array[TJoystickAxis] of Double ;
        ) ;
      true : (
        X : Double ;
        Y : Double ;
        Z : Double ;
        R : Double ;
        U : Double ;
        V : Double ;
        ) ;
  end ;

  TAxisOption = (aoCentered, aoInverted) ;
  TAxisOptionSet = set of TAxisOption ;

  TAxisOptions = record case boolean of
      false : (
        AsArray : array[TJoystickAxis] of TAxisOptionSet ;
        ) ;
      true : (
        X : TAxisOptionSet ;
        Y : TAxisOptionSet ;
        Z : TAxisOptionSet ;
        R : TAxisOptionSet ;
        U : TAxisOptionSet ;
        V : TAxisOptionSet ;
        ) ;
  end ;

  TDeviceErrorEvent = procedure(Sender : TepJoystick ; ErrorCode : Cardinal) of object ;
  TJoystickEvent = procedure(Sender : TepJoystick) of object ;

  TepJoystick = class(TComponent)
  private
    FtmrPolling : TTimer ;
    FJoystickId : TJoystickId ;
    FDeviceError : Integer ;
    FJoyCaps : TJoyCaps ;
    FJoyInfoEx : TJoyInfoEx ;
    FOnDeviceError : TDeviceErrorEvent ;
    FAttached : boolean ;
    FRaiseErrors : boolean ;

    FPositions : TAxisValues ;
    FMidScales : TAxisValues ;
    FScales : TAxisScales ;

    FAxisOptions : TAxisOptions ;

    FSensitivity : Integer ;

    FButtons : Cardinal ;
    FPressedButtons : TJoystickButtonSet ;

    FChangedButtons : TJoystickButtonSet ;
    FChangedAxis : TJoystickAxisSet ;

    FFirstRead : boolean ;

    FOnJoystickChange : TJoystickEvent ;
    FOnJoystickPoll : TJoystickEvent ;

    function GetPollingPeriod : Integer ;
    procedure SetPollingPeriod(const Value : Integer) ;
    function GetEnabled : boolean ;
    procedure SetEnabled(const Value : boolean) ;
    procedure InitializeJoystick ;
    function GetDeviceOK : boolean ;

    procedure NotifyDeviceError ;
    function IsOK(errCode : Cardinal) : boolean ;
    procedure SetAxisOptionsX(const Value : TAxisOptionSet) ;
    procedure SetAxisOptionsR(const Value : TAxisOptionSet) ;
    procedure SetAxisOptionsU(const Value : TAxisOptionSet) ;
    procedure SetAxisOptionsV(const Value : TAxisOptionSet) ;
    procedure SetAxisOptionsY(const Value : TAxisOptionSet) ;
    procedure SetAxisOptionsZ(const Value : TAxisOptionSet) ;
    function ConvertAxisValue(axis : TJoystickAxis ; rawValue : Integer) : Integer ;
    procedure NotifyJoystickChange ;
    procedure NotifyJoystickPoll ;

  protected
    procedure tmrJoystickTimer(Sender : TObject) ;

  public
    constructor Create(aOwner : TComponent) ; override ;

    function GetJoyCapsAsText : TStrings ;
    function GetJoyCaps : TJoyCaps ;

    function IsButtonPressed(btn : TJoystickButton) : boolean ;

    property Attached : boolean read FAttached ;
    property DeviceOK : boolean read GetDeviceOK ;
    property DeviceError : Integer read FDeviceError ;
    property AxisOptions : TAxisOptions read FAxisOptions write FAxisOptions ;

    property XPos : Integer read FPositions.X ;
    property YPos : Integer read FPositions.Y ;
    property ZPos : Integer read FPositions.Z ;
    property RPos : Integer read FPositions.R ;
    property UPos : Integer read FPositions.U ;
    property VPos : Integer read FPositions.V ;
    property AxisPositions : TAxisValues read FPositions ;
    property Buttons : Cardinal read FButtons ;
    property ChangedAxis : TJoystickAxisSet read FChangedAxis ;
    property ChangedButtons : TJoystickButtonSet read FChangedButtons ;
    property PressedButtons : TJoystickButtonSet read FPressedButtons ;

  published
    property PollingPeriod : Integer read GetPollingPeriod write SetPollingPeriod default 100 ;
    property Enabled : boolean read GetEnabled write SetEnabled default false ;
    property JoystickId : TJoystickId read FJoystickId write FJoystickId default idJoystick1 ;
    property RaiseErrors : boolean read FRaiseErrors write FRaiseErrors default true ;
    property Sensitivity : Integer read FSensitivity write FSensitivity default 5 ;

    property AxisOptionsX : TAxisOptionSet read FAxisOptions.X write SetAxisOptionsX default [aoCentered] ;
    property AxisOptionsY : TAxisOptionSet read FAxisOptions.Y write SetAxisOptionsY default [aoCentered] ;
    property AxisOptionsZ : TAxisOptionSet read FAxisOptions.Z write SetAxisOptionsZ default [aoCentered] ;
    property AxisOptionsR : TAxisOptionSet read FAxisOptions.R write SetAxisOptionsR default [aoCentered] ;
    property AxisOptionsU : TAxisOptionSet read FAxisOptions.U write SetAxisOptionsU default [aoCentered] ;
    property AxisOptionsV : TAxisOptionSet read FAxisOptions.V write SetAxisOptionsV default [aoCentered] ;

    // events
    property OnDeviceError : TDeviceErrorEvent read FOnDeviceError write FOnDeviceError ;
    property OnJoystickChange : TJoystickEvent read FOnJoystickChange write FOnJoystickChange ;
    property OnJoystickPoll : TJoystickEvent read FOnJoystickPoll write FOnJoystickPoll ;
  end ;

  TJoystickError = class(Exception)
  private
    FErrorCode : Integer ;
  public
    constructor Create(errCode : Integer ; const msg : string = '') ;
    property ErrorCode : Integer read FErrorCode ;
  end ;

procedure Register ;

implementation

{$IFDEF CODESITE}
uses
  CSIntf
  ;
{$ENDIF}

procedure Register ;
begin
  RegisterComponents('User Components', [TepJoystick]) ;
end ;

const
  JoystickIdToMMid  : array[TJoystickId] of Cardinal = (JOYSTICKID1, JOYSTICKID2) ;

  { TepJoystick }

constructor TepJoystick.Create(aOwner : TComponent) ;
var
  axis              : TJoystickAxis ;
begin
  inherited Create(aOwner) ;

  FtmrPolling := TTimer.Create(Self) ;
  FtmrPolling.OnTimer := tmrJoystickTimer ;

  FDeviceError := JOYERR_UNPLUGGED ;
  FButtons := 0 ;
  Fillchar(FPositions, sizeof(FPositions), 0) ;

  for axis := Low(TJoystickAxis) to High(TJoystickAxis) do
    FAxisOptions.AsArray[axis] := [aoCentered] ;

  FAttached := false ;
  JoystickId := idJoystick1 ;
  Enabled := false ;
  PollingPeriod := 100 ;
  RaiseErrors := true ;
  Sensitivity := 5 ;

  FFirstRead := true ;
end ;

procedure TepJoystick.InitializeJoystick ;
begin
  if not IsOK(joyGetDevCaps(JoystickIdToMMid[FJoystickId], @FJoyCaps, sizeof(joyCaps))) then exit ;

  FScales.X := 100 / (FJoyCaps.wXmax - FJoyCaps.wXmin) * 2 ;
  FScales.Y := 100 / (FJoyCaps.wYmax - FJoyCaps.wYmin) * 2 ;
  FScales.Z := 100 / (FJoyCaps.wZmax - FJoyCaps.wZmin) * 2 ;
  FScales.R := 100 / (FJoyCaps.wRmax - FJoyCaps.wRmin) * 2 ;
  FScales.U := 100 / (FJoyCaps.wUmax - FJoyCaps.wUmin) * 2 ;
  FScales.V := 100 / (FJoyCaps.wVmax - FJoyCaps.wVmin) * 2 ;

  FMidScales.X := (FJoyCaps.wXmax + FJoyCaps.wXmin) div 2 ;
  FMidScales.Y := (FJoyCaps.wYmax + FJoyCaps.wYmin) div 2 ;
  FMidScales.Z := (FJoyCaps.wZmax + FJoyCaps.wZmin) div 2 ;
  FMidScales.R := (FJoyCaps.wRmax + FJoyCaps.wRmin) div 2 ;
  FMidScales.U := (FJoyCaps.wUmax + FJoyCaps.wUmin) div 2 ;
  FMidScales.V := (FJoyCaps.wVmax + FJoyCaps.wVmin) div 2 ;

  FAttached := true ;
end ;

function TepJoystick.ConvertAxisValue(axis : TJoystickAxis ; rawValue : Integer) : Integer ;
begin
{$IFDEF CODESITE}
  //  CodeSite.SendFmtMsg('axis=%d - rawValue=%d', [Ord(axis), rawValue]) ;
{$ENDIF}

  if aoCentered in FAxisOptions.AsArray[axis] then begin
    Result := Round((rawValue - FMidScales.AsArray[axis]) * FScales.AsArray[axis]) ;
    if aoInverted in FAxisOptions.AsArray[axis] then
      Result := -Result ;
  end
  else begin
    Result := Round((rawValue) * FScales.AsArray[axis] / 2) ;
    if aoInverted in FAxisOptions.AsArray[axis] then
      Result := 100 - Result ;
  end ;
end ;

function TepJoystick.GetPollingPeriod : Integer ;
begin
  Result := FtmrPolling.Interval ;
end ;

procedure TepJoystick.SetPollingPeriod(const Value : Integer) ;
begin
  FtmrPolling.Interval := Value ;
end ;

function TepJoystick.GetEnabled : boolean ;
begin
  Result := FtmrPolling.Enabled ;
end ;

procedure TepJoystick.SetEnabled(const Value : boolean) ;
begin
  InitializeJoystick ;

  FtmrPolling.Enabled := Value and FAttached ;
end ;

function TepJoystick.GetDeviceOK : boolean ;
begin
  Result := FDeviceError = JOYERR_NOERROR ;
end ;

procedure TepJoystick.tmrJoystickTimer(Sender : TObject) ;
var
  prevPos           : TAxisValues ;
  axisValuesPtr     : ^TAxisValues ;
  axis              : TJoystickAxis ;
  newVal            : Integer ;
  changedBtnMask    : Cardinal ;
  btnMask           : Cardinal ;
  btn               : TJoystickButton ;
begin
  if not (Attached and DeviceOK) then begin
    FtmrPolling.Enabled := false ;
    exit ;
  end ;

  Fillchar(FJoyInfoEx, sizeof(FJoyInfoEx), 0) ;
  FJoyInfoEx.dwSize := sizeof(FJoyInfoEx) ;
  FJoyInfoEx.dwFlags := JOY_RETURNALL ;

  if not IsOK(joyGetPosEx(JoystickIdToMMid[FJoystickId], @FJoyInfoEx)) then exit ;

  prevPos := FPositions ;
  axisValuesPtr := @FJoyInfoEx.wXpos ;
  FChangedAxis := [] ;
  for axis := Low(TJoystickAxis) to High(TJoystickAxis) do begin
    newVal := ConvertAxisValue(axis, axisValuesPtr.AsArray[axis]) ;
    if (abs(newVal - prevPos.AsArray[axis]) > Sensitivity) or FFirstRead then begin
      Include(FChangedAxis, axis) ;
      FPositions.AsArray[axis] := newVal ;
    end ;
  end ;

  FChangedButtons := [] ;
  if (FJoyInfoEx.wButtons <> FButtons) or FFirstRead then begin
    FPressedButtons := [] ;
    btnMask := 1 ;
    changedBtnMask := FJoyInfoEx.wButtons xor FButtons ;
    for btn := Low(TJoystickButton) to High(TJoystickButton) do begin
      if (btnMask and FJoyInfoEx.wButtons) <> 0 then Include(FPressedButtons, btn) ;
      if btnMask and changedBtnMask <> 0 then
        Include(FChangedButtons, btn) ;
      btnMask := btnMask shl 1 ;
    end ;
    FButtons := FJoyInfoEx.wButtons ;
  end ;

  if (FChangedAxis <> []) or (FChangedButtons <> []) or FFirstRead then
    NotifyJoystickChange ;

  NotifyJoystickPoll ;

  FFirstRead := false ;
end ;

procedure TepJoystick.NotifyJoystickPoll ;
begin
  if Assigned(FOnJoystickPoll) then
    FOnJoystickPoll(Self) ;
end ;

procedure TepJoystick.NotifyJoystickChange ;
begin
  if Assigned(FOnJoystickChange) then
    FOnJoystickChange(Self) ;
end ;

procedure TepJoystick.NotifyDeviceError ;
begin
  if RaiseErrors then
    raise TJoystickError.Create(FDeviceError) ;

  if Assigned(FOnDeviceError) then
    FOnDeviceError(Self, FDeviceError) ;
end ;

function TepJoystick.IsOK(errCode : Cardinal) : boolean ;
begin
  FDeviceError := errCode ;
  Result := FDeviceError = JOYERR_NOERROR ;

  if not Result then NotifyDeviceError ;
end ;

function TepJoystick.GetJoyCapsAsText : TStrings ;
begin
  Result := TStringList.Create ;
  with Result do begin
    Add(Format('Vendor/Product ids = 0x%x / 0x%x', [FJoyCaps.wMid, FJoyCaps.wPid])) ;
    Add(Format('Product name = %s', [FJoyCaps.szPname])) ;
    Add(Format('Max buttons = %d', [FJoyCaps.wMaxButtons])) ;
    Add(Format('Buttons in use = %d', [FJoyCaps.wNumButtons])) ;
    Add(Format('Max axes = %d', [FJoyCaps.wMaxAxes])) ;
    Add(Format('Axes in use = %d', [FJoyCaps.wNumAxes])) ;
    Add(Format('X min/max = %d / %d', [FJoyCaps.wXmin, FJoyCaps.wXmax])) ;
    Add(Format('Y min/max = %d / %d', [FJoyCaps.wYmin, FJoyCaps.wYmax])) ;
    Add(Format('Z min/max = %d / %d', [FJoyCaps.wZmin, FJoyCaps.wZmax])) ;
    Add(Format('R min/max = %d / %d', [FJoyCaps.wRmin, FJoyCaps.wRmax])) ;
    Add(Format('U min/max = %d / %d', [FJoyCaps.wUmin, FJoyCaps.wUmax])) ;
    Add(Format('V min/max = %d / %d', [FJoyCaps.wVmin, FJoyCaps.wVmax])) ;
    Add(Format('Period min/max = %d / %d', [FJoyCaps.wPeriodMin, FJoyCaps.wPeriodMax])) ;
    Add(Format('Capabilities = 0x%x', [FJoyCaps.wCaps])) ;
    Add(Format('Reg key = %s', [FJoyCaps.szRegKey])) ;
    Add(Format('OEM VxD = %s', [FJoyCaps.szOEMVxD])) ;
  end ;
end ;

function TepJoystick.GetJoyCaps : TJoyCaps ;
begin
  Result := FJoyCaps ;
end ;

procedure TepJoystick.SetAxisOptionsX(const Value : TAxisOptionSet) ;
begin
  FAxisOptions.X := Value ;
end ;

procedure TepJoystick.SetAxisOptionsR(const Value : TAxisOptionSet) ;
begin
  FAxisOptions.R := Value ;
end ;

procedure TepJoystick.SetAxisOptionsU(const Value : TAxisOptionSet) ;
begin
  FAxisOptions.U := Value ;
end ;

procedure TepJoystick.SetAxisOptionsV(const Value : TAxisOptionSet) ;
begin
  FAxisOptions.V := Value ;
end ;

procedure TepJoystick.SetAxisOptionsY(const Value : TAxisOptionSet) ;
begin
  FAxisOptions.Y := Value ;
end ;

procedure TepJoystick.SetAxisOptionsZ(const Value : TAxisOptionSet) ;
begin
  FAxisOptions.Z := Value ;
end ;

function TepJoystick.IsButtonPressed(btn : TJoystickButton) : boolean ;
begin
  Result := btn in FPressedButtons ;
end ;

{ TJoystickError }

constructor TJoystickError.Create(errCode : Integer ; const msg : string) ;
begin
  FErrorCode := errCode ;

  if msg <> '' then
    inherited Create(msg)
  else
    inherited Create(Format('Joystick error (Code = %d)', [FErrorCode])) ;

end ;

end.

