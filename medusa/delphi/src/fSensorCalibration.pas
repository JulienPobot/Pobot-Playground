unit fSensorCalibration ;

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
  RzCommon,
  RzButton,
  Series,
  TeEngine,
  TeeProcs,
  Chart,
  StdCtrls,
  Mask,
  RzEdit,
  ExtCtrls,
  MedusaUtils,
  RzPanel,
  Htmlview ;

const
  CALIBRATION_POINT_MAX_COUNT = 5 ;

type
  TCalibrationPointEditor = record
    rawValueEditor : TRzNumericEdit ;
    physValueEditor : TRzNumericEdit ;
  end ;

  TSensorCalibration = class(TForm)
    gbCalibration : TRzGroupBox ;
    Label28 : TLabel ;
    Label29 : TLabel ;
    lbUnits : TLabel ;
    lbFormula : TLabel ;
    edRawValue1 : TRzNumericEdit ;
    edPhysValue1 : TRzNumericEdit ;
    edRawValue2 : TRzNumericEdit ;
    edPhysValue2 : TRzNumericEdit ;
    edRawValue3 : TRzNumericEdit ;
    edPhysValue3 : TRzNumericEdit ;
    edRawValue4 : TRzNumericEdit ;
    edPhysValue4 : TRzNumericEdit ;
    edRawValue5 : TRzNumericEdit ;
    edPhysValue5 : TRzNumericEdit ;
    Chart : TChart ;
    Series1 : TLineSeries ;
    Series2 : TPointSeries ;
    btnCalcFormula : TRzButton ;
    btnSaveFormula : TRzButton ;
    btnCancel : TRzButton ;
    btnClear : TRzButton ;
    RzFrameController1 : TRzFrameController ;
    RzGroupBox1 : TRzGroupBox ;
    hvHelp : THTMLViewer ;
    procedure FormCreate(Sender : TObject) ;
    procedure btnClearClick(Sender : TObject) ;
    procedure btnCalcFormulaClick(Sender : TObject) ;
    procedure FormDestroy(Sender : TObject) ;
    procedure edPhysValueKeyPress(Sender : TObject ; var Key : char) ;
    procedure edRawValue1Exit(Sender : TObject) ;
  private
    { Déclarations privées }
    FPointEditors : array[1..CALIBRATION_POINT_MAX_COUNT] of TCalibrationPointEditor ;
    FPolynom : TPolynom ;

    procedure Clear ;
  public
    procedure SetUnits(units : string) ;
    { Déclarations publiques }
    property polynom : TPolynom read FPolynom ;

  end ;

var
  SensorCalibration : TSensorCalibration ;

implementation

uses
  polyreg2,
  MissionDocument
  ;

{$R *.dfm}

procedure TSensorCalibration.Clear ;
var
  i                 : Integer ;
begin
  for i := Low(FPointEditors) to High(FPointEditors) do
    with FPointEditors[i] do begin
      rawValueEditor.Clear ;
      physValueEditor.Clear ;
    end ;
end ;

procedure TSensorCalibration.SetUnits(units : string) ;
begin
  if units <> '' then
    lbUnits.Caption := '(' + units + ')'
  else
    lbUnits.Caption := '' ;
end ;

procedure TSensorCalibration.FormCreate(Sender : TObject) ;
var
  i                 : Integer ;
  html              : TStringList ;
begin
  html := TStringList.Create ;
  try
    html.Add('Cette page permet d''étalonner un capteur en fournissant une liste de mesures expérimentales.') ;
    html.Add('<p>La méthode consiste à entrer plusieurs mesures effectuées (entre 3 et 5), avec pour chacune d''entre elles :') ;
    html.Add('<ul><li>la valeur brute renvoyée par le capteur, telle qu''affichée par le logiciel de contrôle du radeau</li>') ;
    html.Add('<li>la valeur physique qu''elle représente</li></ul>') ;
    html.Add('<p>En appuyant sur le bouton <b>Calculer la formule</b> l''outil va générer la formule de conversion correspondante, ') ;
    html.Add('et l''afficher pour information. En cliquant ensuite sur <b>Enregistrer</b> cette formule sera reportée dans la zone de saisie ') ;
    html.Add('de la configuration du capteur concerné.') ;
    hvHelp.LoadStrings(html) ;
  finally
    html.Free ;
  end ;

  for i := Low(FPointEditors) to High(FPointEditors) do
    with FPointEditors[i] do begin
      rawValueEditor := TRzNumericEdit(FindComponent('edRawValue' + IntToStr(i))) ;
      physValueEditor := TRzNumericEdit(FindComponent('edPhysValue' + IntToStr(i))) ;
    end ;

  Clear ;
end ;

procedure TSensorCalibration.btnClearClick(Sender : TObject) ;
begin
  Clear ;
end ;

procedure TSensorCalibration.btnCalcFormulaClick(Sender : TObject) ;
var
  i                 : Integer ;
  nbPts             : Integer ;
  funcPts           : array of TFuncPoint ;
  ptNum             : Integer ;
  edRaw             : TRzNumericEdit ;
  edPhys            : TRzNumericEdit ;
  coeffs            : TArrayDouble ;
  X                 : extended ;
  xMin, xMax, step  : extended ;
begin
  for i := 0 to Chart.SeriesCount - 1 do
    Chart.Series[i].Clear ;
  lbFormula.Caption := '' ;
  btnSaveFormula.Enabled := False ;

  nbPts := 0 ;
  for i := Low(FPointEditors) to High(FPointEditors) do
    if FPointEditors[i].rawValueEditor.Text <> '' then Inc(nbPts) ;

  if nbPts <> 0 then begin
    SetLength(funcPts, nbPts) ;
    ptNum := 0 ;
    for i := Low(FPointEditors) to High(FPointEditors) do begin
      edRaw := FPointEditors[i].rawValueEditor ;
      if edRaw.IntValue <> 0 then begin
        edPhys := FPointEditors[i].physValueEditor ;
        with funcPts[ptNum] do begin
          X := edRaw.IntValue ;
          Y := edPhys.Value ;
          Chart.Series[1].AddXY(X, Y) ;
        end ;
        Inc(ptNum) ;
      end ;
    end ;

    coeffs := ComputePolynomCoefs(POLY_REG_ORDER, funcPts) ;

    FPolynom.Free ;
    FPolynom := TPolynom.Create(coeffs) ;

    xMin := Chart.Series[1].MinXValue ;
    xMax := Chart.Series[1].MaxXValue ;
    step := (xMax - xMin) / 10 ;
    X := xMin ;
    for i := 0 to 10 do begin
      Chart.Series[0].AddXY(X, FPolynom.Evaluate(X)) ;
      X := X + step ;
    end ;

    lbFormula.Caption := FPolynom.ToString ;
    lbFormula.Visible := True ;

    btnSaveFormula.Enabled := True ;
  end ;
end ;

procedure TSensorCalibration.FormDestroy(Sender : TObject) ;
begin
  FPolynom.Free ;
end ;

procedure TSensorCalibration.edPhysValueKeyPress(Sender : TObject ;
  var Key : char) ;
begin
  if Key = '.' then Key := ',' ;
end ;

procedure TSensorCalibration.edRawValue1Exit(Sender : TObject) ;
var
  ed                : TRzNumericEdit ;
begin
  ed := TRzNumericEdit(Sender) ;
  if ed.Text = '' then
    FPointEditors[ed.Tag].physValueEditor.Text := '' ;
end ;

end.

