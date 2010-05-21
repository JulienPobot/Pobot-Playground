unit epPowerIndicator ;

interface

uses
  Windows,
  SysUtils,
  Classes,
  Controls,
  ExtCtrls,
  Graphics ;

type
  TOrientation = (orVertical, orHorizontal) ;

  TepPowerIndicator = class(TCustomPanel)
  private
    FbmOffs : TBitmap ;
    FMax : Integer ;
    FMin : Integer ;
    FOrientation : TOrientation ;
    FSegmentColorPos : TColor ;
    FSegmentColorNeg : TColor ;
    FSegmentCount : Integer ;
    FSegmentGapSize : Integer ;
    FSegmentSize : Integer ;
    FSegmentStep : Double ;
    FValue : Integer ;
    FZeroMarkerColor : TColor ;
    procedure SetOrientation(const Value : TOrientation) ;
    procedure SetSegmentColorPos(const Value : TColor) ;
    procedure SetSegmentColorNeg(const Value : TColor) ;
    procedure SetSegmentGapSize(const Value : Integer) ;
    procedure SetSegmentSize(const Value : Integer) ;
    procedure SetValue(const Value : Integer) ;
    procedure SetZeroMarkerColor(const Value : TColor) ;
    function ValueToPos(Value : Integer) : Integer ;
    procedure UpdateSegmentCount ;
    procedure UpdateSegmentStep ;
  protected
    procedure DrawBackground ;
    procedure DrawFrame ;
    procedure DrawSegments ;
    procedure DrawZeroMarker ;
    procedure Loaded ; override ;
    procedure Paint ; override ;
    procedure Resize ; override ;
  public
    constructor Create(aOwner : TComponent) ; override ;
    destructor Destroy ; override ;
  published
    property Orientation : TOrientation read FOrientation write SetOrientation default orVertical ;
    property SegmentColorPos : TColor read FSegmentColorPos write SetSegmentColorPos default clLime ;
    property SegmentColorNeg : TColor read FSegmentColorNeg write SetSegmentColorNeg default clRed ;
    property SegmentGapSize : Integer read FSegmentGapSize write SetSegmentGapSize default 2 ;
    property SegmentSize : Integer read FSegmentSize write SetSegmentSize default 3 ;
    property Value : Integer read FValue write SetValue ;
    property ZeroMarkerColor : TColor read FZeroMarkerColor write SetZeroMarkerColor default clBlue ;

    property Width ;
    property Height ;
  end ;

procedure Register ;

implementation

uses
  CSIntf,
  Types
  ,
  Math ;

const
  MIN_WIDTH         = 17 ;
  MIN_HEIGHT        = 17 ;

procedure Register ;
begin
  RegisterComponents('User Components', [TepPowerIndicator]) ;
end ;

{ TepJoystickViewer }

constructor TepPowerIndicator.Create(aOwner : TComponent) ;
begin
  inherited ;

  if aOwner is TWinControl then
    Parent := TWinControl(aOwner) ;

  FMin := -100 ;
  FMax := 100 ;

  Color := clBlack ;
  Orientation := orVertical ;
  Width := MIN_WIDTH ;
  Height := 80 ;

  Constraints.MinHeight := MIN_HEIGHT ;
  Constraints.MinWidth := MIN_WIDTH ;

  FbmOffs := TBitmap.Create() ;
  FbmOffs.Width := Self.ClientWidth ;
  FbmOffs.Height := Self.ClientHeight ;

  SegmentSize := 3 ;
  SegmentGapSize := 2 ;
  ZeroMarkerColor := clBlue ;
  SegmentColorPos := clLime ;
  SegmentColorNeg := clRed ;
end ;

destructor TepPowerIndicator.Destroy ;
begin
  FbmOffs.Free ;

  inherited ;
end ;

function TepPowerIndicator.ValueToPos(Value : Integer) : Integer ;
begin
  result := 0 ;
  Value := Value - FMin ;

  case Orientation of
    orHorizontal : begin
        result := Round(Value * FbmOffs.Width / (FMax - FMin))
      end ;
    orVertical : begin
        result := FbmOffs.Height - Round(Value * FbmOffs.Height / (FMax - FMin))
      end ;
  end ;
end ;

procedure TepPowerIndicator.DrawBackground ;
begin
  with FbmOffs.Canvas do begin
    Brush.Color := Self.Color ;
    FillRect(Rect(0, 0, FbmOffs.Width, FbmOffs.Height)) ;
  end ;
end ;

procedure TepPowerIndicator.DrawFrame ;
var
  r                 : TRect ;
begin
  with FbmOffs.Canvas do begin
    Pen.Style := psSolid ;
    r := Rect(0, 0, FbmOffs.Width, FbmOffs.Height) ;
    Frame3D(FbmOffs.Canvas, r, clBtnShadow, clBtnHighlight, 1) ;
  end ;
end ;

procedure TepPowerIndicator.UpdateSegmentCount ;
begin
  //CodeSite.SendFmtMsg('SegmentSize=%d - SegmentGapSize=%d - Height=%d', [SegmentSize, SegmentGapSize, Height]) ;

  if SegmentSize + SegmentGapSize = 0 then exit ;

  try
    case Orientation of
      orHorizontal :
        FSegmentCount := FbmOffs.Width div (SegmentSize + SegmentGapSize) ;
      orVertical :
        FSegmentCount := FbmOffs.Height div (SegmentSize + SegmentGapSize) ;
    end ;
    UpdateSegmentStep ;

  except
    on e : Exception do begin
      raise Exception.Create(Format('%s error in UpdateSegmentCount (SegmentSize=%d SegmentGapSize=%d',
        [e.Message, SegmentSize, SegmentGapSize])) ;
    end ;
  end ;
end ;

procedure TepPowerIndicator.UpdateSegmentStep ;
begin
  FSegmentStep := (FMax - FMin) / FSegmentCount ;
end ;

procedure TepPowerIndicator.DrawSegments ;
var
  r                 : TRect ;
  i                 : Integer ;
  segCount          : Integer ;
  moveDist          : Integer ;
  pos0              : Integer ;
begin
  //  CodeSite.SendFmtMsg('Value=%d - Min=%d - Max=%d - FSegmentStep=%f', [Value, FMin, FMax, FSegmentStep]) ;
  if FSegmentStep = 0 then exit ;

  with FbmOffs.Canvas do begin
    Pen.Style := psSolid ;
    Brush.Style := bsSolid ;
  end ;

  pos0 := ValueToPos(0) ;
  segCount := Round(abs(Value) / FSegmentStep) ;
  //  CodeSite.SendInteger('segCount', segCount) ;

  with FbmOffs.Canvas.Brush do
    if Value > 0 then
      Color := SegmentColorPos
    else
      Color := SegmentColorNeg ;
  with FbmOffs.Canvas do
    Pen.Color := Brush.Color ;

  case Orientation of
    orHorizontal : begin
        r := Rect(0, 2, SegmentSize, FbmOffs.Height - 2) ;
        if Value > 0 then begin
          moveDist := SegmentSize + SegmentGapSize ;
          OffsetRect(r, pos0 + 1, 0) ;
        end
        else begin
          moveDist := -(SegmentSize + SegmentGapSize) ;
          OffsetRect(r, pos0 + 2 - SegmentSize - SegmentGapSize, 0) ;
        end ;

        for i := 1 to segCount do begin
          FbmOffs.Canvas.Rectangle(r) ;
          OffsetRect(r, moveDist, 0) ;
        end ;
      end ;

    orVertical : begin
        r := Rect(2, 0, FbmOffs.Width - 2, SegmentSize) ;
        if Value > 0 then begin
          moveDist := -(SegmentSize + SegmentGapSize) ;
          OffsetRect(r, 0, FbmOffs.Height - pos0 + 1 - SegmentSize - SegmentGapSize) ;
        end
        else begin
          moveDist := SegmentSize + SegmentGapSize ;
          OffsetRect(r, 0, FbmOffs.Height - pos0 + 2) ;
        end ;

        for i := 1 to segCount do begin
          FbmOffs.Canvas.Rectangle(r) ;
          OffsetRect(r, 0, moveDist) ;
        end ;
      end ;
  end ;
end ;

procedure TepPowerIndicator.DrawZeroMarker ;
var
  xy                : Integer ;
begin
  if ZeroMarkerColor = Color then exit ;

  xy := ValueToPos(0) ;
  case Orientation of
    orHorizontal : begin
        with FbmOffs.Canvas do begin
          Pen.Style := psSolid ;
          Pen.Width := 1 ;
          Pen.Color := ZeroMarkerColor ;
          MoveTo(xy, 0) ;
          LineTo(xy, FbmOffs.Height) ;
        end ;
      end ;
    orVertical : begin
        with FbmOffs.Canvas do begin
          Pen.Style := psSolid ;
          Pen.Width := 1 ;
          Pen.Color := ZeroMarkerColor ;
          MoveTo(0, xy) ;
          LineTo(FbmOffs.Width, xy) ;
        end ;
      end ;
  end ;
end ;

procedure TepPowerIndicator.Loaded ;
begin
  inherited ;

  UpdateSegmentCount ;
end ;

procedure TepPowerIndicator.Paint ;
begin
  inherited ;

  FbmOffs.Canvas.Lock ;
  try
    DrawBackground ;
    DrawSegments ;
    DrawZeroMarker ;
    DrawFrame ;

    BitBlt(Self.Canvas.Handle, 0, 0, ClientWidth, ClientHeight,
      FbmOffs.Canvas.Handle, 0, 0, SRCCOPY) ;

  finally
    FbmOffs.Canvas.UnLock ;
  end ;
end ;

procedure TepPowerIndicator.Resize ;
begin
  inherited ;

  UpdateSegmentCount ;

  if Assigned(FbmOffs) then begin
    FbmOffs.Width := Self.ClientWidth ;
    FbmOffs.Height := Self.ClientHeight ;
  end ;
end ;

procedure TepPowerIndicator.SetOrientation(const Value : TOrientation) ;
begin
  if FOrientation <> Value then begin
    FOrientation := Value ;
    Resize ;
    Invalidate ;
  end ;
end ;

procedure TepPowerIndicator.SetSegmentColorPos(const Value : TColor) ;
begin
  if FSegmentColorPos <> Value then begin
    FSegmentColorPos := Value ;
    Invalidate ;
  end ;
end ;

procedure TepPowerIndicator.SetSegmentColorNeg(const Value : TColor) ;
begin
  if FSegmentColorNeg <> Value then begin
    FSegmentColorNeg := Value ;
    Invalidate ;
  end ;
end ;

procedure TepPowerIndicator.SetSegmentGapSize(const Value : Integer) ;
begin
  if FSegmentGapSize <> Value then begin
    if Value <= 0 then
      raise Exception.Create('Segment gap size must be > 0') ;
      
    FSegmentGapSize := Value ;
    UpdateSegmentCount ;
    Invalidate ;
  end ;
end ;

procedure TepPowerIndicator.SetSegmentSize(const Value : Integer) ;
begin
  if FSegmentSize <> Value then begin
    if Value <= 0 then
      raise Exception.Create('Segment size must be > 0') ;

    FSegmentSize := Value ;
    UpdateSegmentCount ;
    Invalidate ;
  end ;
end ;

procedure TepPowerIndicator.SetValue(const Value : Integer) ;
begin
  if FValue <> Value then begin
    if Value > FMax then
      FValue := FMax
    else if FValue < FMin then
      FValue := FMin
    else
      FValue := Value ;

    Invalidate ;
  end ;
end ;

procedure TepPowerIndicator.SetZeroMarkerColor(const Value : TColor) ;
begin
  if FZeroMarkerColor <> Value then begin
    FZeroMarkerColor := Value ;
    Invalidate ;
  end ;
end ;

end.

