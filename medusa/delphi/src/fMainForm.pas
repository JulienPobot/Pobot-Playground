unit fMainForm ;

interface

uses
  Windows,
  Messages,
  SysUtils,
  Variants,
  Classes,
  Graphics,
  Controls,
  Forms,
  Dialogs,
  RzForms,
  RzCommon,
  RzStatus,
  ExtCtrls,
  StdCtrls,
  epConsole,
  AnalogMeter,
  OoMisc,
  AdPort,
  AdExcept,
  Mask,
  RzButton,
  AdStatLt,
  AdPacket,
  RzBorder,
  atbIndicator,
  ComCtrls,
  epJoystick,
  epJoystickViewer,
  epPowerIndicator,
  RaftData,
  Grids,
  VirtualTrees,
  VrCheckLed,
  VrControls,
  RzPanel,
  VrDeskTop,
  RzLabel,
  dmRaftController,
  PngImage1,
  VrProgressBar,
  VrArrow,
  VrSystem,
  VrDesign,
  RzEdit,
  RzSpnEdt,
  VrLights,
  InputDevMapping,
  Menus,
  ImgList ;

type

  TWinchDirection = (wdNone, wdUp, wdDown) ;
  TWinchState = (wsOff, wsRunningUp, wsRunningDown, wsLimitUp, wsLimitDown) ;

  TMainForm = class(TForm)
    epJoystick1 : TepJoystick ;
    logoPS : TImage ;
    imgLogo : TImage ;
    RzFormState : TRzFormState ;
    RzRegIniFile : TRzRegIniFile ;
    RzStatusBar : TRzStatusBar ;
    RzVersionInfo : TRzVersionInfo ;
    RzVersionInfoStatus : TRzVersionInfoStatus ;
    tmrClock : TTimer ;
    VrDeskTop1 : TVrDeskTop ;
    RzGroupBox2 : TRzGroupBox ;
    pnlPosition : TPanel ;
    RzGroupBox3 : TRzGroupBox ;
    pnlHeading : TPanel ;
    RzGroupBox4 : TRzGroupBox ;
    lbClock : TPanel ;
    RzGroupBox5 : TRzGroupBox ;
    vstSensors : TVirtualStringTree ;
    RzGroupBox6 : TRzGroupBox ;
    Panel2 : TPanel ;
    mtrTempLeft : TAnalogMeter ;
    RzGroupBox10 : TRzGroupBox ;
    epConsoleLocal : TepConsole ;
    RzPanel1 : TRzPanel ;
    btnClearConsoleLocal : TRzButton ;
    RzGroupBox11 : TRzGroupBox ;
    ledRxData : TApdStatusLight ;
    Label2 : TLabel ;
    Label3 : TLabel ;
    indComLost : TatbIndicator ;
    Bevel7 : TBevel ;
    spStatus : TRzStatusPane ;
    spMessage : TRzStatusPane ;
    dlgSelectUserConfig : TOpenDialog ;
    chkComActive : TVrCheckLed ;
    Label8 : TLabel ;
    Image1 : TImage ;
    Image2 : TImage ;
    Image3 : TImage ;
    Image4 : TImage ;
    Image5 : TImage ;
    tmrRxLed : TTimer ;
    Label1 : TLabel ;
    Label6 : TLabel ;
    pnlLinkStatus : TPanel ;
    grpCommands : TRzGroupBox ;
    lbRelay1 : TLabel ;
    lbRelay2 : TLabel ;
    lbRelay3 : TLabel ;
    lbRelay4 : TLabel ;
    lbRelay5 : TLabel ;
    lbRelay6 : TLabel ;
    lbRelay7 : TLabel ;
    lbRelay8 : TLabel ;
    Bevel11 : TBevel ;
    vrchkRelay1 : TVrCheckLed ;
    vrchkRelay2 : TVrCheckLed ;
    vrchkRelay3 : TVrCheckLed ;
    vrchkRelay4 : TVrCheckLed ;
    vrchkRelay5 : TVrCheckLed ;
    vrchkRelay6 : TVrCheckLed ;
    vrchkRelay7 : TVrCheckLed ;
    vrchkRelay8 : TVrCheckLed ;
    grpRelayCommands : TRzGroupBox ;
    grpMotorCommands : TRzGroupBox ;
    epJoystickViewer1 : TepJoystickViewer ;
    piMotorLeft : TepPowerIndicator ;
    piMotorRight : TepPowerIndicator ;
    RzLabel1 : TRzLabel ;
    RzLabel2 : TRzLabel ;
    grpWinch : TRzGroupBox ;
    piWinch : TepPowerIndicator ;
    lbWinchLen : TPanel ;
    Label16 : TLabel ;
    RzGroupBox8 : TRzGroupBox ;
    //    grpCommands : TRzGroupBox ;
    vrChkRecord : TVrCheckLed ;
    RzGroupBox1 : TRzGroupBox ;
    Panel5 : TPanel ;
    mtrBattElec : TAnalogMeter ;
    mtrBattMotors : TAnalogMeter ;
    mtrTempRight : TAnalogMeter ;
    VrBitmapList : TVrBitmapList ;
    Panel1 : TPanel ;
    vrimgTemp : TVrBitmapImage ;
    vrimgBattery : TVrBitmapImage ;
    Label4 : TLabel ;
    Label5 : TLabel ;
    Label7 : TLabel ;
    Label9 : TLabel ;
    edRecordPeriod : TRzSpinEdit ;
    Label10 : TLabel ;
    Label11 : TLabel ;
    btnManualRecord : TRzButton ;
    grpRecorder : TRzGroupBox ;
    Label12 : TLabel ;
    vrchkAutoRecord : TVrCheckLed ;
    pnlRecordCount : TPanel ;
    Bevel1 : TBevel ;
    Bevel2 : TBevel ;
    vrLinkQuality : TVrLights ;
    vrimgWinch : TVrBitmapImage ;
    RzLabel3 : TRzLabel ;
    Label13 : TLabel ;
    lbSpacer1 : TLabel ;
    Label14 : TLabel ;
    RzPanel2 : TRzPanel ;
    RzLabel4 : TRzLabel ;
    indJoystickOK : TatbIndicator ;
    indJoystickError : TatbIndicator ;
    epConsoleRemote : TepConsole ;
    btnClearConsoleRemote : TRzButton ;
    pmSensorsDisplay : TPopupMenu ;
    miDisplayRawValues : TMenuItem ;
    miDisplayBatteryValues : TMenuItem ;
    btnMap : TRzButton ;
    pmGPS : TPopupMenu ;
    miEnterCoordFix : TMenuItem ;
    chkWinchActivated : TVrCheckLed ;
    Label15 : TLabel ;
    btnComSetup : TRzToolButton ;
    ImageList : TImageList ;
    procedure btnClearConsoleLocalClick(Sender : TObject) ;
    procedure FormCloseQuery(Sender : TObject ; var CanClose : Boolean) ;
    procedure FormCreate(Sender : TObject) ;
    procedure FormShow(Sender : TObject) ;
    procedure epJoystick1JoystickChange(Sender : TepJoystick) ;
    procedure vstSensorsGetText(Sender : TBaseVirtualTree ;
      Node : PVirtualNode ; Column : TColumnIndex ; TextType : TVSTTextType ;
      var CellText : WideString) ;
    procedure vrChkRecordChange(Sender : TObject) ;
    procedure tmrClockTimer(Sender : TObject) ;
    procedure chkComActiveChange(Sender : TObject) ;
    procedure vrChkRelayXChange(Sender : TObject) ;
    procedure tmrRxLedTimer(Sender : TObject) ;
    procedure AboutClick(Sender : TObject) ;
    procedure btnManualRecordClick(Sender : TObject) ;
    procedure mtrBattChange(Sender : TObject) ;
    procedure mtrTempRightChange(Sender : TObject) ;
    procedure btnClearConsoleRemoteClick(Sender : TObject) ;
    procedure miDisplayBatteryValuesClick(Sender : TObject) ;
    procedure miDisplayRawValuesClick(Sender : TObject) ;
    procedure vstSensorsPaintText(Sender : TBaseVirtualTree ;
      const TargetCanvas : TCanvas ; Node : PVirtualNode ;
      Column : TColumnIndex ; TextType : TVSTTextType) ;
    procedure btnMapClick(Sender : TObject) ;
    procedure miEnterCoordFixClick(Sender : TObject) ;
    procedure btnComSetupClick(Sender : TObject) ;
  private
    FLastRecordTime : TDateTime ;
    FCurrentTime : TDateTime ;
    FDataLogDir : string ;
    FDataLogFile : TextFile ;
    FDisplayRawValues : Boolean ;
    FFirstFrame : Boolean ;
    FInputDevMapping : TInputDeviceMapping ;
    FManualAddRequest : Boolean ;
    FNoRFLinkShutdownWarning : Boolean ;
    FReady : Boolean ;
    FRecording : Boolean ;
    FRecordsCount : Integer ;
    FWinchCommandDir : TWinchDirection ;
    FWinchStoppedDir : TWinchDirection ;
    FConfigFilePath : string ;

    procedure SetMotorPower(whichOne : TMotorSelector ; power : Integer) ;
    procedure ControlPropulsionMotors(speed, bias : Integer) ;
    procedure SetupSensorsDisplay ;
    procedure SetupRelayCommands ;
    procedure DisplayStatus(const msg : string ; fatal : Boolean = false) ;
    procedure DisplayLinkQuality(missingFrames : Integer) ;

    procedure RaftControllerComOpen(Sender : TRaftController) ;
    procedure RaftControllerComClose(Sender : TRaftController) ;
    procedure RaftControllerDataReceived(Sender : TRaftController ; data : TRaftData) ;
    procedure RaftControllerMsgReceived(Sender : TRaftController ; msg : string) ;
    procedure DisplayMotorTemperature(motor : TMotorSelector ; degrees : Integer) ;
    procedure DisplayWinchedLength(lgCm : Integer) ;
    procedure DisplayBatteryLevels(lvlLogic, lvlPower : double) ;
    procedure DisplayOnlineStatus(status : Boolean) ;
    procedure RecordData(data : TRaftData) ;
    procedure StartRecording ;
    procedure StopRecording ;
    procedure EnableCommands(State : Boolean) ;
    procedure EnableRecorder(State : Boolean) ;
    procedure ResetMotorControlDisplay ;
    procedure SetIndicatorState(ind : TVrBitmapImage ; onoff : Boolean) ;
    procedure SetWinchStateIndicator(State : TWinchState) ;
    procedure UpdateWinchStateIndicator ;
    procedure ResetNumericalDisplays ;
    procedure ResetWinchControlDisplay ;
    procedure ResetRelayCommands ;
    procedure ResetCommandsAndDisplays ;
  end ;

var
  MainForm          : TMainForm ;

implementation

uses
  MMSystem,
  IniFiles,
  Registry,
  fAbout,
  fMapForm,
  DateUtils,
  Math,
  MedusaUtils,
  dlgSelComPort,
  dlgPositionFixEntry
  ;

const
  STX               = #$02 ;
  ETX               = #$03 ;

  CL_GREEN_OFF      = $00004000 ;

  IND_ON            = true ;
  IND_OFF           = false ;

  UNKNOWN_GPS_POSITION = '- - -' ;
  UNKNOWN_HEADING   = '---' ;
  UNKNOWN_WINCH_CABLE_LENGTH = '---' ;

  CONFIG_FILE_EXTENSION = '.mdscfg' ;
  CONFIG_FILE_DIR   = 'config\' ;

type
  EConfiguration = class(Exception) ;

{$R *.dfm}

procedure TMainForm.btnClearConsoleLocalClick(Sender : TObject) ;
begin
  epConsoleLocal.Clear ;
end ;

procedure TMainForm.btnClearConsoleRemoteClick(Sender : TObject) ;
begin
  epConsoleRemote.Clear ;
end ;

procedure TMainForm.epJoystick1JoystickChange(Sender : TepJoystick) ;
var
  i                 : Integer ;
  vrChkRelay        : TVrCheckLed ;
begin
  FInputDevMapping.CaptureInputs ;

  epJoystickViewer1.SetPosition(FInputDevMapping.Steering, FInputDevMapping.Throttle) ;

  if RaftController.Active then begin
    ControlPropulsionMotors(FInputDevMapping.Throttle, FInputDevMapping.Steering) ;

    if chkWinchActivated.Checked then begin
      if abs(FInputDevMapping.WinchPower) > 5 then begin
        SetMotorPower(motWinch, FInputDevMapping.WinchPower) ;
        if FInputDevMapping.WinchPower > 0 then
          FWinchCommandDir := wdUp
        else
          FWinchCommandDir := wdDown ;
      end
      else begin
        SetMotorPower(motWinch, 0) ;
        FWinchCommandDir := wdNone ;
      end ;
      UpdateWinchStateIndicator ;
    end ;

    for i := Low(TIONum) + 1 to High(TIONum) + 1 do
      if FInputDevMapping.RelayButtons[i] then begin
        vrChkRelay := TVrCheckLed(FindComponent('vrChkRelay' + IntToStr(i))) ;
        if Assigned(vrChkRelay) and vrChkRelay.Enabled then
          vrChkRelay.Checked := not vrChkRelay.Checked ;
      end ;
  end ;
end ;

procedure TMainForm.UpdateWinchStateIndicator ;
begin
  case FWinchCommandDir of
    wdNone : SetWinchStateIndicator(wsOff) ;
    wdUp : if FWinchStoppedDir = wdUp then
        SetWinchStateIndicator(wsLimitUp)
      else
        SetWinchStateIndicator(wsRunningUp) ;
    wdDown : if FWinchStoppedDir = wdDown then
        SetWinchStateIndicator(wsLimitDown)
      else
        SetWinchStateIndicator(wsRunningDown) ;
  end ;
end ;

procedure TMainForm.SetWinchStateIndicator(State : TWinchState) ;
begin
  vrimgWinch.BitmapIndex := vrimgWinch.Tag + Ord(State) ;
end ;

procedure TMainForm.ControlPropulsionMotors(speed, bias : Integer) ;
const
  SCALE             = 100 ;
begin
  SetMotorPower(motLeft, min(SCALE, max(-SCALE, speed + bias))) ;
  SetMotorPower(motRight, min(SCALE, max(-SCALE, speed - bias))) ;
end ;

procedure TMainForm.FormCloseQuery(Sender : TObject ; var CanClose : Boolean) ;
begin
  if RaftController.Active then begin
    MessageBeep(MB_ICONHAND) ;
    MessageDlg('Impossible de quitter l''application si la communication est active', mtError, [mbOk], 0) ;
    CanClose := false ;
  end
  else
    CanClose := true ;
end ;

procedure TMainForm.FormCreate(Sender : TObject) ;
begin
  FReady := false ;
  Self.Caption := Application.Title ;
  epConsoleLocal.Put(Application.Title + ' version ' + RzVersionInfo.FileVersion) ;

  FWinchCommandDir := wdNone ;
  FWinchStoppedDir := wdNone ;

  RaftController := TRaftController.Create(Self) ;
  RaftController.RaftData.GPSPosition.InvalidPositionString := UNKNOWN_GPS_POSITION ;
  RaftController.OnComOpen := RaftControllerComOpen ;
  RaftController.OnComClose := RaftControllerComClose ;
  RaftController.OnDataReceived := RaftControllerDataReceived ;
  RaftController.OnMsgReceived := RaftControllerMsgReceived ;

  SetMotorPower(motLeft, 0) ;
  SetMotorPower(motRight, 0) ;
  SetMotorPower(motWinch, 0) ;

  FFirstFrame := true ;
  ResetNumericalDisplays ;
end ;

procedure TMainForm.ResetNumericalDisplays ;
begin
  pnlPosition.Caption := UNKNOWN_GPS_POSITION ;
  pnlHeading.Caption := UNKNOWN_HEADING ;
  lbWinchLen.Caption := UNKNOWN_WINCH_CABLE_LENGTH ;
end ;

procedure TMainForm.DisplayStatus(const msg : string ; fatal : Boolean) ;
begin
  spStatus.Caption := msg ;
  if fatal then
    spStatus.Font.Color := clRed
  else
    spStatus.Font.Color := clNavy ;

  spStatus.Refresh ;
end ;

const
  SECT_INPUT_DEV    = 'input-device' ;
  VAR_DEV_TYPE      = 'device-type' ;
  VAR_DISPLAY_CAPS  = 'display-caps' ;

  SECT_DEBUG        = 'debug' ;
  VAR_USER_PARMS_WITH_EXE = 'user-parms-with-exe' ;
  VAR_NO_RFLINK_SHUTDOWN_WARNING = 'no-rflink-shutdown-warning' ;

  SECT_LAST_SESSION = 'last-session' ;
  VAR_USER_CONFIG_FILE = 'user-config-file' ;

procedure TMainForm.FormShow(Sender : TObject) ;
var
  cfgFile           : TIniFile ;
  inputDevType      : string ;
  usrParmPath       : string ;
  usrParmsDir       : string ;
  anomDetected      : Boolean ;

  procedure DisplayJoystickCaps ;
  var
    sl              : TStrings ;
    i               : Integer ;
  begin
    sl := epJoystick1.GetJoyCapsAsText ;
    try
      for i := 0 to sl.Count - 1 do
        epConsoleLocal.Put(' - ' + sl[i]) ;
    finally
      sl.Free ;
    end ;
  end ;

begin
  anomDetected := false ;
  Self.Refresh ;                        // cosmétique, pour que tout soit affiché avant la suite
  Screen.Cursor := crHourGlass ;
  try
    DisplayOnlineStatus(false) ;
    EnableCommands(false) ;
    EnableRecorder(false) ;

    epConsoleLocal.SkipLine ;
    epConsoleLocal.Put('Initialisation système...') ;
    DisplayStatus('Initialisation...') ;

    try
      FConfigFilePath := IncludeTrailingPathDelimiter(ExtractFileDir(Application.ExeName)) +
        IncludeTrailingPathDelimiter(CONFIG_FILE_DIR) +
        ChangeFileExt(ExtractFileName(Application.ExeName), CONFIG_FILE_EXTENSION) ;

      if not FileExists(FConfigFilePath) then
        raise EConfiguration.Create('Fichier de configuration non trouvé') ;

      cfgFile := TIniFile.Create(FConfigFilePath) ;
      try
        // configuration système du contrôleur du radeau

        epConsoleLocal.Put('- contrôleur radeau : ') ;
        RaftController.SetupSystem(cfgFile) ;
        mtrBattElec.LowZoneValue := RaftController.RaftData.BatteryCriticalLevel[batLogic] ;
        mtrBattMotors.LowZoneValue := RaftController.RaftData.BatteryCriticalLevel[batPower] ;

        epConsoleLocal.AppendToLastLine('OK') ;

        // configuration utilisateur des capteurs et actionneurs

        epConsoleLocal.Put('- configuration utilisateur : ') ;
        usrParmPath := RzRegIniFile.ReadString(SECT_LAST_SESSION, VAR_USER_CONFIG_FILE, '') ;
        if usrParmPath <> '' then
          usrParmsDir := ExtractFilePath(usrParmPath)
        else if cfgFile.ReadBool(SECT_DEBUG, VAR_USER_PARMS_WITH_EXE, false) then
          usrParmsDir := ExtractFilePath(Application.ExeName)
        else begin
          usrParmsDir := GetSpecialFolder('Personal') + '\Medusa1\' ;
          if not DirectoryExists(usrParmsDir) then
            MkDir(usrParmsDir) ;
        end ;
        FDataLogDir := usrParmsDir ;

        if usrParmPath <> '' then
          dlgSelectUserConfig.FileName := usrParmPath
        else
          dlgSelectUserConfig.InitialDir := usrParmsDir ;

        if dlgSelectUserConfig.Execute then begin
          Self.Refresh ;
          usrParmPath := dlgSelectUserConfig.FileName ;
          RaftController.SetupUserConfig(usrParmPath) ;
          SetupSensorsDisplay ;
          SetupRelayCommands ;
          edRecordPeriod.Value := RaftController.RecordingPeriod ;
          vrchkAutoRecord.Checked := RaftController.RecordingAuto ;

          btnMap.Enabled := TMapDisplay.Setup(usrParmPath) ;
          if btnMap.Enabled then btnMap.Show ;

          RzRegIniFile.WriteString(SECT_LAST_SESSION, VAR_USER_CONFIG_FILE, usrParmPath) ;
          epConsoleLocal.AppendToLastLine('OK') ;
        end
        else
          epConsoleLocal.Put('[ERR] Aucune configuration utilisateur chargée') ;

        // configuration de la manette ou joystick de commande

        inputDevType := cfgFile.ReadString(SECT_INPUT_DEV, VAR_DEV_TYPE, 'joystick') ;

        epConsoleLocal.Put('- manette de commande : ') ;
        epJoystick1.Enabled := true ;
        if not epJoystick1.Attached then begin
          epConsoleLocal.Put('[ERR] manette de commande non détectée') ;
          indJoystickError.State := isBlinking ;
          anomDetected := true ;
        end
        else begin
          indJoystickOK.State := isOn ;

          epConsoleLocal.AppendToLastLine(inputDevType) ;
          if cfgFile.ReadBool(SECT_INPUT_DEV, VAR_DISPLAY_CAPS, false) then
            DisplayJoystickCaps ;
          FInputDevMapping := TInputDeviceMapping.Create(epJoystick1) ;
          FInputDevMapping.LoadSettings(cfgFile, 'input-device-mapping.' + inputDevType) ;
        end ;

        FNoRFLinkShutdownWarning := cfgFile.ReadBool(SECT_DEBUG, VAR_NO_RFLINK_SHUTDOWN_WARNING, false) ;

        // c'est bon, nous sommes prêts

        FReady := true ;

      finally
        FreeAndNil(cfgFile) ;

        epConsoleLocal.SkipLine ;
        if not anomDetected then begin
          epConsoleLocal.Put('Système prêt.') ;
          DisplayStatus('Prêt') ;
        end
        else begin
          epConsoleLocal.Put('Anomalie configuration.') ;
          epConsoleLocal.Put('(consultez les messages précédents)') ;
          DisplayStatus('Anomalie configuration', true) ;
        end ;
        epConsoleLocal.SkipLine ;
      end ;

    except
      on e : Exception do begin
        epConsoleLocal.Put('[ERR] ' + e.Message) ;
      end ;
    end ;

  finally
    Screen.Cursor := crDefault ;
  end ;
end ;

procedure TMainForm.SetIndicatorState(ind : TVrBitmapImage ; onoff : Boolean) ;
begin
  if onoff then
    ind.BitmapIndex := ind.Tag + 1
  else
    ind.BitmapIndex := ind.Tag ;
end ;

procedure TMainForm.mtrBattChange(Sender : TObject) ;
var
  mtr               : TAnalogMeter ;
begin
  mtr := Sender as TAnalogMeter ;

  if RaftController.Active then begin
    if mtr.Value <= mtr.LowZoneValue then
      mtr.Font.Color := mtr.LowZoneColor
    else if mtr.Value >= mtr.HighZoneValue then
      mtr.Font.Color := mtr.HighZoneColor
    else
      mtr.Font.Color := mtr.OKZoneColor ;

    SetIndicatorState(vrimgBattery,
      (mtrBattElec.Value <= mtrBattElec.LowZoneValue) or (mtrBattMotors.Value <= mtrBattMotors.LowZoneValue)
      ) ;
  end

  else begin
    mtr.Font.Color := mtr.Color ;
    vrimgBattery.BitmapIndex := vrimgBattery.Tag ;
  end ;
end ;

procedure TMainForm.mtrTempRightChange(Sender : TObject) ;
var
  mtr               : TAnalogMeter ;
begin
  mtr := Sender as TAnalogMeter ;

  if RaftController.Active then begin
    if mtr.Value <= mtr.LowZoneValue then
      mtr.Font.Color := mtr.LowZoneColor
    else if mtr.Value >= mtr.HighZoneValue then
      mtr.Font.Color := mtr.HighZoneColor
    else
      mtr.Font.Color := mtr.OKZoneColor ;

    SetIndicatorState(vrimgTemp,
      (mtrTempLeft.Value >= mtrTempLeft.HighZoneValue) or (mtrTempRight.Value >= mtrTempRight.LowZoneValue)
      ) ;
  end

  else begin
    mtr.Font.Color := mtr.Color ;
    vrimgBattery.BitmapIndex := vrimgBattery.Tag ;
  end ;
end ;

procedure TMainForm.SetMotorPower(whichOne : TMotorSelector ; power : Integer) ;
begin
  case whichOne of
    motLeft : begin
        piMotorLeft.Value := power ;
      end ;
    motRight : begin
        piMotorRight.Value := power ;
      end ;
    motWinch : begin
        piWinch.Value := power ;
      end ;
  end ;

  RaftController.SetMotorPower(whichOne, power) ;
end ;

type

  TSensorType = (stRaft, stProbe, stBattery) ;

  TNodeData = record
    sensorNum : Cardinal ;
    sensorType : TSensorType ;
    sensor : TSensor ;
  end ;

procedure TMainForm.SetupSensorsDisplay ;
var
  pNode             : PVirtualNode ;
  nodeData          : ^TNodeData ;
  i                 : Integer ;
  bat               : TBattery ;
begin
  vstSensors.NodeDataSize := sizeof(TNodeData) ;

  for i := Low(TLocalSensorId) to High(TLocalSensorId) do
    if Assigned(RaftController.RaftData.LocalSensors[i]) then begin
      pNode := vstSensors.AddChild(nil) ;
      nodeData := vstSensors.GetNodeData(pNode) ;
      with nodeData^ do begin
        sensorNum := i ;
        sensorType := stRaft ;
        sensor := RaftController.RaftData.LocalSensors[i] ;
      end ;
    end ;

  for i := Low(TRemoteSensorId) to High(TRemoteSensorId) do
    if Assigned(RaftController.RaftData.RemoteSensors[i]) then begin
      pNode := vstSensors.AddChild(nil) ;
      nodeData := vstSensors.GetNodeData(pNode) ;
      with nodeData^ do begin
        sensorNum := i ;
        sensorType := stProbe ;
        sensor := RaftController.RaftData.RemoteSensors[i] ;
      end ;
    end ;

  for bat := Low(TBattery) to High(TBattery) do begin
    pNode := vstSensors.AddChild(nil) ;
    nodeData := vstSensors.GetNodeData(pNode) ;
    nodeData.sensorType := stBattery ;
    nodeData.sensor := RaftController.RaftData.BatterySensors[bat] ;
    // par défaut les capteurs des batteries ne sont pas visibles
    Exclude(pNode.States, vsVisible) ;
  end ;
end ;

procedure TMainForm.vstSensorsGetText(Sender : TBaseVirtualTree ;
  Node : PVirtualNode ; Column : TColumnIndex ; TextType : TVSTTextType ;
  var CellText : WideString) ;
var
  pNodeData         : ^TNodeData ;
begin
  pNodeData := Sender.GetNodeData(Node) ;
  case Column of
    0 : case pNodeData.sensorType of
        stRaft : CellText := 'R' + IntToStr(pNodeData.sensorNum) ;
        stProbe : CellText := 'S' + IntToStr(pNodeData.sensorNum) ;
        stBattery : CellText := '' ;
      end ;
    1 : CellText := pNodeData.sensor.Name ;
    2 : if FDisplayRawValues then
        CellText := Format('%4d', [pNodeData.sensor.RawValue])
      else
        CellText := Format('%.1f', [pNodeData.sensor.Value]) ;
    3 : if FDisplayRawValues then
        CellText := ''
      else
        CellText := pNodeData.sensor.Units ;
  end ;
end ;

procedure TMainForm.vstSensorsPaintText(Sender : TBaseVirtualTree ;
  const TargetCanvas : TCanvas ; Node : PVirtualNode ; Column : TColumnIndex ;
  TextType : TVSTTextType) ;
var
  pNodeData         : ^TNodeData ;
begin
  pNodeData := Sender.GetNodeData(Node) ;
  case pNodeData.sensorType of
    stRaft : TargetCanvas.Font.Color := clMoneyGreen ;
    stProbe : TargetCanvas.Font.Color := clSkyBlue ;
    stBattery : TargetCanvas.Font.Color := clOlive ;
  else
    TargetCanvas.Font.Color := clSilver ;
  end ;
end ;

procedure TMainForm.vrChkRecordChange(Sender : TObject) ;
begin
  FRecording := vrChkRecord.Checked ;

  if FRecording then begin
    StartRecording ;
    btnManualRecord.Enabled := true ;
  end
  else begin
    StopRecording ;
    btnManualRecord.Enabled := false ;
  end ;
end ;

procedure TMainForm.tmrClockTimer(Sender : TObject) ;
begin
  FCurrentTime := Now ;
  lbClock.Caption := FormatDateTime('dd/mm/yyyy - hh:nn:ss', FCurrentTime) ;

  if RaftController.Active then
    DisplayLinkQuality(RaftController.MissingRxFramesCount) ;
end ;

procedure TMainForm.DisplayLinkQuality(missingFrames : Integer) ;
begin
  if missingFrames <= 1 then
    vrLinkQuality.LedState := [lsGreen]
  else if missingFrames < 5 then
    vrLinkQuality.LedState := [lsYellow]
  else begin
    vrLinkQuality.LedState := [lsRed] ;
  end ;

  if missingFrames >= 10 then begin
    if indComLost.State <> isBlinking then begin
      MessageBeep(MB_ICONHAND) ;
      epConsoleLocal.Put('*** ATTENTION *** Perte de la communication avec le radeau') ;
      ResetCommandsAndDisplays ;
    end ;
    indComLost.State := isBlinking ;
  end
  else begin
    if indComLost.State <> isOff then begin
      MessageBeep(MB_ICONEXCLAMATION) ;
      epConsoleLocal.Put('>>> Communication rétablie') ;
    end ;
    indComLost.State := isOff ;
  end ;
end ;

procedure TMainForm.DisplayMotorTemperature(motor : TMotorSelector ; degrees : Integer) ;
var
  mtr               : TAnalogMeter ;
begin
  case motor of
    motLeft : begin
        mtr := mtrTempLeft ;
      end ;
    motRight : begin
        mtr := mtrTempRight ;
      end ;
  else
    exit ;
  end ;

  mtr.Font.Color := mtr.LowZoneColor ;
  mtr.Value := degrees ;
  if degrees > 0 then
    mtr.Caption := Format('%d °C', [degrees])
  else
    with mtr do begin
      Caption := '' ;
      Font.Color := clSilver ;
    end ;

  if FFirstFrame then begin
    if degrees > mtr.HighZoneValue then
      vrimgTemp.BitmapIndex := vrimgTemp.Tag + 1 ;
  end ;
end ;

procedure TMainForm.EnableCommands(State : Boolean) ;
  procedure ProcessContainer(cntnr : TWinControl) ;
  var
    i               : Integer ;
    ctrl            : TControl ;
  begin
    for i := 0 to cntnr.ControlCount - 1 do begin
      ctrl := cntnr.Controls[i] ;
      if ctrl is TVrCheckLed then begin
        with TVrCheckLed(ctrl) do
          if State then
            Enabled := Palette.Low <> clSilver
          else
            Enabled := false ;
      end
      else if ctrl is TWinControl then
        ProcessContainer(TWinControl(ctrl)) ;
    end ;
  end ;

begin
  ProcessContainer(grpCommands) ;
end ;

procedure TMainForm.EnableRecorder(State : Boolean) ;
begin
  with vrChkRecord do begin
    Enabled := State ;
    if Enabled then
      Palette.Low := clMaroon
    else
      Palette.Low := clSilver ;
  end ;
  if not State then
    btnManualRecord.Enabled := false ;
end ;

procedure TMainForm.RaftControllerComClose(Sender : TRaftController) ;
begin
  DisplayOnlineStatus(false) ;
  EnableCommands(false) ;
  EnableRecorder(false) ;
  btnComSetup.Enabled := true ;

  vrLinkQuality.LedState := [] ;

  if FRecording then
    vrChkRecord.Checked := false ;
  vrChkRecord.Enabled := false ;

  ResetCommandsAndDisplays ;

  indComLost.State := isOff ;

  epConsoleLocal.Put('----------- Communication désactivée') ;
end ;

procedure TMainForm.ResetCommandsAndDisplays() ;
begin
  DisplayMotorTemperature(motLeft, 0) ;
  DisplayMotorTemperature(motRight, 0) ;

  DisplayBatteryLevels(0, 0) ;

  ResetMotorControlDisplay ;
  ResetWinchControlDisplay ;
  ResetRelayCommands ;

  ResetNumericalDisplays ;
end ;

procedure TMainForm.RaftControllerComOpen(Sender : TRaftController) ;
begin
  DisplayOnlineStatus(true) ;
  FFirstFrame := true ;
  EnableCommands(true) ;
  EnableRecorder(RaftController.Configured) ;
  btnComSetup.Enabled := false ;

  epConsoleLocal.Put('----------- Communication activée') ;
end ;

procedure TMainForm.DisplayWinchedLength(lgCm : Integer) ;
var
  oldSep            : char ;
begin
  oldSep := DecimalSeparator ;
  DecimalSeparator := '.' ;
  try
    lbWinchLen.Caption := Format('%4.1f', [Round(lgCm / 10.0) / 10.0]) ;
  finally
    DecimalSeparator := oldSep ;
  end ;
end ;

procedure TMainForm.DisplayBatteryLevels(lvlLogic, lvlPower : double) ;
  procedure DisplayLevel(lvl : double ; meter : TAnalogMeter) ;
  begin
    with meter do begin
      Value := lvl ;
      if lvl > 0 then
        Caption := Format('%d%%', [Round(lvl)])
      else begin
        Caption := '' ;
        Font.Color := HighZoneColor ;
      end ;
      Refresh ;
    end ;
  end ;

begin
  DisplayLevel(lvlLogic, mtrBattElec) ;
  DisplayLevel(lvlPower, mtrBattMotors) ;

  if FFirstFrame then begin
    if ((lvlLogic > 0) and (lvlLogic < mtrBattElec.LowZoneValue)) or
      ((lvlPower > 0) and (lvlPower < mtrBattMotors.LowZoneValue)) then
      vrimgBattery.BitmapIndex := vrimgBattery.Tag + 1 ;
  end ;
end ;

procedure TMainForm.RaftControllerDataReceived(Sender : TRaftController ;
  data : TRaftData) ;
const
  hwStatus          : array[Boolean] of string = ('--', 'OK') ;
begin
  ledRxData.Lit := true ;
  tmrRxLed.Enabled := true ;

  if FFirstFrame then begin
    epConsoleLocal.Put('Systèmes embarqués :') ;
    epConsoleLocal.Put('- GPS .................. ' + hwStatus[hfGPS in data.HardwareFlags]) ;
    epConsoleLocal.Put('- compas ............... ' + hwStatus[hfCompass in data.HardwareFlags]) ;
    epConsoleLocal.Put('- sonde immergée ....... ' + hwStatus[hfRemoteSensors in data.HardwareFlags]) ;
  end ;

  DisplayBatteryLevels(data.BatteryVoltage[batLogic], data.BatteryVoltage[batPower]) ;

  DisplayWinchedLength(data.WinchedLength) ;
  if sfWinchUp in data.StatusFlags then
    FWinchStoppedDir := wdUp
  else if sfWinchDown in data.StatusFlags then
    FWinchStoppedDir := wdDown
  else
    FWinchStoppedDir := wdNone ;
  UpdateWinchStateIndicator ;

  if hfTempMotor1 in data.HardwareFlags then
    DisplayMotorTemperature(motRight, data.motorTemp[motRight]) ;
  if hfTempMotor2 in data.HardwareFlags then
    DisplayMotorTemperature(motLeft, data.motorTemp[motLeft]) ;

  if hfGPS in data.HardwareFlags then begin
    pnlPosition.Caption := data.GPSPosition.AsString ;
    miEnterCoordFix.Enabled := data.GPSPosition.IsValid ;
  end ;

  if hfCompass in data.HardwareFlags then
    pnlHeading.Caption := IntToStr(data.Heading) ;

  vstSensors.Refresh ;
  if FRecording then begin
    if vrchkAutoRecord.Checked and (SecondsBetween(FLastRecordTime, FCurrentTime) >= edRecordPeriod.Value) then begin
      FManualAddRequest := true ;
      FLastRecordTime := FCurrentTime ;
    end ;
    if FManualAddRequest then begin
      RecordData(data) ;

      pnlRecordCount.Caption := IntToStr(FRecordsCount) ;
      FManualAddRequest := false ;
    end ;
  end ;

  FFirstFrame := false ;
end ;

procedure TMainForm.RaftControllerMsgReceived(Sender : TRaftController ;
  msg : string) ;
begin
  epConsoleRemote.Put(msg) ;
end ;

procedure TMainForm.tmrRxLedTimer(Sender : TObject) ;
begin
  ledRxData.Lit := false ;
  tmrRxLed.Enabled := false ;
end ;

procedure TMainForm.chkComActiveChange(Sender : TObject) ;
begin
  if not chkComActive.Checked then begin
    if RaftController.Active then begin
      if not FNoRFLinkShutdownWarning then begin
        MessageBeep(MB_ICONEXCLAMATION) ;
        if MessageDlg('Confirmez-vous la coupure de la communication avec le radeau ?', mtWarning, [mbYes, mbNo], 0)
          = mrYes then
          RaftController.Active := false
        else
          chkComActive.Checked := true
      end
      else
        RaftController.Active := false
    end
  end

  else try
    RaftController.Active := true ;
  except
    on e : EOpenComm do begin
      MessageBeep(MB_ICONHAND) ;
      MessageDlg('Erreur ouverture communication : ' + e.Message, mtError, [mbOk], 0) ;
      chkComActive.Checked := false ;
    end ;
  end ;
end ;

procedure TMainForm.DisplayOnlineStatus(status : Boolean) ;
begin
  with pnlLinkStatus do begin
    if status then begin
      Caption := 'En ligne' ;
      Font.Color := clLime ;
    end
    else begin
      Caption := 'Non connecté' ;
      Font.Color := clRed ;
      vrLinkQuality.LedState := [] ;
    end ;
  end ;
end ;

procedure TMainForm.vrChkRelayXChange(Sender : TObject) ;
begin
  with Sender as TVrCheckLed do
    RaftController.SetIO(Tag, Ord(Checked)) ;
end ;

procedure TMainForm.SetupRelayCommands ;
var
  i                 : Integer ;
  vrChkRelay        : TVrCheckLed ;
  lbRelay           : TLabel ;
begin
  for i := Low(TIONum) to High(TIONum) do begin
    vrChkRelay := TVrCheckLed(FindComponent('vrChkRelay' + IntToStr(i + 1))) ;
    lbRelay := TLabel(FindComponent('lbRelay' + IntToStr(i + 1))) ;

    if RaftController.OutputNames[i] <> '' then begin
      //vrChkRelay.Enabled := true ;
      vrChkRelay.Palette.Low := clGreen ;
      lbRelay.Caption := RaftController.OutputNames[i] ;
      lbRelay.Enabled := true ;
    end
    else begin
      vrChkRelay.Enabled := false ;
      vrChkRelay.Palette.Low := clSilver ;
      lbRelay.Caption := 'non configuré' ;
      lbRelay.Enabled := false ;
    end ;
  end ;
end ;

procedure TMainForm.ResetRelayCommands ;
var
  i                 : Integer ;
  vrChkRelay        : TVrCheckLed ;
begin
  for i := Low(TIONum) to High(TIONum) do begin
    vrChkRelay := TVrCheckLed(FindComponent('vrChkRelay' + IntToStr(i + 1))) ;
    vrChkRelay.Checked := false ;
  end ;
end ;

const
  FIELD_SEP         = #$09 ;

procedure TMainForm.StartRecording ;
var
  FileName          : string ;
  i                 : Integer ;
begin
  FileName := IncludeTrailingPathDelimiter(FDataLogDir) + FormatDateTime('yymmdd-hhnnss', Now) + '.txt' ;

  AssignFile(FDataLogFile, FileName) ;
  Rewrite(FDataLogFile) ;

  Write(FDataLogFile, 'Temps', FIELD_SEP, 'Position', FIELD_SEP, 'Cap') ;
  with RaftController.RaftData do begin
    for i := Low(TLocalSensorId) to High(TLocalSensorId) do
      if Assigned(LocalSensors[i]) then
        Write(FDataLogFile, FIELD_SEP, LocalSensors[i].Name) ;
    if HasRemoteSensors then begin
      Write(FDataLogFile, FIELD_SEP, 'Profondeur') ;
      for i := Low(TRemoteSensorId) to High(TRemoteSensorId) do
        if Assigned(RemoteSensors[i]) then
          Write(FDataLogFile, FIELD_SEP, RemoteSensors[i].Name) ;
    end ;

  end ;
  WriteLn(FDataLogFile) ;

  Write(FDataLogFile, FIELD_SEP, FIELD_SEP) ;
  with RaftController.RaftData do begin
    for i := Low(TLocalSensorId) to High(TLocalSensorId) do
      if Assigned(LocalSensors[i]) then
        Write(FDataLogFile, FIELD_SEP, LocalSensors[i].Units) ;
    if HasRemoteSensors then begin
      Write(FDataLogFile, FIELD_SEP, 'm') ;
      for i := Low(TRemoteSensorId) to High(TRemoteSensorId) do
        if Assigned(RemoteSensors[i]) then
          Write(FDataLogFile, FIELD_SEP, RemoteSensors[i].Units) ;
    end ;

  end ;
  WriteLn(FDataLogFile) ;

  FRecordsCount := 0 ;

  epConsoleLocal.Put('>>> Enregistrement des mesures activé.') ;
  epConsoleLocal.Put(' - fichier : ' + FileName) ;
end ;

procedure TMainForm.StopRecording ;
begin
  CloseFile(FDataLogFile) ;
  epConsoleLocal.Put('>>> Enregistrement des mesures désactivé') ;
end ;

procedure TMainForm.RecordData(data : TRaftData) ;
var
  i                 : Integer ;
begin
  Write(FDataLogFile,
    FormatDateTime('dd/mm/yyyy hh:nn:ss', FCurrentTime), FIELD_SEP,
    pnlPosition.Caption, FIELD_SEP,
    pnlHeading.Caption
    ) ;

  for i := Low(TLocalSensorId) to High(TLocalSensorId) do
    if Assigned(data.LocalSensors[i]) then
      Write(FDataLogFile, FIELD_SEP, Format('%10.4f', [data.LocalSensors[i].Value])) ;

  if data.HasRemoteSensors then begin
    Write(FDataLogFile, FIELD_SEP, Format('%5.2f', [data.WinchedLength / 100.0])) ;
    for i := Low(TRemoteSensorId) to High(TRemoteSensorId) do
      if Assigned(data.RemoteSensors[i]) then
        Write(FDataLogFile, FIELD_SEP, Format('%10.4f', [data.RemoteSensors[i].Value])) ;
  end ;

  WriteLn(FDataLogFile) ;
  Flush(FDataLogFile) ;

  Inc(FRecordsCount) ;
end ;

procedure TMainForm.AboutClick(Sender : TObject) ;
begin
  TAbout.Display ;
end ;

procedure TMainForm.btnManualRecordClick(Sender : TObject) ;
begin
  FManualAddRequest := true ;
end ;

procedure TMainForm.ResetMotorControlDisplay ;
begin
  piMotorLeft.Value := 0 ;
  piMotorRight.Value := 0 ;
end ;

procedure TMainForm.ResetWinchControlDisplay ;
begin
  piWinch.Value := 0 ;
  SetWinchStateIndicator(wsOff) ;
end ;

procedure TMainForm.miDisplayBatteryValuesClick(Sender : TObject) ;
var
  pNode             : PVirtualNode ;
  pNodeData         : ^TNodeData ;
  displayBatSensors : Boolean ;
begin
  displayBatSensors := miDisplayBatteryValues.Checked ;
  pNode := vstSensors.GetFirst ;
  while (pNode <> nil) do begin
    pNodeData := vstSensors.GetNodeData(pNode) ;
    with pNodeData^ do
      if sensor is TBatterySensor then
        if displayBatSensors then
          Include(pNode.States, vsVisible)
        else
          Exclude(pNode.States, vsVisible) ;

    pNode := pNode.NextSibling ;
  end ;
  vstSensors.Refresh ;
end ;

procedure TMainForm.miDisplayRawValuesClick(Sender : TObject) ;
const
  colHdrValue       : array[Boolean] of string = ('mesure', 'valeur brute') ;
  colHdrUnit        : array[Boolean] of string = ('unités', '') ;
begin
  FDisplayRawValues := miDisplayRawValues.Checked ;
  vstSensors.Header.Columns[2].Text := colHdrValue[FDisplayRawValues] ;
  vstSensors.Header.Columns[3].Text := colHdrUnit[FDisplayRawValues] ;
  vstSensors.Refresh ;
end ;

procedure TMainForm.btnMapClick(Sender : TObject) ;
begin
  TMapDisplay.ShowWindow ;
end ;

procedure TMainForm.miEnterCoordFixClick(Sender : TObject) ;
var
  geoPos            : TGeoPosition ;
begin
  geoPos.lon := RaftController.RaftData.GPSPosition.Longitude ;
  geoPos.lat := RaftController.RaftData.GPSPosition.Latitude ;
  if TPositionFixEntryDialog.GetRealPosition(geoPos) then begin
    with RaftController.RaftData.GPSPosition do begin
      LongitudeFix := DMSToSecs(geoPos.lon) - DMSToSecs(Longitude) ;
      LatitudeFix := DMSToSecs(geoPos.lat) - DMSToSecs(Latitude) ;
    end ;
  end ;
end ;

procedure TMainForm.btnComSetupClick(Sender : TObject) ;
begin
  with TComPortSelector.Create(nil) do try
    PreSelectPort(RaftController.ComNumber) ;
    if (ShowModal = mrOk) then begin
      RaftController.ComNumber := SelectedComNum ;
      RaftController.UpdateConfigurationFile(FConfigFilePath) ;
    end ;

  finally
    Free ;
  end ;
end ;

end.

