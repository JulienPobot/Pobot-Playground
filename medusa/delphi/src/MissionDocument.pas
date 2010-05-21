unit MissionDocument ;

interface
uses
  IniFiles ;

const
  SURF_SENSOR_LOW   = 1 ;
  SURF_SENSOR_HIGH  = 6 ;

  DEEP_SENSOR_LOW   = 1 ;
  DEEP_SENSOR_HIGH  = 4 ;

  RELAY_LOW         = 1 ;
  RELAY_HIGH        = 8 ;

  POLY_REG_ORDER    = 3 ;

type
  TSensorDescriptor = class
    name : string ;
    units : string ;
    polyCoeffs : array[0..POLY_REG_ORDER] of double ;
  end ;

  TRecorderOptions = record
    PeriodicMode : Boolean ;
    IntervalSecs : Integer ;
  end ;

  TMissionDocument = class
  private
    FLocation : string ;
    FDescription : string ;
    FSurfaceSensors : array[SURF_SENSOR_LOW..SURF_SENSOR_HIGH] of TSensorDescriptor ;
    FDeepSensors : array[DEEP_SENSOR_LOW..DEEP_SENSOR_HIGH] of TSensorDescriptor ;
    FRelayNames : array[RELAY_LOW..RELAY_HIGH] of string ;
    FRecorderOptions : TRecorderOptions ;
    function GetSurfaceSensors(num : Integer) : TSensorDescriptor ;
    procedure SetSurfaceSensors(num : Integer ; const Value : TSensorDescriptor) ;
    function GetRelayNames(num : Integer) : string ;
    procedure SetRelayNames(num : Integer ; const Value : string) ;
    function GetDeepSensors(num : Integer) : TSensorDescriptor ;
    procedure SetDeepSensors(num : Integer ;
      const Value : TSensorDescriptor) ;
  public
    constructor Create ;
    destructor Destroy ; override ;
    procedure Clear ;
    property Description : string read FDescription write FDescription ;
    property Location : string read FLocation write FLocation ;
    property RelayNames[num : Integer] : string read GetRelayNames write SetRelayNames ;
    property SurfaceSensors[num : Integer] : TSensorDescriptor read GetSurfaceSensors write SetSurfaceSensors ;
    property DeepSensors[num : Integer] : TSensorDescriptor read GetDeepSensors write SetDeepSensors ;
    property RecorderOptions : TRecorderOptions read FRecorderOptions write FRecorderOptions ;
  end ;

  TMissionDocumentFiler = class
  private
    FDocument : TMissionDocument ;
    function ConvertFormat(docFile : TIniFile) : Boolean ;
  public
    constructor Create(document : TMissionDocument) ;
    procedure LoadFromFile(const filePath : string) ;
    procedure StoreToFile(const filePath : string) ;
  end ;

implementation

uses
  Dialogs,
  SysUtils ;

constructor TMissionDocument.Create ;
begin
  Clear ;
end ;

destructor TMissionDocument.Destroy ;
var
  i                 : Integer ;
begin
  for i := Low(FSurfaceSensors) to High(FSurfaceSensors) do begin
    FSurfaceSensors[i].Free ;
  end ;
  for i := Low(FDeepSensors) to High(FDeepSensors) do begin
    FDeepSensors[i].Free ;
  end ;

  inherited ;
end ;

procedure TMissionDocument.Clear ;
var
  i                 : Integer ;
begin
  FLocation := '' ;
  FDescription := '' ;

  for i := Low(FSurfaceSensors) to High(FSurfaceSensors) do begin
    FSurfaceSensors[i].Free ;
    FSurfaceSensors[i] := TSensorDescriptor.Create ;
  end ;

  for i := Low(FDeepSensors) to High(FDeepSensors) do begin
    FDeepSensors[i].Free ;
    FDeepSensors[i] := TSensorDescriptor.Create ;
  end ;

  for i := Low(FRelayNames) to High(FRelayNames) do
    FRelayNames[i] := '' ;

  with FRecorderOptions do begin
    PeriodicMode := True ;
    IntervalSecs := 30 ;
  end ;
end ;

function TMissionDocument.GetRelayNames(num : Integer) : string ;
begin
  Result := FRelayNames[num] ;
end ;

function TMissionDocument.GetSurfaceSensors(num : Integer) : TSensorDescriptor ;
begin
  Result := FSurfaceSensors[num] ;
end ;

procedure TMissionDocument.SetRelayNames(num : Integer ;
  const Value : string) ;
begin
  FRelayNames[num] := Value ;
end ;

procedure TMissionDocument.SetSurfaceSensors(num : Integer ; const Value : TSensorDescriptor) ;
begin
  FSurfaceSensors[num] := Value ;
end ;

function TMissionDocument.GetDeepSensors(num : Integer) : TSensorDescriptor ;
begin
  Result := FDeepSensors[num] ;
end ;

procedure TMissionDocument.SetDeepSensors(num : Integer ; const Value : TSensorDescriptor) ;
begin
  FDeepSensors[num] := Value ;
end ;

{ TMissionDocumentFiler }

const
  FORMAT_VERSION    = 2 ;

constructor TMissionDocumentFiler.Create(document : TMissionDocument) ;
begin
  FDocument := document ;
end ;

const
  SECT_HEADER       = 'head' ;
  KEY_FMT_VERSION   = 'format' ;

  SECT_INFOS        = 'infos' ;
  KEY_LOCATION      = 'lieu' ;
  KEY_DESCRIPTION   = 'description' ;

  SECT_LOCAL_SENSORS = 'capteur-surface.%d' ;
  SECT_REMOTE_SENSORS = 'capteur-fond.%d' ;
  KEY_SENSOR_NAME   = 'nom' ;
  KEY_UNITS         = 'unités' ;
  KEY_POLY_COEFF    = 'coeff.%d' ;
  KEY_CONV_FORMULA  = 'conversion' ;

  SECT_RELAYS       = 'relais' ;
  KEY_RELAY_NAME    = 'nom.%d' ;

  SECT_RECORDER     = 'enregistreur' ;
  KEY_PERIODIC_MODE = 'enreg-auto' ;
  KEY_RECORD_PERIOD = 'enreg-période' ;

function TMissionDocumentFiler.ConvertFormat(docFile : TIniFile) : Boolean ;
var
  i                 : Integer ;
  sectName          : string ;
  keyName           : string ;
  formula           : string ;
  coeffs            : array[0..POLY_REG_ORDER] of double ;

  function ParseFormula(formula : string ; var coeffs : array of double) : Boolean ;
  type
    ParserState = (Start, InCoeff, InFactor, InExponent) ;
  var
    k               : Integer ;
    p               : Integer ;
    buffer          : string ;
    c               : char ;
    state           : ParserState ;

    procedure SetCoeff(k : Integer) ;
    begin
      if length(buffer) > 0 then begin
        if DecimalSeparator = ',' then
          buffer := StringReplace(buffer, '.', DecimalSeparator, [rfReplaceAll])
        else
          buffer := StringReplace(buffer, ',', DecimalSeparator, [rfReplaceAll]) ;

        coeffs[k] := StrToFloat(buffer) ;
        buffer := '' ;
      end
      else
        coeffs[k] := 1 ;
    end ;

  begin
    for k := Low(coeffs) to High(coeffs) do
      coeffs[k] := 0 ;

    p := 1 ;
    k := 0 ;
    buffer := '' ;
    state := Start ;
    formula := LowerCase(formula) ;
    while p <= length(formula) do begin
      c := formula[p] ;
      case state of
        Start : begin
            if c = 'x' then
              state := InFactor
            else
              state := InCoeff ;
            Dec(p) ;
          end ;
        InCoeff :
          if c <> '*' then begin
            if c in ['0'..'9', '.', ','] then
              buffer := buffer + c
          end
          else
            state := InFactor ;
        InFactor :
          if c = 'x' then begin
            k := 1 ;
            state := InExponent ;
          end ;
        InExponent :
          case c of
            '^' : ;
            '0'..'9' : begin
                SetCoeff(Ord(c) - Ord('0')) ;
                state := InCoeff ;
              end ;
          else
            // passage au terme suivant sans avoir eu l'exposant => terme d'ordre 1
            SetCoeff(k) ;
            state := InCoeff ;
            Dec(p) ;                    // pour ne pas perdre le caractère
          end ;                         // case c
      end ;                             // case state

      Inc(p) ;
    end ;

    // traitement du terme résiduel
    case state of
      InCoeff :
        SetCoeff(0) ;
      InExponent :
        SetCoeff(1) ;
    end ;

    Result := True ;
  end ;

  procedure ConvertFormula(formula, sectName, keyName : string) ;
  var
    k               : Integer ;
  begin
    ParseFormula(formula, coeffs) ;

    for k := 0 to POLY_REG_ORDER do begin
      keyName := Format(KEY_POLY_COEFF, [k]) ;
      docFile.WriteFloat(sectName, keyName, coeffs[k]) ;
    end ;
  end ;

begin
  for i := SURF_SENSOR_LOW to SURF_SENSOR_HIGH do begin
    sectName := Format(SECT_LOCAL_SENSORS, [i]) ;
    formula := docFile.ReadString(sectName, KEY_CONV_FORMULA, '') ;
    ConvertFormula(formula, sectName, keyName) ;
  end ;

  for i := DEEP_SENSOR_LOW to DEEP_SENSOR_HIGH do begin
    sectName := Format(SECT_REMOTE_SENSORS, [i]) ;
    formula := docFile.ReadString(sectName, KEY_CONV_FORMULA, '') ;
    ConvertFormula(formula, sectName, keyName) ;
  end ;

  Result := True ;
end ;

procedure TMissionDocumentFiler.LoadFromFile(const filePath : string) ;
var
  docFile           : TIniFile ;
  i                 : Integer ;
  k                 : Integer ;
  keyName           : string ;
  sectName          : string ;
  descr             : TSensorDescriptor ;
  version           : Integer ;

begin
  if not FileExists(filePath) then
    raise Exception.Create('Fichier non trouvé : ' + filePath) ;

  FDocument.Clear ;

  docFile := TIniFile.Create(filePath) ;
  with docFile do try
    version := ReadInteger(SECT_HEADER, KEY_FMT_VERSION, 0) ;
    if version < FORMAT_VERSION then begin
      MessageDlg(
        'Ce fichier utilise un ancien format.'#13#10 +
        'Il va être converti au nouveau format',
        mtInformation, [mbOK], 0) ;
      ConvertFormat(docFile) ;
    end ;

    FDocument.Location := ReadString(SECT_INFOS, KEY_LOCATION, '') ;
    FDocument.Description := ReadString(SECT_INFOS, KEY_DESCRIPTION, '') ;

    for i := SURF_SENSOR_LOW to SURF_SENSOR_HIGH do begin
      sectName := Format(SECT_LOCAL_SENSORS, [i]) ;
      descr := TSensorDescriptor.Create ;
      descr.name := ReadString(sectName, KEY_SENSOR_NAME, '') ;
      descr.units := ReadString(sectName, KEY_UNITS, '') ;

      for k := 0 to POLY_REG_ORDER do begin
        keyName := Format(KEY_POLY_COEFF, [k]) ;
        descr.polyCoeffs[k] := ReadFloat(sectName, keyName, 0) ;
      end ;

      FDocument.SurfaceSensors[i] := descr ;
    end ;

    for i := DEEP_SENSOR_LOW to DEEP_SENSOR_HIGH do begin
      sectName := Format(SECT_REMOTE_SENSORS, [i]) ;
      descr := TSensorDescriptor.Create ;
      descr.name := ReadString(sectName, KEY_SENSOR_NAME, '') ;
      descr.units := ReadString(sectName, KEY_UNITS, '') ;
      for k := 0 to POLY_REG_ORDER do begin
        keyName := Format(KEY_POLY_COEFF, [k]) ;
        descr.polyCoeffs[k] := ReadFloat(sectName, keyName, 0) ;
      end ;

      FDocument.DeepSensors[i] := descr ;
    end ;

    for i := RELAY_LOW to RELAY_HIGH do begin
      FDocument.RelayNames[i] := ReadString(SECT_RELAYS, Format(KEY_RELAY_NAME, [i]), '') ;
    end ;

    with FDocument.RecorderOptions do begin
      PeriodicMode := ReadBool(SECT_RECORDER, KEY_PERIODIC_MODE, True) ;
      IntervalSecs := ReadInteger(SECT_RECORDER, KEY_RECORD_PERIOD, 30) ;
    end ;

  finally
    Free ;
  end ;
end ;

procedure TMissionDocumentFiler.StoreToFile(const filePath : string) ;
var
  i                 : Integer ;
  k                 : Integer ;
  sectName          : string ;
  keyName           : string ;
  iniFile           : TIniFile ;

  procedure WriteKey(sectName, keyName, Value : string) ;
  begin
    if Value <> '' then
      iniFile.WriteString(sectName, keyName, Value)
    else
      iniFile.DeleteKey(sectName, keyName) ;
  end ;

begin
  iniFile := TIniFile.Create(filePath) ;
  with iniFile do try
    WriteKey(SECT_HEADER, KEY_FMT_VERSION, IntToStr(FORMAT_VERSION)) ;

    WriteKey(SECT_INFOS, KEY_LOCATION, FDocument.Location) ;
    WriteKey(SECT_INFOS, KEY_DESCRIPTION, FDocument.Description) ;

    for i := SURF_SENSOR_LOW to SURF_SENSOR_HIGH do begin
      sectName := Format(SECT_LOCAL_SENSORS, [i]) ;
      with FDocument.SurfaceSensors[i] do begin
        if name <> '' then begin
          WriteKey(sectName, KEY_SENSOR_NAME, name) ;
          WriteKey(sectName, KEY_UNITS, units) ;
          for k := 0 to POLY_REG_ORDER do begin
            keyName := Format(KEY_POLY_COEFF, [k]) ;
            WriteKey(sectName, keyName, Format('%.4e', [polyCoeffs[k]])) ;
          end ;
        end
        else
          EraseSection(sectName) ;
      end ;
    end ;

    for i := DEEP_SENSOR_LOW to DEEP_SENSOR_HIGH do begin
      sectName := Format(SECT_REMOTE_SENSORS, [i]) ;
      with FDocument.DeepSensors[i] do begin
        if name <> '' then begin
          WriteKey(sectName, KEY_SENSOR_NAME, name) ;
          WriteKey(sectName, KEY_UNITS, units) ;
          for k := 0 to POLY_REG_ORDER do begin
            keyName := Format(KEY_POLY_COEFF, [k]) ;
            WriteKey(sectName, keyName, Format('%.4e', [polyCoeffs[k]])) ;
          end ;
        end
        else
          EraseSection(sectName) ;
      end ;
    end ;

    for i := RELAY_LOW to RELAY_HIGH do begin
      keyName := Format(KEY_RELAY_NAME, [i]) ;
      WriteKey(SECT_RELAYS, keyName, FDocument.RelayNames[i]) ;
    end ;

    with FDocument.RecorderOptions do begin
      WriteBool(SECT_RECORDER, KEY_PERIODIC_MODE, PeriodicMode) ;
      WriteInteger(SECT_RECORDER, KEY_RECORD_PERIOD, IntervalSecs) ;
    end ;

  finally
    Free ;
  end ;
end ;

end.

