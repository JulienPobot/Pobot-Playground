unit dmRaftController ;

interface

uses
  SysUtils,
  Classes,
  ExtCtrls,
  AdPacket,
  OoMisc,
  AdPort,
  RaftData,
  IniFiles,
  MissionDocument ;

type

  (*
   * Bloc de commande envoyé par le poste de contrôle
   *
   * Le bloc de commande est de taille fixe et comprend systématiquement les consignes pour
   * les 3 ESC (moteurs de propulsion et moteur du treuil), ainsi que l'état des 8 IOs.
   * Ce bloc est encadré par STX et ETX afin de constituer une trame complète. L'encodage
   * des données contenues dans le bloc est défini de manière à ce qu'aucun des bytes de donnée
   * ne puisse être confondu avec les caractères de délimitation de la trame.
   *
   * Les commandes proprement dites sont précédées par l'indicateur de réponse attendue (cf définition
   * du type TExpectedReply)
   *
   * Une trame complète est définie sur 11 bytes, dont 2 pour le protocole. Elle est structurée
   * comme suit :
   * <STX>
   * reply
   * ESC0_dir ESC0_speed
   * ESC1_dir ESC1_speed
   * ESC2_dir ESC2_speed
   * IO7..4   IO3..0
   * <ETX>
   *
   * avec :
   * reply = type de réponse attendue (' '=aucune, 'X'=extended reply)
   * ESCx_dir = direction moteur x ('+'=avant, '-'=arrière, '0'=stop)
   * ESCx_speed = vitesse moteur x ('0'..'9','A'..'F'), ignorée si dir = '0'
   * IO7..4 = représentation en hexa ASCII du nibble de poids fort de l'état des sorties ('0'..'F')
   * IO3..0 = idem pour le nibble de poids faible
  *)

  TWinchDirection = (wdStop, wdUp, wdDown) ;
  TMotorDirectionCode = (
    dirStop = Ord('0'),
    dirFwd = Ord('+'),
    dirRev = Ord('-'),
    dirUp = dirFwd,
    dirDown = dirRev
    ) ;
  TMotorSpeedLevel = 0..15 ;

  TMotorCommand = record
    direction : TMotorDirectionCode ;
    power : char ;
  end ;

  TIONum = 0..7 ;
  TIOState = 0..1 ;

  TExpectedReply = (
    repExtended = Ord('X'),
    repNone = Ord(' ')
    ) ;

  TCommandBlock = record
    expectedReply : TExpectedReply ;
    motorCommand : array[TMotorSelector] of TMotorCommand ;
    IO47 : char ;
    IO03 : char ;
  end ;

  TCommandFrame = record
    header : char ;
    payload : TCommandBlock ;
    trailer : char ;
  end ;

  TRaftController = class ;

  TComOpenEvent = procedure(Sender : TRaftController) of object ;
  TComCloseEvent = procedure(Sender : TRaftController) of object ;
  TDataReceivedEvent = procedure(Sender : TRaftController ; data : TRaftData) of object ;
  TMsgReceivedEvent = procedure(Sender : TRaftController ; msg : string) of object ;
  TFrameReceivedEvent = procedure(Sender : TRaftController) of object ;

  TRaftController = class(TDataModule)
    ApdComPort : TApdComPort ;
    tmrCom : TTimer ;
    dpXFrame : TApdDataPacket ;
    dpMFrame : TApdDataPacket ;
    procedure DataModuleCreate(Sender : TObject) ;
    procedure DataModuleDestroy(Sender : TObject) ;
    procedure ApdComPortPortOpen(Sender : TObject) ;
    procedure ApdComPortPortClose(Sender : TObject) ;
    procedure tmrComTimer(Sender : TObject) ;
    procedure dpXFramePacket(Sender : TObject ; data : Pointer ;
      Size : Integer) ;
    procedure dpMFramePacket(Sender : TObject ; data : Pointer ;
      Size : Integer) ;
  private
    FRxFrameCounter : Integer ;
    FTxFrameCounter : Integer ;
    FCommandBlock : TCommandBlock ;
    FConfigured : Boolean ;
    FIObin : array[0..1] of byte ;
    FMissingRxFramesCount : Integer ;
    FMissionDocument : TMissionDocument ;
    FOnComClose : TComCloseEvent ;
    FRaftData : TRaftData ;
    FOnComOpen : TComOpenEvent ;
    FOnDataReceived : TDataReceivedEvent ;
    FOnMsgReceived : TMsgReceivedEvent ;
    FOnFrameReceived : TFrameReceivedEvent ;
    FRxData : TExtendedData ;
    FOutputNames : array[TIONum] of string ;
    FRecordingAuto : Boolean ;
    FRecordingPeriod : Integer ;
    FRFLinkShutdownRequested : Boolean ;
    function GetActive : Boolean ;
    procedure SetActive(const Value : Boolean) ;
    procedure SendCommandFrame ;
    function GetBaud : TBaudRate ;
    procedure SetBaud(const Value : TBaudRate) ;
    function GetComNumber : TBaudRate ;
    function GetOutputNames(ioNum : TIONum) : string ;
    procedure SetComNumber(const Value : TBaudRate) ;
  public
    { Déclarations publiques }
    property Active : Boolean read GetActive write SetActive ;
    property Baud : TBaudRate read GetBaud write SetBaud ;
    property ComNumber : TBaudRate read GetComNumber write SetComNumber ;
    property Configured : Boolean read FConfigured ;
    property MissingRxFramesCount : Integer read FMissingRxFramesCount ;
    property RaftData : TRaftData read FRaftData ;

    property OnComClose : TComCloseEvent read FOnComClose write FOnComClose ;
    property OnComOpen : TComOpenEvent read FOnComOpen write FOnComOpen ;
    property OnDataReceived : TDataReceivedEvent read FOnDataReceived write FOnDataReceived ;
    property OnMsgReceived : TMsgReceivedEvent read FOnMsgReceived write FOnMsgReceived ;
    property OnFrameReceived : TFrameReceivedEvent read FOnFrameReceived write FOnFrameReceived ;
    property OutputNames[ioNum : TIONum] : string read GetOutputNames ;
    property RecordingAuto : Boolean read FRecordingAuto ;
    property RecordingPeriod : Integer read FRecordingPeriod ;

    procedure ProcessXFrame(data : Pointer ; Size : Integer) ;
    procedure SetMotorPower(whichOne : TMotorSelector ; power : Integer) ;
    procedure SetIO(num : TIONum ; Value : TIOState) ;
    procedure SetupSystem(const filePath : string) ; overload ;
    procedure SetupSystem(cfgFile : TIniFile) ; overload ;
    procedure SetupUserConfig(const filePath : string) ; overload ;
    procedure UpdateConfigurationFile(const filePath : string);
  end ;

  RaftControllerError = class(Exception) ;
  RCEFileNotFound = class(RaftControllerError) ;
  RCEInternal = class(RaftControllerError) ;
  RCEInvalidState = class(RaftControllerError) ;
  RCEConfiguration = class(RaftControllerError) ;

var
  RaftController    : TRaftController ;

implementation

{$R *.dfm}

uses
  MedusaUtils
  ;

const
  HexDigits         : array[0..15] of char
                    = ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F') ;
  STX               = #02 ;
  ETX               = #03 ;

procedure TRaftController.dpXFramePacket(Sender : TObject ; data : Pointer ;
  Size : Integer) ;
begin
  Inc(FRxFrameCounter) ;
  FMissingRxFramesCount := 0 ;
  ProcessXFrame(data, Size) ;

  if Assigned(FOnDataReceived) then
    FOnDataReceived(Self, FRaftData) ;
end ;

procedure TRaftController.dpMFramePacket(Sender : TObject ; data : Pointer ;
  Size : Integer) ;
var
  len               : byte ;
  sPtr              : PChar ;
  msg               : string[50] ;
begin
  if Assigned(FOnMsgReceived) then begin
    len := PByte(data)^ ;
    sPtr := PChar(data) + 1 ;
    msg := Copy(sPtr, 0, len - 1) ;
    FOnMsgReceived(Self, msg) ;
  end ;
end ;

procedure TRaftController.DataModuleCreate(Sender : TObject) ;
var
  i                 : Integer ;
begin
  FRaftData := TRaftData.Create ;

  for i := Low(FOutputNames) to High(FOutputNames) do
    FOutputNames[i] := '' ;

  FCommandBlock.IO47 := '0' ;
  FCommandBlock.IO03 := '0' ;

  dpMFrame.EndString := #0 ;
end ;

procedure TRaftController.DataModuleDestroy(Sender : TObject) ;
begin
  Active := False ;
  FRaftData.Free ;
end ;

procedure TRaftController.ApdComPortPortOpen(Sender : TObject) ;
begin
  tmrCom.Enabled := True ;
  FMissingRxFramesCount := 0 ;
  FTxFrameCounter := 0 ;
  FRxFrameCounter := 0 ;
  FCommandBlock.expectedReply := repNone ;

  FRaftData.ResetBatteryAlarms ;

  if Assigned(FOnComOpen) then FOnComOpen(Self) ;
end ;

procedure TRaftController.ApdComPortPortClose(Sender : TObject) ;
begin
  tmrCom.Enabled := False ;

  if Assigned(FOnComClose) then FOnComClose(Self) ;
end ;

procedure TRaftController.tmrComTimer(Sender : TObject) ;
begin
  SendCommandFrame ;
end ;

function TRaftController.GetActive : Boolean ;
begin
  result := ApdComPort.Open ;
end ;

procedure TRaftController.SetActive(const Value : Boolean) ;
begin
  if Value then
    ApdComPort.Open := True

  else
    {
      Avant de couper la communication, il faut stopper les moteurs et actionneurs par sécurité. C'est
      normalement pris en charge par le programme embarqué dès qu'il ne reçoit plus de trames de contrôle,
      mais on n'est jamais trop prudent.
      On ne fait donc pas la coupure des communications ici, mais on poste une requête pour que la trame
      suivante qui va être envoiyée contienne les ordres correspondants. La communication ne sera réellement
      coupée qu'une fois cette trame envoyée au radeau.
    }

    {
      Signale que nous sommes dans une phase de coupure, de manière à ce que la prochaine trame émise soit
      en configuration d'arrêt complet, au lieu d'être la trame normale.
    }
    FRFLinkShutdownRequested := True ;
end ;

procedure TRaftController.SendCommandFrame ;
const
  REPLY_PERIOD      = 10 ;
var
  frame             : TCommandFrame ;
  motor             : TMotorSelector ;
  io                : TIONum ;
begin
  if ApdComPort.Open then begin
    // on vérifie d'abord qu'on a bien reçu la dernière réponse attendue
    if (FCommandBlock.expectedReply <> repNone) and (FRxFrameCounter = 0) then
      Inc(FMissingRxFramesCount) ;

    {
      Remarque : si nous faisions du vrai multi-tâches, nous aurions besoin de placer le code qui
      suit en section critique, afin de "geler" la trame à émettre tant que l'émission n'est pas
      faite.
      Mais nous sommes ici dans un processus event-driven, et donc aucun code susceptible de
      modifier la trame ne peut être appelé.
    }

    if FTxFrameCounter mod REPLY_PERIOD = 0 then begin
      FCommandBlock.expectedReply := repExtended ;
      FRxFrameCounter := 0 ;
    end
    else begin
      FCommandBlock.expectedReply := repNone ;
    end ;

    frame.header := STX ;
    frame.trailer := ETX ;

    {
      Si nous avons une requête d'arrêt avant coupure de la liaison, on remplace la trame
      en attente par une configuration d'arrêt des moteurs et des actionnerus.
    }
    if FRFLinkShutdownRequested then begin
      for motor := Low(TMotorSelector) to High(TMotorSelector) do
        SetMotorPower(motor, 0) ;
      for io := Low(TIONum) to High(TIONum) do
        SetIO(io, 0) ;
    end ;

    frame.payload := FCommandBlock ;

    ApdComPort.PutBlock(frame, sizeof(frame)) ;
    ApdComPort.FlushOutBuffer ;
    Inc(FTxFrameCounter) ;

    // Si une requête d'arrêt est en cours, on coupe la communication et on reset l'indicateur
    if FRFLinkShutdownRequested then begin
      ApdComPort.Open := False ;
      FRFLinkShutdownRequested := False ;
    end ;
  end ;
end ;

function TRaftController.GetBaud : TBaudRate ;
begin
  result := ApdComPort.Baud ;
end ;

procedure TRaftController.SetBaud(const Value : TBaudRate) ;
begin
  ApdComPort.Baud := Value ;
end ;

function TRaftController.GetComNumber : TBaudRate ;
begin
  result := ApdComPort.ComNumber ;
end ;

function TRaftController.GetOutputNames(ioNum : TIONum) : string ;
begin
  result := FOutputNames[ioNum] ;
end ;

procedure TRaftController.SetComNumber(const Value : TBaudRate) ;
begin
  ApdComPort.ComNumber := Value ;
end ;

procedure TRaftController.ProcessXFrame(data : Pointer ; Size : Integer) ;
begin
  if Size > sizeof(FRxData) then begin
    SetActive(False) ;
    raise RCEInternal.Create('Dépassement de taille des données reçues') ;
  end ;

  Move(data^, FRxData, Size) ;
  FRaftData.ProcessExtendedFrame(FRxData) ;
end ;

procedure TRaftController.SetMotorPower(whichOne : TMotorSelector ; power : Integer) ;
const
  POWER_ABS_MAX     = 100 ;
var
  powerLevel        : Integer ;
begin
  if power > POWER_ABS_MAX then
    power := POWER_ABS_MAX
  else if power < -POWER_ABS_MAX then
    power := -POWER_ABS_MAX ;

  powerLevel := Round(power * 15 / 100) ;
  if powerLevel > 0 then
    FCommandBlock.motorCommand[whichOne].direction := dirFwd
  else if powerLevel < 0 then
    FCommandBlock.motorCommand[whichOne].direction := dirRev
  else
    FCommandBlock.motorCommand[whichOne].direction := dirStop ;
  FCommandBlock.motorCommand[whichOne].power := HexDigits[abs(powerLevel)] ;
end ;

procedure TRaftController.SetIO(num : TIONum ; Value : TIOState) ;
var
  bVal              : byte ;
begin
  if num in [0..3] then begin
    bVal := FIObin[0] ;
    if Value = 0 then
      bVal := (bVal and not (1 shl num)) and $0F
    else
      bVal := (bVal or (1 shl num)) and $0F ;
    FIObin[0] := bVal ;
    FCommandBlock.IO03 := HexDigits[bVal]
  end

  else begin
    bVal := FIObin[1] ;
    num := num - 4 ;
    if Value = 0 then
      bVal := (bVal and not (1 shl num)) and $0F
    else
      bVal := (bVal or (1 shl num)) and $0F ;
    FIObin[1] := bVal ;
    FCommandBlock.IO47 := HexDigits[bVal] ;
  end ;
end ;

procedure TRaftController.SetupSystem(const filePath : string) ;
var
  cfgFile           : TIniFile ;
begin
  if not FileExists(filePath) then
    raise RCEFileNotFound.Create(filePath) ;

  cfgFile := TIniFile.Create(filePath) ;
  try
    SetupSystem(cfgFile) ;
  finally
    cfgFile.Free ;
  end ;
end ;

const
  SECT_RF_LINK      = 'rf-link' ;
  VAR_COMPORT       = 'comport' ;
  VAR_BAUD          = 'baud' ;
  VAR_COM_TRACE     = 'trace' ;

  SECT_BATT_CONVERSIONS = 'battery.ADC-conversions' ;
  SECT_BATT_CRITICAL_LEVELS = 'battery.critical-levels' ;
  VAR_LOGIC         = 'logic-coeff.%d' ;
  VAR_POWER         = 'power-coeff.%d' ;
  BATT_POLYNOM_ORDER = 3 ;

  SECT_WINCH        = 'winch' ;
  VAR_CM_PER_TICK   = 'cm-per-tick' ;

procedure TRaftController.SetupSystem(cfgFile : TIniFile) ;
var
  saveDecSep        : char ;
  polynom           : TPolynom ;
begin
  if ApdComPort.Open then raise RCEInvalidState.Create('Configuration impossible si communications actives') ;
  assert(Assigned(cfgFile), 'TRaftController.Configure called with nil argument') ;

  ApdComPort.ComNumber := cfgFile.ReadInteger(SECT_RF_LINK, VAR_COMPORT, ApdComPort.ComNumber) ;
  ApdComPort.Baud := cfgFile.ReadInteger(SECT_RF_LINK, VAR_BAUD, ApdComPort.Baud) ;

  saveDecSep := DecimalSeparator ;
  DecimalSeparator := '.' ;
  try
    polynom := TPolynom.Create([-610, 3.32, 0, 0]) ;
    polynom.ReadCoeffsFromIni(cfgFile, SECT_BATT_CONVERSIONS, VAR_LOGIC) ;
    FRaftData.BatterySensors[batLogic].ConversionPolynom := polynom ;

    polynom := TPolynom.Create([-105.92, 1.56, 0, 0]) ;
    polynom.ReadCoeffsFromIni(cfgFile, SECT_BATT_CONVERSIONS, VAR_POWER) ;
    FRaftData.BatterySensors[batPower].ConversionPolynom := polynom ;

    FRaftData.BatteryCriticalLevel[batLogic] := cfgFile.ReadFloat(SECT_BATT_CRITICAL_LEVELS, VAR_LOGIC, 55) ;
    FRaftData.BatteryCriticalLevel[batPower] := cfgFile.ReadFloat(SECT_BATT_CRITICAL_LEVELS, VAR_POWER, 55) ;
    FRaftData.WinchCmPerTick := cfgFile.ReadFloat(SECT_WINCH, VAR_CM_PER_TICK, 0) ;

  finally
    DecimalSeparator := saveDecSep ;
  end ;

  if cfgFile.ReadBool(SECT_RF_LINK, VAR_COM_TRACE, False) then
    ApdComPort.Tracing := tlOn
  else
    ApdComPort.Tracing := tlOff ;
end ;

procedure TRaftController.UpdateConfigurationFile(const filePath : string);
begin
  with TIniFile.Create(filePath) do try
    WriteInteger(SECT_RF_LINK, VAR_COMPORT, ComNumber) ;

  finally
    Free ;
  end ;
end ;

procedure TRaftController.SetupUserConfig(const filePath : string) ;
var
  i                 : Integer ;
  saveDecSep        : char ;
begin
  if not FileExists(filePath) then
    raise RCEFileNotFound.Create(filePath) ;

  if FMissionDocument <> nil then FMissionDocument.Free ;
  FMissionDocument := TMissionDocument.Create ;
  with TMissionDocumentFiler.Create(FMissionDocument) do try
    LoadFromFile(filePath) ;

    saveDecSep := DecimalSeparator ;
    DecimalSeparator := '.' ;
    try
      for i := SURF_SENSOR_LOW to SURF_SENSOR_HIGH do begin
        with FMissionDocument.SurfaceSensors[i] do
          if name <> '' then begin
            FRaftData.AddSensor(stLocal, i, name, units, polyCoeffs) ;
          end ;
      end ;

      for i := DEEP_SENSOR_LOW to DEEP_SENSOR_HIGH do begin
        with FMissionDocument.DeepSensors[i] do
          if name <> '' then
            FRaftData.AddSensor(stRemote, i, name, units, polyCoeffs) ;
      end ;

      for i := Low(TIONum) to High(TIONum) do begin
        FOutputNames[i] := FMissionDocument.RelayNames[i + 1] ;
      end ;

      with FMissionDocument.RecorderOptions do begin
        FRecordingAuto := PeriodicMode ;
        FRecordingPeriod := IntervalSecs ;
      end ;

      FConfigured := True ;

    finally
      DecimalSeparator := saveDecSep ;
    end ;

  finally
    Free ;
  end ;
end ;

end.

