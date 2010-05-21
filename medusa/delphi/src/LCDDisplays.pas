{ Collection de composants dérivés de Lcd99 écrit par Jonathan Hosking,
  implémentant :
   - un afficheur de valeurs réelles
   - une horloge
   - un chronomètre

  Auteur: Eric PASCUAL

  $Revision: 1.1 $

  $Date: 2009/10/31 17:35:25 $
   }

unit LCDDisplays ;

interface

uses
  Classes,
  Lcd99,
  ExtCtrls ;

type
  { Format d'affichage de l'horloge :
    <table>
      dtDate           date courante
      dtTime           heure courante
      dtDateTime       date et heure courantes
    </table>
  }
  TDisplayType = (dtDate, dtTime, dtDateTime) ;

  { Horloge LCD.

    <IMAGE TLCDClock>

    Remarque:
    L'horloge peut utiliser un timer externe. Si aucun n'y est connecté,
    un timer interne est automatiquement créé. Ce timer est public et peut
    être utilisé extérieurement.
  }
  TLCDClock = class(TCustomLCD99)
  private
    { Déclarations privées }
    FTimer : TTimer ;
    FDisplayType : TDisplayType ;
    procedure FTimerTimer(Sender : TObject) ;
    procedure SetDisplayType(const Value : TDisplayType) ;
  protected
    { Déclarations protégées }
    procedure UpdateDisplay ; virtual ;
  public
    constructor Create(AOwner : TComponent) ; override ;
    procedure Loaded ; override ;
  published
    { inherited }
    property Align ;
    property Color ;
    property DragCursor ;
    property DragMode ;
    property Enabled ;
    property ParentShowHint ;
    property PopupMenu ;
    property ShowHint ;
    property Visible ;
    property OnClick ;
    property OnDblClick ;
    property OnDragDrop ;
    property OnDragOver ;
    property OnEndDrag ;
    property OnMouseDown ;
    property OnMouseMove ;
    property OnMouseUp ;
    { custom }
    property About ;
    property Animation ;
    property AnimationDelay ;
    property DigitNum ;
    property DigitSpacing ;
    // Format d'affichage de l'horloge
    property DisplayType : TDisplayType read FDisplayType write SetDisplayType ;
    property DotDisplay ;
    property DotSpacing ;
    property DoubleBuffer ;
    property GapX ;
    property GapY ;
    property IsPainting ;
    property OffColor ;
    property OnColor ;
    property PaintDuration ;
    property Preview ;
    property SegmentSize ;
    property OnChange ;
    // Timer
    property Timer : TTimer read FTimer write FTimer ;
  end ;

  { Chronomètre.

    <IMAGE TLCDStopWatch>

    Une forme d'horloge spécifique, avec les fonctions habituelles d'un
    chronomètre (start, stop, reset, pause)
  }
  TLCDStopWatch = class(TCustomLCD99)
  private
    { Déclarations privées }
    FTimer : TTimer ;
    FStartTime : TDateTime ;
    FAccum : TDateTime ;
    FElapsed : TDateTime ;
    procedure FTimerTimer(Sender : TObject) ;
    procedure UpdateDisplay ; virtual ;
    procedure SetStartTime(const Value : TDateTime) ;
  protected
    { Déclarations protégées }
  public
    // Temps écoulé
    property Elapsed : TDateTime read FElapsed ;

    property StartTime : TDateTime read FStartTime write SetStartTime ;

    constructor Create(AOwner : TComponent) ; override ;
    procedure Loaded ; override ;

    // Démarre du chrono
    procedure Start ;
    // Arrête le chrono
    procedure Stop ;
    // Remet le chrono à 0
    procedure Reset ;

    function IsRunning : Boolean ;

  published
    { inherited }
    property Align ;
    property Color ;
    property DragCursor ;
    property DragMode ;
    property Enabled ;
    property ParentShowHint ;
    property PopupMenu ;
    property ShowHint ;
    property Visible ;
    property OnClick ;
    property OnDblClick ;
    property OnDragDrop ;
    property OnDragOver ;
    property OnEndDrag ;
    property OnMouseDown ;
    property OnMouseMove ;
    property OnMouseUp ;
    { custom }
    property About ;
    property Animation ;
    property AnimationDelay ;
    property DigitNum ;
    property DigitSpacing ;
    property DotDisplay ;
    property DotSpacing ;
    property DoubleBuffer ;
    property GapX ;
    property GapY ;
    property IsPainting ;
    property OffColor ;
    property OnColor ;
    property PaintDuration ;
    property Preview ;
    property SegmentSize ;
    property OnChange ;
  end ;

  { Afficheur LCD pour valeurs réelles.

    <IMAGE TLCDFloat>

    Cet afficheur fonctionne en virgule fixe et inclu le paramétrage du
    format à utiliser (nombre de digits, nombre de décimales)
  }
  TLCDFloat = class(TCustomLCD99)
  private
    FPrecision : Integer ;
    FValueFloat : Single ;
    procedure SetPrecision(const Value : Integer) ;
    procedure SetValueFloat(const Value : Single) ;
    procedure UpdateDisplay ;
  public
    constructor Create(AOwner : TComponent) ; override ;
  published
    property Align ;
    property Color ;
    property DragCursor ;
    property DragMode ;
    property Enabled ;
    property ParentShowHint ;
    property PopupMenu ;
    property ShowHint ;
    property Visible ;
    property OnClick ;
    property OnDblClick ;
    property OnDragDrop ;
    property OnDragOver ;
    property OnEndDrag ;
    property OnMouseDown ;
    property OnMouseMove ;
    property OnMouseUp ;

    property About ;
    property Animation ;
    property AnimationDelay ;
    property DigitNum ;
    property DigitSpacing ;
    property DotDisplay ;
    property DotSpacing ;
    property DoubleBuffer ;
    property GapX ;
    property GapY ;
    property IsPainting ;
    property OffColor ;
    property OnColor ;
    property PaintDuration ;
    property Preview ;
    property SegmentSize ;
    property OnChange ;

    // Précision d'affichage (nombre de décimales)
    property Precision : Integer read FPrecision write SetPrecision ;
    // Valeur affichée
    property Value : Single read FValueFloat write SetValueFloat ;
  end ;

{$IFNDEF PACKAGE_LEVEL_REGISTRATION}
procedure Register ;
{$ENDIF}

// ============================================================================
implementation

uses
  SysUtils,
  Dialogs,
  Graphics
  ;

{$IFNDEF PACKAGE_LEVEL_REGISTRATION}

//******************* Register

procedure Register ;
begin
  RegisterComponents('User components', [TLCDClock, TLCDStopWatch, TLCDFloat]) ;
end ;
{$ENDIF}

{ TLCDClock }

const
  DfltDigitWidth    = 12 ;
  DigitNums         : array[TDisplayType] of Integer = (10, 8, 19) ;
  // ----------------------------------------------------------------------------

//******************* TLCDClock.Create

constructor TLCDClock.Create(AOwner : TComponent) ;
begin
  inherited ;
  Color := clTeal ;
  OffColor := Color ;
  OnColor := clAqua ;
  SegmentSize := 1 ;
  GapX := 1 ;
  GapY := 1 ;
  DigitSpacing := 2 ;
  DisplayType := dtDateTime ;
  DigitNum := DigitNums[DisplayType] ;
  Width := DfltDigitWidth * DigitNum + 4 ;
  Height := 22 ;
end ;

//******************* TLCDClock.FTimerTimer

procedure TLCDClock.FTimerTimer(Sender : TObject) ;
begin
  UpdateDisplay ;
end ;

//******************* TLCDClock.Loaded

procedure TLCDClock.Loaded ;
begin
  inherited ;
  if csDesigning in ComponentState then
    UpdateDisplay
  else if not Assigned(FTimer) then begin
    FTimer := TTimer.Create(Self) ;
    FTimer.Interval := 1000 ;
    FTimer.OnTimer := FTimerTimer ;
    FTimerTimer(Self) ;
  end ;
end ;

//******************* TLCDClock.SetDisplayType

procedure TLCDClock.SetDisplayType(const Value : TDisplayType) ;
var
  CurDigitWidth     : Integer ;
begin
  if (Value <> FDisplayType) then begin
    CurDigitWidth := (Self.Width - 4) div DigitNums[FDisplayType] ;
    FDisplayType := Value ;
    DigitNum := DigitNums[FDisplayType] ;
    Self.Width := DigitNum * CurDigitWidth + 4 ;
    UpdateDisplay ;
  end ;
end ;

//******************* TLCDClock.UpdateDisplay

procedure TLCDClock.UpdateDisplay ;
const
  FormatStrings     : array[TDisplayType] of string
                    = ('dd-mm-yyyy', 'hh:nn:ss', 'dd-mm-yyyy hh:nn:ss') ;
begin
  Self.Value := FormatDateTime(FormatStrings[DisplayType], Now) ;
end ;

// ----------------------------------------------------------------------------
{ TLCDStopWatch }
// ----------------------------------------------------------------------------

//******************* TLCDStopWatch.Create

constructor TLCDStopWatch.Create(AOwner : TComponent) ;
begin
  inherited ;
  Color := clTeal ;
  OffColor := Color ;
  OnColor := clAqua ;
  SegmentSize := 1 ;
  GapX := 1 ;
  GapY := 1 ;
  DigitSpacing := 2 ;
  DigitNum := 11 ;                      // 00 00:00:00
  Width := DfltDigitWidth * DigitNum + 4 ;
  Height := 22 ;
end ;

//******************* TLCDStopWatch.FTimerTimer

procedure TLCDStopWatch.FTimerTimer(Sender : TObject) ;
begin
  FElapsed := Now - FStartTime + FAccum ;
  UpdateDisplay ;
end ;

//******************* TLCDStopWatch.IsRunning

function TLCDStopWatch.IsRunning : Boolean ;
begin
  Result := FTimer.Enabled ;
end ;

//******************* TLCDStopWatch.Loaded

procedure TLCDStopWatch.Loaded ;
begin
  inherited ;
  if not (csDesigning in ComponentState) then begin
    FTimer := TTimer.Create(Self) ;
    FTimer.Interval := 1000 ;
    FTimer.Enabled := False ;
    FTimer.OnTimer := FTimerTimer ;
  end ;
  Self.Value := '00 00:00:00' ;
end ;

//******************* TLCDStopWatch.Reset

procedure TLCDStopWatch.Reset ;
begin
  FStartTime := 0 ;
  FElapsed := 0 ;
  FAccum := 0 ;
  UpdateDisplay ;
end ;

//******************* TLCDStopWatch.SetStartTime

procedure TLCDStopWatch.SetStartTime(const Value : TDateTime) ;
begin
  FStartTime := Value ;
  FAccum := 0 ;
end ;

//******************* TLCDStopWatch.Start

procedure TLCDStopWatch.Start ;
begin
  FAccum := FElapsed ;
  FStartTime := Now ;
  FTimer.Enabled := True ;
end ;

//******************* TLCDStopWatch.Stop

procedure TLCDStopWatch.Stop ;
begin
  FTimer.Enabled := False ;
end ;

//******************* TLCDStopWatch.UpdateDisplay

procedure TLCDStopWatch.UpdateDisplay ;
begin
  Self.Value :=
    Format('%.2d ', [Trunc(FElapsed)]) +
  FormatDateTime('hh:nn:ss', Frac(FElapsed)) ;
end ;

// ----------------------------------------------------------------------------
{ TLCDFloat }
// ----------------------------------------------------------------------------

//******************* TLCDFloat.Create

constructor TLCDFloat.Create(AOwner : TComponent) ;
begin
  inherited ;
  Precision := 1 ;
  GapX := 1 ;
  GapY := 1 ;
  DigitSpacing := 3 ;
end ;

//******************* TLCDFloat.SetPrecision

procedure TLCDFloat.SetPrecision(const Value : Integer) ;
resourcestring
  sErrPrecision     = 'Precision must be between 0 and DigitNum-1' ;
begin
  if Value <> FPrecision then begin
    if not (Value in [0..DigitNum - 1]) then begin
      ShowMessage(sErrPrecision) ;
    end
    else begin
      FPrecision := Value ;
      UpdateDisplay ;
    end ;
  end ;
end ;

//******************* TLCDFloat.SetValueFloat

procedure TLCDFloat.SetValueFloat(const Value : Single) ;
begin
  if Value <> FValueFloat then begin
    FValueFloat := Value ;
    UpdateDisplay ;
  end ;
end ;

//******************* TLCDFloat.UpdateDisplay

procedure TLCDFloat.UpdateDisplay ;
var
  digCnt            : Cardinal ;
  fmt               : string ;
  sVal              : string ;
  p                 : Integer ;
begin
  if Precision = 0 then
    digCnt := DigitNum
  else
    digCnt := DigitNum + 1 ;
  fmt := '%' + IntToStr(digCnt) + '.' + IntToStr(Precision) + 'f' ;
  sVal := Format(fmt, [FValueFloat]) ;
  if Length(sVal) > DigitNum + Ord(Precision > 0) then
    sVal := StringOfChar('-', DigitNum)
  else begin
    p := Pos(',', sVal) ;
    if p > 0 then sVal[p] := '.' ;
  end ;
  inherited Value := sVal ;
end ;

end.

