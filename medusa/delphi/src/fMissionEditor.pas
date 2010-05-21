unit fMissionEditor ;

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
  StdCtrls,
  ComCtrls,
  IniFiles,
  Menus,
  ActnList,
  StdActns,
  ExtCtrls,
  RzPanel,
  RzTabs,
  RzCommon,
  Mask,
  RzEdit,
  Htmlview,
  RzBtnEdt,
  RzButton,
  MissionDocument,
  RzRadChk,
  RzCmboBx,
  RzStatus,
  ImgList,
  frSensorEditor
  ;

const
  RELAY_COUNT       = 8 ;

type
  TEdSensorProps = record
    name : TRzEdit ;
    units : TRzEdit ;
    coeffs : array[0..3] of TRzNumericEdit ;
  end ;

  TEdRelayProps = record
    name : TRzEdit ;
  end ;

  TMissionEditor = class(TForm)
    MainMenu : TMainMenu ;
    Fichier1 : TMenuItem ;
    Aide1 : TMenuItem ;
    ActionList : TActionList ;
    actAbout : TAction ;
    actFileOpen : TFileOpen ;
    actFileSaveAs : TFileSaveAs ;
    actFileExit : TFileExit ;
    actFileSave : TAction ;
    actFileNew : TAction ;
    Nouvellemission1 : TMenuItem ;
    N1 : TMenuItem ;
    Ouvrir1 : TMenuItem ;
    Enregistrer1 : TMenuItem ;
    Enregistrersous1 : TMenuItem ;
    N2 : TMenuItem ;
    Quitter1 : TMenuItem ;
    Apropos1 : TMenuItem ;
    pcMain : TRzPageControl ;
    tsInfos : TRzTabSheet ;
    tsSensors : TRzTabSheet ;
    RzStatusBar1 : TRzStatusBar ;
    RzFrameController : TRzFrameController ;
    RzPanel1 : TRzPanel ;
    RzPanel2 : TRzPanel ;
    grpNoticeInfos : TRzGroupBox ;
    RzPanel3 : TRzPanel ;
    RzGroupBox2 : TRzGroupBox ;
    Label2 : TLabel ;
    Label3 : TLabel ;
    edLocation : TRzEdit ;
    edDescription : TRzEdit ;
    hvGeneralInfos : THTMLViewer ;
    tsRelays : TRzTabSheet ;
    Label24 : TLabel ;
    Label25 : TLabel ;
    RzPanel6 : TRzPanel ;
    RzGroupBox3 : TRzGroupBox ;
    hvRelays : THTMLViewer ;
    RzPanel7 : TRzPanel ;
    RzGroupBox4 : TRzGroupBox ;
    Label14 : TLabel ;
    Label15 : TLabel ;
    Label18 : TLabel ;
    Label19 : TLabel ;
    Label20 : TLabel ;
    Label21 : TLabel ;
    Label22 : TLabel ;
    Label23 : TLabel ;
    Label16 : TLabel ;
    Label17 : TLabel ;
    Label26 : TLabel ;
    Label27 : TLabel ;
    edRelayName1 : TRzEdit ;
    edRelayName2 : TRzEdit ;
    edRelayName3 : TRzEdit ;
    edRelayName4 : TRzEdit ;
    edRelayName5 : TRzEdit ;
    edRelayName6 : TRzEdit ;
    edRelayName7 : TRzEdit ;
    edRelayName8 : TRzEdit ;
    tsDeepSensors : TRzTabSheet ;
    RzPanel10 : TRzPanel ;
    RzGroupBox6 : TRzGroupBox ;
    hvDeepSensors : THTMLViewer ;
    RzPanel11 : TRzPanel ;
    RzGroupBox7 : TRzGroupBox ;
    Label1 : TLabel ;
    Label30 : TLabel ;
    Label31 : TLabel ;
    Label32 : TLabel ;
    tsRecorderOptions : TRzTabSheet ;
    RzPanel12 : TRzPanel ;
    RzGroupBox8 : TRzGroupBox ;
    hvRecorder : THTMLViewer ;
    RzPanel13 : TRzPanel ;
    RzGroupBox9 : TRzGroupBox ;
    chkPeriodicRecording : TRzCheckBox ;
    edRecordInterval : TRzNumericEdit ;
    Label37 : TLabel ;
    Label38 : TLabel ;
    mnuPredefSensors : TPopupMenu ;
    Label39 : TLabel ;
    cbPredefProbes : TRzComboBox ;
    RzVersionInfo : TRzVersionInfo ;
    RzVersionInfoStatus1 : TRzVersionInfoStatus ;
    RzStatusPane1 : TRzStatusPane ;
    spMsg : TRzStatusPane ;
    fsDocument : TRzFieldStatus ;
    ImageList : TImageList ;
    Label46 : TLabel ;
    Label47 : TLabel ;
    Label48 : TLabel ;
    DeepSensorEditor1 : TSensorEditor ;
    DeepSensorEditor2 : TSensorEditor ;
    DeepSensorEditor3 : TSensorEditor ;
    DeepSensorEditor4 : TSensorEditor ;
    RzPanel4 : TRzPanel ;
    RzGroupBox1 : TRzGroupBox ;
    hvSensorList : THTMLViewer ;
    RzPanel8 : TRzPanel ;
    gbSensors : TRzGroupBox ;
    Label4 : TLabel ;
    Label5 : TLabel ;
    Label6 : TLabel ;
    Label7 : TLabel ;
    SurfSensorEditor1 : TSensorEditor ;
    SurfSensorEditor2 : TSensorEditor ;
    SurfSensorEditor3 : TSensorEditor ;
    SurfSensorEditor4 : TSensorEditor ;
    SurfSensorEditor5 : TSensorEditor ;
    SurfSensorEditor6 : TSensorEditor ;
    procedure FormCreate(Sender : TObject) ;
    procedure actFileOpenOpenDialogShow(Sender : TObject) ;
    procedure actFileOpenAccept(Sender : TObject) ;
    procedure actFileSaveUpdate(Sender : TObject) ;
    procedure actFileSaveExecute(Sender : TObject) ;
    procedure actFileSaveAsAccept(Sender : TObject) ;
    procedure actFileNewExecute(Sender : TObject) ;
    procedure btnPickStockSensorClick(Sender : TObject) ;
    procedure FormDestroy(Sender : TObject) ;
    procedure cbPredefProbesClick(Sender : TObject) ;
    procedure edChange(Sender : TObject) ;
    procedure FormCloseQuery(Sender : TObject ; var CanClose : Boolean) ;
    procedure actAboutExecute(Sender : TObject) ;
    procedure btnPickSensor1MouseDown(Sender : TObject ;
      Button : TMouseButton ; Shift : TShiftState ; X, Y : Integer) ;
    procedure actFileOpenBeforeExecute(Sender : TObject) ;
    procedure actFileSaveAsSaveDialogShow(Sender : TObject) ;
  private
    { Déclarations privées }
    FConfigFilePath : string ;
    FDirty : Boolean ;
    FSurfSensorEditors : array[SURF_SENSOR_LOW..SURF_SENSOR_HIGH] of TSensorEditor ;
    FDeepSensorEditors : array[DEEP_SENSOR_LOW..DEEP_SENSOR_HIGH] of TSensorEditor ;
    FEdRelayProps : array[RELAY_LOW..RELAY_HIGH] of TEdRelayProps ;
    FDocument : TMissionDocument ;
    FDocumentFilePath : string ;
    FPredefinedSurfSensors : TStringList ;
    FPredefinedProbes : TStringList ;
    procedure UpdateUserInterface ;
    procedure UpdateDocument ;
    procedure LoadPredefinedSurfSensors ;
    procedure miPredefSensorClick(Sender : TObject) ;
    procedure ClearSensorList(list : TStringList) ;
    procedure ClearPredefinedProbes ;
    procedure LoadSensorList(list : TStringList ; listName : string ; cfgFile : TIniFile) ;
    procedure DisplayStatusMessage(const msg : string) ;
    procedure SetCurrentDocumentFilePath(const document : string) ;
    procedure DoSaveDocument ;
    procedure LoadMissionDocument(const filePath : string) ;
    procedure SensorEditorChange(Sender : TObject) ;
    function SaveChangesBeforeAction(const action : string) : Boolean ;
  public
    { Déclarations publiques }
  end ;

var
  MissionEditor     : TMissionEditor ;

implementation

{$R *.dfm}

uses
  Math,
  polyreg2,
  MedusaUtils,
  fAboutMissionEditor
  ;

const
  CONFIG_FILE_EXTENSION = '.mdscfg' ;
  CONFIG_FILE_DIR   = 'config' ;

procedure TMissionEditor.FormCreate(Sender : TObject) ;
var
  html              : TStrings ;
  i                 : Integer ;
begin
  FConfigFilePath := IncludeTrailingPathDelimiter(ExtractFileDir(Application.ExeName)) +
    IncludeTrailingPathDelimiter(CONFIG_FILE_DIR) +
    ChangeFileExt(ExtractFileName(Application.ExeName), CONFIG_FILE_EXTENSION) ;

  html := TStringList.Create ;
  try
    html.Add('Entrez dans cette page les informations décrivatnt la mission.') ;
    html.Add('<p>Ces informations sont enregistrées avec le fichier des mesures, et permettent de vous rappeler') ;
    html.Add('plus tard les conditions (endroit, description,...) dans lesquelles l''expérimentation a été faite.') ;
    hvGeneralInfos.LoadStrings(html) ;

    html.Clear ;
    html.Add('Cette page permet de définir les capteurs que vous avez raccordés au boitier du radeau.') ;
    html.Add('<p>Chaque capteur est défini par :<ul>') ;
    html.Add('<li>un nom (obligatoire)</li>') ;
    html.Add('<li>des unités (facultatif)</li>') ;
    html.Add('<li>une formule de conversion (obligatoire)</li></ul>') ;
    html.Add('<p>La formule de conversion sert à calculer la valeur électrique ' +
      'fournie par le capteur en la valeur physique correspondante (température, % de luminosité,...). ' +
      '<p>Pour s''aider à définir cette formule, il est possible de donner des listes de valeurs mesurées ' +
      'pendant une manipulation d''étalonnage. Le logiciel calculera alors automatiquement la formule de conversion. ' +
      'Pour accéder à cet outil, cliquez sur le bouton <b>...</b> à droite de la zone de saisie.') ;
    html.Add('<p>Vous pouvez également sélectionner un capteur déjà configuré en cliquant sur le bouton en début de ligne.') ;
    hvSensorList.LoadStrings(html) ;

    html.Clear ;
    html.Add('Cette page permet de définir les capteurs contenus dans la sonde immergée.') ;
    html.Add('<p>Vous pouvez sélectionner une sonde parmi celles qui sont déjà définies, ' +
      'afin de configurer rapidement tous les capteurs qu''elle contient.') ;
    html.Add('<p>En principe les configurations des capteurs des sondes n''ont pas à être modifiées, ' +
      'mais si vous en avez besoin, vous pouvez le faire comme dans la page des capteurs de surface') ;
    hvDeepSensors.LoadStrings(html) ;

    html.Clear ;
    html.Add('Cette page permet de définir l''utilisation des relais disponibles sur le radeau.') ;
    html.Add('<p>Pour en utiliser un, entrez sa description sur la ligne correspondante.') ;
    html.Add('<p>Le bouton associé sera alors disponible dans le logiciel de contrôle du radeau pour l''activer à volonté.') ;
    hvRelays.LoadStrings(html) ;

    html.Clear ;
    html.Add('Cette page permet de définir les options de fonctionnement de l''enregistreur de mesures.') ;
    html.Add('<dl>') ;
    html.Add('<dt><b>enregistrement automatique</b></dt>') ;
    html.Add('<dd>Si cette case est cochée, une fois activé, l''enregistreur écrira sur disque toutes les mesures de manière régulière.') ;
    html.Add('Si elle n''est pas cochée, l''enregistrement se fera manuellement en appuyant sur le bouton de déclenchement lorsqu''on voudra sauvegarder des mesures</dd>') ;
    html.Add('<dt><b>intervalle entre deux enregistrements</b></dt>') ;
    html.Add('<dd>Lorsque l''enregistrement automatique est activé, détermine l''intervalle de temps (en secondes) séparant deux enregistrements de mesures</dd>') ;
    html.Add('</dl>') ;
    html.Add('<p>A noter que quelle que soient les options choisies ici, il est possible de modifier le fonctionnement de l''enregistreur pendant le déroulement de la mission. ') ;
    html.Add('Les indications saisies dans cette page servent uniquement à définir les options utilisées lors de la mission si aucune autre indication n''est donnée à ce moment-là.') ;
    hvRecorder.LoadStrings(html) ;

  finally
    html.Free ;
  end ;

  for i := Low(FSurfSensorEditors) to High(FSurfSensorEditors) do begin
    FSurfSensorEditors[i] := TSensorEditor(FindComponent('SurfSensorEditor' + IntToStr(i))) ;
    with FSurfSensorEditors[i] do begin
      SensorNum := i ;
      OnChange := SensorEditorChange ;
      Clear ;
    end ;
  end ;

  for i := Low(FDeepSensorEditors) to High(FDeepSensorEditors) do begin
    FDeepSensorEditors[i] := TSensorEditor(FindComponent('DeepSensorEditor' + IntToStr(i))) ;
    with FDeepSensorEditors[i] do begin
      SensorNum := i ;
      OnChange := SensorEditorChange ;
      Clear ;
    end ;
  end ;

  for i := 1 to RELAY_COUNT do begin
    with FEdRelayProps[i] do begin
      name := TRzEdit(FindComponent('edRelayName' + IntToStr(i))) ;
    end ;
  end ;

  FPredefinedSurfSensors := TStringList.Create ;
  FPredefinedProbes := TStringList.Create ;
  LoadPredefinedSurfSensors ;

  FDocument := TMissionDocument.Create ;
  UpdateUserInterface ;

  if ParamCount > 0 then
    LoadMissionDocument(ParamStr(1)) ;

  DisplayStatusMessage('Prêt') ;
end ;

procedure TMissionEditor.LoadPredefinedSurfSensors() ;
const
  SECT_PREDEF_PROBES = 'predef-probes' ;
  VAR_COUNT         = 'count' ;
  SECT_PROBE        = 'probe.%d' ;
  VAR_PROBE_NAME    = 'probe.name' ;
var
  defFile           : TIniFile ;
  cnt               : Integer ;
  i                 : Integer ;
  sensorName        : string ;
  probeName         : string ;
  sectName          : string ;
  mnuItem           : TMenuItem ;
  probeSensors      : TStringList ;
begin
  ClearSensorList(FPredefinedSurfSensors) ;
  mnuPredefSensors.Items.Clear ;

  ClearPredefinedProbes ;
  cbPredefProbes.Items.Clear ;
  cbPredefProbes.Items.Add('- aucune -') ;

  defFile := TIniFile.Create(FConfigFilePath) ;
  try
    LoadSensorList(FPredefinedSurfSensors, 'predef', defFile) ;
    for i := 0 to FPredefinedSurfSensors.Count - 1 do begin
      sensorName := FPredefinedSurfSensors[i] ;
      mnuItem := TMenuItem.Create(Self) ;
      mnuItem.Caption := sensorName ;
      mnuItem.OnClick := miPredefSensorClick ;
      mnuPredefSensors.Items.Add(mnuItem) ;
    end ;

    cnt := defFile.ReadInteger(SECT_PREDEF_PROBES, VAR_COUNT, 0) ;
    for i := 1 to cnt do begin
      sectName := Format(SECT_PROBE, [i]) ;
      probeName := defFile.ReadString(sectName, VAR_PROBE_NAME, '') ;
      if probeName <> '' then begin
        probeSensors := TStringList.Create ;
        LoadSensorList(probeSensors, sectName, defFile) ;
        FPredefinedProbes.AddObject(probeName, probeSensors) ;

        cbPredefProbes.AddItem(probeName, probeSensors) ;
      end ;
    end ;
    cbPredefProbes.ItemIndex := 0 ;

  finally
    defFile.Free ;
  end ;
end ;

procedure TMissionEditor.LoadSensorList(list : TStringList ; listName : string ; cfgFile : TIniFile) ;
const
  SECT_LIST         = '%s-sensors' ;
  SECT_SENSOR       = '%s-sensor.%d' ;
  VAR_COUNT         = 'count' ;
  VAR_SENSOR_NAME   = 'sensor.name' ;
  VAR_SENSOR_UNITS  = 'sensor.units' ;
  VAR_SENSOR_COEFF  = 'sensor.coeff.%d' ;
var
  sectName          : string ;
  cnt               : Integer ;
  i                 : Integer ;
  sensorName        : string ;
  specs             : TSensorDescriptor ;

begin
  sectName := Format(SECT_LIST, [listName]) ;
  cnt := cfgFile.ReadInteger(sectName, VAR_COUNT, 0) ;
  for i := 1 to cnt do begin
    sectName := Format(SECT_SENSOR, [listName, i]) ;
    sensorName := cfgFile.ReadString(sectName, VAR_SENSOR_NAME, '') ;
    if sensorName <> '' then begin
      specs := TSensorDescriptor.Create ;
      with specs do begin
        name := sensorName ;
        units := cfgFile.ReadString(sectName, VAR_SENSOR_UNITS, '') ;
        TPolynom.ReadPolynomCoeffsFromIni(polyCoeffs, cfgFile, sectName, VAR_SENSOR_COEFF) ;
      end ;
      list.AddObject(sensorName, specs) ;
    end ;
  end ;
end ;

procedure TMissionEditor.ClearSensorList(list : TStringList) ;
var
  i                 : Integer ;
begin
  for i := 0 to list.Count - 1 do
    list.Objects[i].Free ;
  list.Clear ;
end ;

procedure TMissionEditor.ClearPredefinedProbes ;
var
  i                 : Integer ;
begin
  for i := 0 to FPredefinedProbes.Count - 1 do begin
    ClearSensorList(TStringList(FPredefinedProbes.Objects[i])) ;
    FPredefinedProbes.Objects[i].Free ;
  end ;
  FPredefinedProbes.Clear ;
end ;

procedure TMissionEditor.FormDestroy(Sender : TObject) ;
begin
  FDocument.Free ;
  FPredefinedSurfSensors.Free ;
  FPredefinedProbes.Free ;
end ;

procedure TMissionEditor.FormCloseQuery(Sender : TObject ;
  var CanClose : Boolean) ;
begin
  CanClose := SaveChangesBeforeAction('quitter l''application') ;
end ;

function TMissionEditor.SaveChangesBeforeAction(const action : string) : Boolean ;
begin
  if FDirty then begin
    case MessageDlg('Voulez-vous enregistrer les modifications avant de ' + action + ' ?',
      mtConfirmation, mbYesNoCancel, 0) of
      mrYes : begin
          actFileSave.Execute ;
          Result := not FDirty ;
        end ;
      mrNo :
        Result := True ;
    else
      Result := False ;
    end ;

  end

  else
    Result := True ;
end ;

procedure TMissionEditor.DisplayStatusMessage(const msg : string) ;
begin
  spMsg.Caption := msg ;
end ;

procedure TMissionEditor.btnPickStockSensorClick(Sender : TObject) ;
var
  btn               : TRzButton ;
  pt                : TPoint ;
begin
  btn := TRzButton(Sender) ;

  mnuPredefSensors.Tag := btn.Tag ;

  pt.X := 0 ;
  pt.Y := 0 ;
  pt := btn.ClientToScreen(pt) ;

  mnuPredefSensors.Popup(pt.X, pt.Y) ;
end ;

procedure TMissionEditor.miPredefSensorClick(Sender : TObject) ;
var
  item              : TMenuItem ;
  sensorName        : string ;
  ndx               : Integer ;
  sensorDef         : TSensorDescriptor ;
  sensorEditor      : TSensorEditor ;

begin
  item := TMenuItem(Sender) ;
  sensorEditor := TSensorEditor(item.GetParentMenu.Tag) ;

  sensorName := item.Caption ;
  ndx := FPredefinedSurfSensors.IndexOf(sensorName) ;
  sensorDef := TSensorDescriptor(FPredefinedSurfSensors.Objects[ndx]) ;

  sensorEditor.Sensor := sensorDef ;
  FDirty := True ;
end ;

procedure TMissionEditor.cbPredefProbesClick(Sender : TObject) ;
var
  ndx               : Integer ;
  probeSensors      : TStringList ;
  SensorNum         : Integer ;
begin
  ndx := cbPredefProbes.ItemIndex ;
  if ndx = -1 then Exit ;

  for SensorNum := Low(FDeepSensorEditors) to High(FDeepSensorEditors) do
    FDeepSensorEditors[SensorNum].Clear ;

  probeSensors := TStringList(cbPredefProbes.Items.Objects[ndx]) ;
  if probeSensors <> nil then begin
    for SensorNum := 1 to probeSensors.Count do begin
      FDeepSensorEditors[SensorNum].Sensor := TSensorDescriptor(probeSensors.Objects[SensorNum - 1]) ;
    end ;
  end ;
end ;

procedure TMissionEditor.actFileOpenOpenDialogShow(Sender : TObject) ;
begin
  if actFileOpen.Dialog.InitialDir = '' then
    actFileOpen.Dialog.InitialDir := GetSpecialFolder('Personal') + '\Medusa1\' ;
end ;

procedure TMissionEditor.actFileOpenBeforeExecute(Sender : TObject) ;
begin
  if not SaveChangesBeforeAction('charger le document') then Abort ;
end ;

procedure TMissionEditor.actFileOpenAccept(Sender : TObject) ;
begin
  LoadMissionDocument(actFileOpen.Dialog.FileName) ;
end ;

procedure TMissionEditor.LoadMissionDocument(const filePath : string) ;
begin
  with TMissionDocumentFiler.Create(FDocument) do try
    SetCurrentDocumentFilePath(filePath) ;
    LoadFromFile(FDocumentFilePath) ;
    UpdateUserInterface() ;
    DisplayStatusMessage('Document chargé.') ;

  finally
    Free ;
  end ;
end ;

procedure TMissionEditor.SetCurrentDocumentFilePath(const document : string) ;
begin
  FDocumentFilePath := document ;
  fsDocument.Caption := FDocumentFilePath ;
end ;

procedure TMissionEditor.actFileSaveUpdate(Sender : TObject) ;
begin
  actFileSave.Enabled := FDirty ;
end ;

procedure TMissionEditor.actFileSaveExecute(Sender : TObject) ;
begin
  if FDocumentFilePath <> '' then
    DoSaveDocument

  else
    actFileSaveAs.Execute ;
end ;

procedure TMissionEditor.actFileSaveAsSaveDialogShow(Sender : TObject) ;
begin
  actFileSaveAs.Dialog.FileName := ExtractFileName(FDocumentFilePath) ;
  actFileSaveAs.Dialog.InitialDir := ExtractFilePath(FDocumentFilePath) ;
end ;

procedure TMissionEditor.actFileSaveAsAccept(Sender : TObject) ;
begin
  SetCurrentDocumentFilePath(actFileSaveAs.Dialog.FileName) ;
  DoSaveDocument ;
end ;

procedure TMissionEditor.DoSaveDocument ;
begin
  UpdateDocument ;
  with TMissionDocumentFiler.Create(FDocument) do try
    StoreToFile(FDocumentFilePath) ;
    FDirty := False ;
    DisplayStatusMessage('Modifications enregistrées.') ;

  finally
    Free ;
  end ;
end ;

procedure TMissionEditor.UpdateUserInterface ;
var
  i                 : Integer ;
begin
  edLocation.Text := FDocument.Location ;
  edDescription.Text := FDocument.Description ;

  for i := SURF_SENSOR_LOW to SURF_SENSOR_HIGH do
    FSurfSensorEditors[i].Sensor := FDocument.SurfaceSensors[i] ;

  for i := DEEP_SENSOR_LOW to DEEP_SENSOR_HIGH do
    FDeepSensorEditors[i].Sensor := FDocument.DeepSensors[i] ;

  for i := RELAY_LOW to RELAY_HIGH do begin
    TEdit(FindComponent('edRelayName' + IntToStr(i))).Text := FDocument.RelayNames[i] ;
  end ;

  with FDocument.RecorderOptions do begin
    chkPeriodicRecording.Checked := PeriodicMode ;
    edRecordInterval.IntValue := IntervalSecs ;
  end ;

  FDirty := False ;
end ;

procedure TMissionEditor.UpdateDocument ;
var
  i                 : Integer ;
begin
  FDocument.Location := edLocation.Text ;
  FDocument.Description := edDescription.Text ;

  for i := Low(FSurfSensorEditors) to High(FSurfSensorEditors) do begin
    FSurfSensorEditors[i].UpdateModel ;
  end ;

  for i := Low(FDeepSensorEditors) to High(FDeepSensorEditors) do begin
    FDeepSensorEditors[i].UpdateModel ;
  end ;

  for i := 1 to RELAY_COUNT do begin
    FDocument.RelayNames[i] := TEdit(FindComponent('edRelayName' + IntToStr(i))).Text ;
  end ;

  with FDocument.RecorderOptions do begin
    PeriodicMode := chkPeriodicRecording.Checked ;
    IntervalSecs := edRecordInterval.IntValue ;
  end ;
end ;

procedure TMissionEditor.actFileNewExecute(Sender : TObject) ;
begin
  if not SaveChangesBeforeAction('créer un nouveau document') then Exit ;

  FDocument.Clear ;
  SetCurrentDocumentFilePath('') ;
  UpdateUserInterface ;
  DisplayStatusMessage('Nouveau document.') ;
end ;

procedure TMissionEditor.edChange(Sender : TObject) ;
begin
  FDirty := True ;
  DisplayStatusMessage('Document modifié.') ;
end ;

procedure TMissionEditor.SensorEditorChange(Sender : TObject) ;
begin
  edChange(Sender) ;
end ;

procedure TMissionEditor.actAboutExecute(Sender : TObject) ;
begin
  TAboutMissionEditor.Display ;
end ;

procedure TMissionEditor.btnPickSensor1MouseDown(Sender : TObject ;
  Button : TMouseButton ; Shift : TShiftState ; X, Y : Integer) ;
begin
  mnuPredefSensors.Tag := TRzToolButton(Sender).Tag ;
end ;

end.

