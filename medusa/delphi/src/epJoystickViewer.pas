unit epJoystickViewer ;

interface

uses
  Windows,
  SysUtils,
  Classes,
  Graphics,
  Controls,
  ExtCtrls
  ;

type
  TepJoystickViewer = class(TPanel)
  private
    { Déclarations privées }
    FAxisColor : TColor ;
    FAxisStyle : TPenStyle ;
    FAxisWidth : Integer ;
    FIndicatorColor : TColor ;
    FIndicatorSize : Integer ;
    FXPosition : Integer ;
    FYPosition : Integer ;
    FbmOffs : TBitmap ;
    procedure SetAxisColor(const Value : TColor) ;
    procedure SetAxisStyle(const Value : TPenStyle) ;
    procedure SetAxisWidth(const Value : Integer) ;
    procedure SetIndicatorColor(const Value : TColor) ;
    procedure SetIndicatorSize(const Value : Integer) ;

  protected
    { Déclarations protégées }
    procedure DrawBackground;
    procedure DrawAxis;
    procedure DrawIndicator;
    procedure DrawFrame;
    procedure Paint ; override ;
    procedure Resize ; override ;

  public
    { Déclarations publiques }
    constructor Create(aOwner : TComponent) ; override ;
    destructor Destroy ; override ;

    procedure SetPosition(X : Integer ; Y : Integer) ;

  published
    { Déclarations publiées }
    property Width default 60 ;
    property Height default 60 ;
    property Top ;
    property Left ;
    property Color default clBlack ;

    property AxisColor : TColor read FAxisColor write SetAxisColor default clLime ;
    property AxisStyle : TPenStyle read FAxisStyle write SetAxisStyle default psSolid ;
    property AxisWidth : Integer read FAxisWidth write SetAxisWidth default 1 ;
    property IndicatorColor : TColor read FIndicatorColor write SetIndicatorColor default clRed ;
    property IndicatorSize : Integer read FIndicatorSize write SetIndicatorSize default 10 ;
  end ;

procedure Register ;

implementation

uses
  Types
  ;

procedure Register ;
begin
  RegisterComponents('User Components', [TepJoystickViewer]) ;
end ;

{ TepJoystickViewer }

constructor TepJoystickViewer.Create(aOwner : TComponent) ;
begin
  inherited ;

  if aOwner is TWinControl then
    Parent := TWinControl(aOwner) ;

  Color := clBlack ;
  Width := 60 ;
  Height := 60 ;
  FAxisColor := clLime ;
  FAxisWidth := 1 ;
  FAxisStyle := psDot ;
  FIndicatorColor := clRed ;
  FIndicatorSize := 10 ;

  FbmOffs := TBitmap.Create() ;
  FbmOffs.Width := Self.ClientWidth ;
  FbmOffs.Height := Self.ClientHeight ;
end ;

destructor TepJoystickViewer.Destroy ;
begin
  FbmOffs.Free ;

  inherited ;
end ;

procedure TepJoystickViewer.DrawBackground;
begin
  with FbmOffs.Canvas do begin
    Brush.Color := Self.Color ;
    FillRect(Rect(0, 0, FbmOffs.Width, FbmOffs.Height)) ;
  end ;
end ;

procedure TepJoystickViewer.DrawFrame;
var
  r                 : TRect ;
begin
  with FbmOffs.Canvas do begin
    Pen.Style := psSolid ;
    r := Rect(0, 0, FbmOffs.Width, FbmOffs.Height) ;
    Frame3D(FbmOffs.Canvas, r, clBtnShadow, clBtnHighlight, 1) ;
  end ;
end ;

procedure TepJoystickViewer.DrawAxis;
begin
  with FbmOffs.Canvas do begin
    Pen.Color := FAxisColor ;
    Pen.Width := FAxisWidth ;
    Pen.Style := FAxisStyle ;

    MoveTo(FbmOffs.Width div 2, 0) ;
    LineTo(FbmOffs.Width div 2, FbmOffs.Height - 1) ;

    MoveTo(0, FbmOffs.Height div 2) ;
    LineTo(FbmOffs.Width - 1, FbmOffs.Height div 2) ;
  end ;
end ;

procedure TepJoystickViewer.DrawIndicator;
var
  radius            : Integer ;
  r                 : TRect ;
begin
  with FbmOffs.Canvas do begin
    Brush.Color := FIndicatorColor ;
    Pen.Style := psClear ;

    radius := FIndicatorSize div 2 ;
    r := Rect(-radius, -radius, radius, radius) ;
    OffsetRect(r, FbmOffs.Width div 2 + 1 + FXPosition, FbmOffs.Height div 2 + 1 + FYPosition) ;
    Ellipse(r) ;
  end ;
end ;

procedure TepJoystickViewer.Paint ;
begin
  inherited ;

  FbmOffs.Canvas.Lock ;
  try
    DrawBackground ;
    DrawAxis ;
    DrawIndicator ;
    DrawFrame ;

    BitBlt(Self.Canvas.Handle, 0, 0, ClientWidth, ClientHeight,
      FbmOffs.Canvas.Handle, 0, 0, SRCCOPY) ;

  finally
    FbmOffs.Canvas.UnLock ;
  end ;
end ;

procedure TepJoystickViewer.Resize ;
begin
  inherited ;

  if Assigned(FbmOffs) then begin
    FbmOffs.Width := Self.ClientWidth ;
    FbmOffs.Height := Self.ClientHeight ;
  end ;
end ;

procedure TepJoystickViewer.SetAxisColor(const Value : TColor) ;
begin
  if Value <> FAxisColor then begin
    FAxisColor := Value ;
    Invalidate ;
  end ;
end ;

procedure TepJoystickViewer.SetAxisStyle(const Value : TPenStyle) ;
begin
  if FAxisStyle <> Value then begin
    FAxisStyle := Value ;
    Invalidate ;
  end ;
end ;

procedure TepJoystickViewer.SetAxisWidth(const Value : Integer) ;
begin
  if Value <> FAxisWidth then begin
    FAxisWidth := Value ;
    Invalidate ;
  end ;
end ;

procedure TepJoystickViewer.SetIndicatorColor(const Value : TColor) ;
begin
  if Value = FIndicatorColor then exit ;

  FIndicatorColor := Value ;
  Invalidate ;
end ;

procedure TepJoystickViewer.SetIndicatorSize(const Value : Integer) ;
begin
  if Value = FIndicatorSize then exit ;

  FIndicatorSize := Value ;
  Invalidate ;
end ;

procedure TepJoystickViewer.SetPosition(X, Y : Integer) ;
begin
  FXPosition := Round(ClientWidth / 200 * X) ;
  FYPosition := -Round(ClientHeight / 200 * Y) ;

  Repaint ;
end ;

end.

