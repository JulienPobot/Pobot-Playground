unit frSensorEditor ;

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
  RzEdit,
  StdCtrls,
  Mask,
  Menus,
  RzButton,
  MissionDocument ;

type
  TSensorEditor = class(TFrame)
    btnPickSensor : TRzToolButton ;
    lbSensorNum : TLabel ;
    edName : TRzEdit ;
    edUnits : TRzEdit ;
    edCoeff3 : TRzNumericEdit ;
    Label40 : TLabel ;
    edCoeff2 : TRzNumericEdit ;
    Label41 : TLabel ;
    edCoeff1 : TRzNumericEdit ;
    Label42 : TLabel ;
    edCoeff0 : TRzNumericEdit ;
    btnCoeffCalc : TRzToolButton ;
    procedure btnPickSensorMouseDown(Sender : TObject ; Button : TMouseButton ;
      Shift : TShiftState ; X, Y : Integer) ;
    procedure frameEditorChange(Sender : TObject) ;
    procedure btnCoeffCalcClick(Sender : TObject) ;
    procedure edCoeffKeyPress(Sender : TObject ; var Key : char) ;
  private
    { Déclarations privées }
    FSensor : TSensorDescriptor ;
    FCoeffEditor : array[0..POLY_REG_ORDER] of TRzNumericEdit ;
    FDirty : Boolean ;
    FUpdating : Boolean ;
    FOnChange : TNotifyEvent ;
    procedure SetSensor(const Value : TSensorDescriptor) ;
    procedure SetSensorNum(const Value : Integer) ;
    function GetPickSensorMenu : TPopupMenu ;
    procedure SetPickSensorMenu(const Value : TPopupMenu) ;
    function GetSensorNum : Integer ;
    procedure SetOnChange(const Value : TNotifyEvent) ;

  protected
    procedure Loaded ; override ;
  public
    { Déclarations publiques }
    property Sensor : TSensorDescriptor read FSensor write SetSensor ;
    property SensorNum : Integer read GetSensorNum write SetSensorNum ;
    property PickSensorMenu : TPopupMenu read GetPickSensorMenu write SetPickSensorMenu ;
    property OnChange : TNotifyEvent read FOnChange write SetOnChange ;

    procedure Clear ;
    procedure UpdateForm ;
    procedure UpdateModel ;
  end ;

implementation

{$R *.dfm}

uses
  fSensorCalibration,
  MedusaUtils
  ;

{ TSensorEditor }

procedure TSensorEditor.Loaded ;
var
  i                 : Integer ;
begin
  inherited ;

  FDirty := False ;

  btnCoeffCalc.OnClick := btnCoeffCalcClick ;

  for i := Low(FCoeffEditor) to High(FCoeffEditor) do begin
    FCoeffEditor[i] := TRzNumericEdit(FindComponent('edCoeff' + IntToStr(i))) ;
    FCoeffEditor[i].OnKeyPress := edCoeffKeyPress ;
  end ;
end ;

procedure TSensorEditor.SetSensor(const Value : TSensorDescriptor) ;
begin
  FSensor := Value ;
  UpdateForm ;
end ;

procedure TSensorEditor.SetSensorNum(const Value : Integer) ;
begin
  Self.Tag := Value ;
  lbSensorNum.Caption := IntToStr(Value) ;
end ;

function TSensorEditor.GetSensorNum : Integer ;
begin
  Result := Self.Tag ;
end ;

procedure TSensorEditor.UpdateForm ;
var
  i                 : Integer ;
begin
  if FSensor = nil then Exit ;

  FUpdating := True ;
  try
    edName.Text := FSensor.name ;
    edUnits.Text := FSensor.units ;
    for i := Low(FCoeffEditor) to High(FCoeffEditor) do
      FCoeffEditor[i].Value := FSensor.polyCoeffs[i]

  finally
    FUpdating := False ;
    FDirty := False ;
  end ;
end ;

procedure TSensorEditor.UpdateModel ;
var
  i                 : Integer ;
begin
  if not FDirty then Exit ;

  FSensor.name := edName.Text ;
  FSensor.units := edUnits.Text ;
  for i := Low(FCoeffEditor) to High(FCoeffEditor) do
    FSensor.polyCoeffs[i] := FCoeffEditor[i].Value ;
end ;

procedure TSensorEditor.btnPickSensorMouseDown(Sender : TObject ;
  Button : TMouseButton ; Shift : TShiftState ; X, Y : Integer) ;
begin
  btnPickSensor.DropDownMenu.Tag := Integer(Self) ;
end ;

function TSensorEditor.GetPickSensorMenu : TPopupMenu ;
begin
  Result := btnPickSensor.DropDownMenu ;
end ;

procedure TSensorEditor.SetPickSensorMenu(const Value : TPopupMenu) ;
begin
  btnPickSensor.DropDownMenu := Value ;
end ;

procedure TSensorEditor.Clear ;
var
  i                 : Integer ;
begin
  FUpdating := True ;
  try
    edName.Clear ;
    edUnits.Clear ;
    for i := Low(FCoeffEditor) to High(FCoeffEditor) do
      FCoeffEditor[i].Value := 0 ;

  finally
    FUpdating := False ;
  end ;
end ;

procedure TSensorEditor.frameEditorChange(Sender : TObject) ;
var
  i                 : Integer ;
begin
  if FUpdating then Exit ;

  if (Sender = edName) and (edName.Text = '') then begin
    FUpdating := True ;
    try
      edUnits.Clear ;
      for i := Low(FCoeffEditor) to High(FCoeffEditor) do
        FCoeffEditor[i].Value := 0 ;
    finally
      FUpdating := False ;
    end ;
  end ;

  FDirty := True ;
  if Assigned(FOnChange) then FOnChange(Self) ;
end ;

procedure TSensorEditor.SetOnChange(const Value : TNotifyEvent) ;
var
  i                 : Integer ;
  ctrl              : TControl ;
begin
  FOnChange := Value ;
  for i := 0 to ControlCount - 1 do begin
    ctrl := Controls[i] ;
    if ctrl is TRzEdit then
      TRzEdit(ctrl).OnChange := frameEditorChange ;
  end ;
end ;

procedure TSensorEditor.btnCoeffCalcClick(Sender : TObject) ;
var
  dlg               : TSensorCalibration ;
  i                 : Integer ;
  polynom           : TPolynom ;
begin
  dlg := TSensorCalibration.Create(nil) ;
  try
    dlg.SetUnits(edUnits.Text) ;
    if dlg.ShowModal = mrOk then begin
      polynom := dlg.polynom ;
      for i := Low(FCoeffEditor) to High(FCoeffEditor) do
        FCoeffEditor[i].Value := polynom.coeffs[i] ;
    end ;

  finally
    dlg.Release ;
  end ;
end ;

procedure TSensorEditor.edCoeffKeyPress(Sender : TObject ; var Key : char) ;
begin
  if Key = '.' then Key := ',' ;
end ;

end.

