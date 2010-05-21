unit Lcd99;

{  LCD display component written by Jonathan Hosking, October 2000.

   Get future component updates from the following address
   Website: http://www.the-hoskings.freeserve.co.uk/

   Send any bugs, suggestions, etc to the following Email
   Email: jonathan@the-hoskings.freeserve.co.uk

   Thanks to Jean Pierre for helping with the drawing routines
   Email: jean-pierre.cocatrix@vx.cit.alcatel.fr

   Thanks to Daniel Szasz for implementing support for colon characters
   Email: daniel@mindcti.com

   Thanks to Alan Warriner for implementing double buffering, the
   animation preview feature, and improving the animation delays
   Email: alan.warriner@bigfoot.com

   Thanks to Mike Heydon for implementing the numeric value routines
   and OnChange event.
   Email: mheydon@eoh.co.za  }

interface

uses
  {$IFDEF WIN32} Windows, {$ELSE} WinTypes, WinProcs, {$ENDIF}
  Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  Menus;

type
  TLCDScale = 1..100;
  TLCDAnimation = (anNone, anSpiral, anUp, anDown, anLeft, anRight, anRandom);
  TLCDAbout = (abNone,abAbout);
  TLCDChangeProc = procedure(Sender: TObject; OldValue, NewValue: string) of object;

  TCustomLCD99 = class(TCustomControl)
  private
    { Private declarations }
    fAbout: TLCDAbout;
    fAnimation: TLCDAnimation;
    fAnimationDelay: Integer;
    fBufferBM: TBitmap;
    fDigitNum: Integer;
    fDigitSpacing: Integer;
    fDoBuffer:Boolean;
    fDotDisplay: Boolean;
    fDotSpacing: Integer;
    fDoubleBuffer: Boolean;
    fGapX: Integer;
    fGapY: Integer;
    fIsChanging: Boolean;
    fIsPainting: Boolean;
    fOffColor: TColor;
    fOldValue: String;
    fOnChange: TLCDChangeProc;
    fOnColor: TColor;
    fPaintDuration: {$IFDEF WIN32}DWord{$ELSE}Longint{$ENDIF};
    fPreview: Boolean;
    fSegmentSize: Integer;
    fValue: String;
    fWorkCanvas: TCanvas;
    procedure SetAnimation(Val: TLCDAnimation);
    procedure SetAnimationDelay(Val: Integer);
    procedure SetDigitNum(Val: Integer);
    procedure SetDigitSpacing(Val: Integer);
    procedure SetDotDisplay(Val: Boolean);
    procedure SetDotSpacing(Val: Integer);
    procedure SetDoubleBuffer(Val: Boolean);
    procedure SetGapX(Val: Integer);
    procedure SetGapY(Val: Integer);
    procedure SetOffColor(Val: TColor);
    procedure SetOnColor(Val: TColor);
    procedure SetSegmentSize(Val: Integer);
    procedure SetPreview(Val: Boolean);
    procedure SetValue(Val: String);
    procedure ShowAbout(Val: TLCDAbout);
    procedure WMEraseBkgnd(var Message: TWMEraseBkgnd); message wm_EraseBkgnd;
  protected
    { Protected declarations }
    procedure Paint; override;
    property About: TLCDAbout read fAbout write ShowAbout default abNone;
    property Animation: TLCDAnimation read fAnimation write SetAnimation default anNone;
    property AnimationDelay: Integer read fAnimationDelay write SetAnimationDelay default 0;
    property DigitNum: Integer read fDigitNum write SetDigitNum default 1;
    property DigitSpacing: Integer read fDigitSpacing write SetDigitSpacing default 5;
    property DotDisplay: Boolean read fDotDisplay write SetDotDisplay default False;
    property DotSpacing: Integer read fDotSpacing write SetDotSpacing default 2;
    property DoubleBuffer: Boolean read fDoubleBuffer write SetDoubleBuffer default True;
    property GapX: Integer read fGapX write SetGapX default 2;
    property GapY: Integer read fGapY write SetGapY default 2;
    property IsPainting: Boolean read fIsPainting;
    property OffColor: TColor read fOffColor write SetOffColor default clBlack;
    property OnColor: TColor read fOnColor write SetOnColor default clLime;
    property PaintDuration: {$IFDEF WIN32}DWord{$ELSE}Longint{$ENDIF} read fPaintDuration;
    property Preview: Boolean read fPreview write SetPreview default False;
    property SegmentSize: Integer read fSegmentSize write SetSegmentSize default 2;
    property Value: String read fValue write SetValue;
    property OnChange: TLCDChangeProc read fOnChange write fOnChange;
  public
    { Public declarations }
    constructor Create(AOwner: TComponent); override;
    procedure SetNumericValue(NewValue: Integer);
    function GetNumericValue: Integer;
  end;

  TLCD99 = class (TCustomLCD99)
  published
    { inherited }
    property Align;
    property Color;
    property DragCursor;
    property DragMode;
    property Enabled;
    property ParentShowHint;
    property PopupMenu;
    property ShowHint;
    property Visible;
    property OnClick;
    property OnDblClick;
    property OnDragDrop;
    property OnDragOver;
    property OnEndDrag;
    property OnMouseDown;
    property OnMouseMove;
    property OnMouseUp;
    { custom }
    property About ;
    property Animation ;
    property AnimationDelay ;
    property DigitNum;
    property DigitSpacing;
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
    property Value ;
    property OnChange ;
  end ;

{$IFNDEF PACKAGE_LEVEL_REGISTRATION}
procedure Register;
{$ENDIF}

implementation

{ TLCD99 }

const
  CopyRightStr: PChar = 'TLCD Component v1.8 (15/10/2000)'+#13+#13+
    'By Jonathan Hosking'+#13+#13+
    'Compiled in '+
    {$IFDEF VER80}  'Delphi 1.0' {$ENDIF}
    {$IFDEF VER90}  'Delphi 2.0' {$ENDIF}
    {$IFDEF VER100} 'Delphi 3.0' {$ENDIF}
    {$IFDEF VER120} 'Delphi 4.0' {$ENDIF}
    {$IFDEF VER130} 'Delphi 5.0' {$ENDIF}
    {$IFDEF VER140} 'Delphi 6.0' {$ENDIF}
    {$IFDEF VER150} 'Delphi 7.0' {$ENDIF}
    {$IFDEF VER93}  'C++Builder 1.0' {$ENDIF}
    {$IFDEF VER110} 'C++Builder 3.0' {$ENDIF}
    {$IFDEF VER125} 'C++Builder 4.0' {$ENDIF};

  { ---1---    This next array stores the digits (0 - 36)
    |     |    and the segments (1 - 7).  The diagram on
    2  8  3    the left shows the layout of the segments.
    |     |    A 1 enables the segment, while a 0 will
    ---4---    turn the segment off.  An 8 will display
    |     |    a colon.
    5  8  6
    |     |
    ---7--- }

  LCDDisplayData: Array[0..37,1..8] of integer =
                    { Numbers, minus sign and colon }
                    ((1,1,1,0,1,1,1,0),(0,0,1,0,0,1,0,0),(1,0,1,1,1,0,1,0),
                    (1,0,1,1,0,1,1,0),(0,1,1,1,0,1,0,0),(1,1,0,1,0,1,1,0),
                    (1,1,0,1,1,1,1,0),(1,0,1,0,0,1,0,0),(1,1,1,1,1,1,1,0),
                    (1,1,1,1,0,1,1,0),(0,0,0,1,0,0,0,0),(0,0,0,0,0,0,0,1),
                    { Letters of the alphabet }
                    (1,1,1,1,1,1,0,0),(0,1,0,1,1,1,1,0),(1,1,0,0,1,0,1,0),
                    (0,0,1,1,1,1,1,0),(1,1,0,1,1,0,1,0),(1,1,0,1,1,0,0,0),
                    (1,1,0,1,1,1,1,0),(0,1,0,1,1,1,0,0),(0,0,1,0,0,1,0,0),
                    (0,0,1,0,0,1,1,0),(0,1,1,1,1,1,0,0),(0,1,0,0,1,0,1,0),
                    (0,0,0,1,1,1,0,0),(0,0,0,1,1,1,0,0),(0,0,0,1,1,1,1,0),
                    (1,1,1,1,1,0,0,0),(1,1,1,1,0,1,0,0),(0,0,0,1,1,0,0,0),
                    (1,1,0,1,0,1,1,0),(0,1,0,1,1,0,0,0),(0,1,1,0,1,1,1,0),
                    (0,1,1,0,1,1,1,0),(0,1,1,0,1,1,1,0),(0,1,1,1,1,1,0,0),
                    (0,1,1,1,0,1,0,0),(1,0,1,1,1,0,1,0));

  { This array stores the animation details, starting at anSpiral }
  LCDAnimationData: Array[1..5,1..8] of integer =
                      ((8,4,2,1,3,6,7,5),(7,6,5,8,4,3,2,1),
                      (1,2,3,8,4,5,6,7),(6,3,7,8,4,1,5,2),
                      (2,5,1,8,4,7,3,6));

var
  CopyRightPtr: Pointer;

{ Thanks to Mike Heydon for this routine }
procedure TCustomLCD99.SetNumericValue(NewValue: Integer);
begin
  Value := IntToStr(NewValue);
  Invalidate;
end;

{ Thanks to Mike Heydon for this routine }
function TCustomLCD99.GetNumericValue: Integer;
begin
  Result := StrToIntDef(Value, 0);
end;

constructor TCustomLCD99.Create(AOwner: TComponent);
begin
  { Setup the control }
  Inherited Create(AOwner);
  ControlStyle:=ControlStyle+[csOpaque];
  CopyRightPtr := @CopyRightStr;
  Color := clBlack;
  fAbout := abNone;
  fAnimation := anNone;
  fAnimationDelay := 0;
  fBufferBM := nil;
  fDigitNum := 4;
  fDigitSpacing := 5;
  fDotDisplay := False;
  fDotSpacing := 2;
  fDoubleBuffer := True;
  fGapX := 2;
  fGapY := 2;
  fIsChanging := False;
  fIsPainting := False;
  fOffColor := clBlack;
  fOnColor := clLime;
  fPaintDuration := 0;
  fPreview := False;
  fSegmentSize := 2;
  fOldValue := '';
  fValue := '';
  Width := 92;
  Height := 34;
end;

procedure TCustomLCD99.SetAnimation(Val: TLCDAnimation);
begin
  { The control doesn't need updating here }
  if fAnimation <> Val then
    fAnimation := Val;
end;

procedure TCustomLCD99.SetAnimationDelay(Val: Integer);
begin
  { The control doesn't need updating here }
  if fAnimationDelay <> Val then
    fAnimationDelay := Val;
end;

procedure TCustomLCD99.SetDigitNum(Val: Integer);
begin
  if fDigitNum <> Val then
  begin
    fDigitNum := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetDigitSpacing(Val: Integer);
begin
  if fDigitSpacing <> Val then
  begin
    fDigitSpacing := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetDotDisplay(Val: Boolean);
begin
  if fDotDisplay <> Val then
  begin
    fDotDisplay := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetDotSpacing(Val: Integer);
begin
  if fDotSpacing <> Val then
  begin
    fDotSpacing := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetDoubleBuffer(Val: Boolean);
begin
  if fDoubleBuffer <> Val then
  begin
    fDoubleBuffer := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetGapX(Val: Integer);
begin
  if fGapX <> Val then
  begin
    fGapX := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetGapY(Val: Integer);
begin
  if fGapY <> Val then
  begin
    fGapY := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetOffColor(Val: TColor);
begin
  if fOffColor <> Val then
  begin
    fOffColor := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetOnColor(Val: TColor);
begin
  if fOnColor <> Val then
  begin
    fOnColor := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetSegmentSize(Val: Integer);
begin
  if fSegmentSize <> Val then
  begin
    fSegmentSize := Val;
    Invalidate;
  end;
end;

procedure TCustomLCD99.SetPreview(Val: Boolean);
begin
  if fPreview <> Val then
  begin
    fPreview := Val;
    Invalidate;
  end;
end;

{ Thanks to Mike Heydon for the OnChange event code }
procedure TCustomLCD99.SetValue(Val: String);
var
  Count: Integer;
  Invalid: Boolean;
begin
  if fValue <> Val then
  begin
    { For this bit, we check the validity of the string }
    Invalid := False;
    if Val <> '' then
      for Count := 1 to length(Val) do
      begin
        Val[Count] := Upcase(Val[Count]);
        if not(((Val[Count] >= '0') and (Val[Count] <= '9')) or
          (Val[Count] = '-') or (Val[Count] = ' ') or (Val[Count] = '.') or
          ((Val[Count] >= 'A') and (Val[Count] <= 'Z')) or
          (Val[Count] = ':')) then
            Invalid := True;
      end;
    if Invalid then Val := '';
    { We don't allow decimal points on the end of the value }
    if (Val <> '') and (Val[length(Val)] = '.') then Delete(Val,length(Val),1);
    fValue := Val;
    { Trigger on change event }
    if Assigned(fOnChange) then fOnChange(Self, fValue, Val);
    { Turn on animation }
    fIsChanging:=true;
    Invalidate;
  end;
end;

procedure TCustomLCD99.ShowAbout(Val: TLCDAbout);
begin
  if fAbout <> Val then
  begin
    if Val = abNone then fAbout := Val else
    begin
      fAbout := abNone;
      MessageDlg(StrPas(CopyRightStr), mtInformation, [mbOk], 0);
    end;
    Invalidate;
  end;
end;

procedure TCustomLCD99.WMEraseBkgnd(var Message: TWMEraseBkgnd);
begin
   Message.Result := 1;
end;

{ Thanks to Alan Warriner for adding double buffering, animation
  previews, and improved animation delays }
procedure TCustomLCD99.Paint;
var
  AnimationNo, SegmentNo: Byte;
  tmp,tmp2,tmp3,tmp4: string;
  PaintStart,FirstTickCount:{$IFDEF WIN32}DWord{$ELSE}Longint{$ENDIF};
  AValue, HG, HH, HW, Spacing, tmpDelay, VG, VH, VW: Integer;

{ Draw a vertical segment - Thanks to Jean Pierre for his help }
procedure DrawVerticalSegment(StartX,StartY,XSpace,YSpace: Integer; SColor:TColor);
var
  BeginAt,Count,HalfY,NextAt,TheSpace: Integer;
  DotOk: Boolean;
  Seg: Integer;
  SegData: Array[0..7] of Integer;
begin
  with fWorkCanvas do
  begin
    Brush.Color := SColor;
    Pen.Color := SColor;
    if fDotDisplay then
    begin
      { Draw a dotted segment }
      for Count := 0 to 7 do SegData[Count] := LCDDisplayData[AValue,Count];
      Seg := SegData[SegmentNo];
      TheSpace := fSegmentSize;
      if YSpace < 0 then
        TheSpace := -(TheSpace);
      BeginAt := StartX;
      while BeginAt < (StartX + XSpace) do
      begin
        { Some dots may be shared by segments, so we shouldn't overwrite them
          by accident }
        DotOk := True;
        NextAt := BeginAt + fSegmentSize + fDotSpacing;
        if Seg = 0 then
          case SegmentNo of
            1: if ((BeginAt = StartX) and (SegData[2] = 1)) or
                 ((NextAt >= StartX + XSpace) and (SegData[3] = 1))
                   then DotOk := False;
            7: if ((BeginAt = StartX) and (SegData[5] = 1)) or
                 ((NextAt >= StartX + XSpace) and (SegData[6] = 1))
                   then DotOk := False;
        end;
        { Draw the dot }
        if DotOk then Ellipse(BeginAt,StartY,BeginAt+fSegmentSize,StartY+TheSpace);
        Inc(BeginAt,fSegmentSize + fDotSpacing);
      end;
    end
    else
    begin
      { Draw a normal segment }
      HalfY := StartY + round(YSpace/4);
      Polygon([Point(StartX,HalfY),
               Point(StartX+(HW div 2),StartY),
               Point((StartX+XSpace)-(HW div 2),StartY),
               Point(StartX+XSpace,HalfY),
               Point((StartX+XSpace)-HW,StartY+YSpace),
               Point(StartX+HW,StartY+YSpace),
               Point(StartX,HalfY)]);
    end;
  end;
end;

{ Draw the centre segment }
procedure DrawCenterSegment(StartX,StartY,XSpace,YSpace: Integer; SColor:TColor);
var
  BeginAt,Count,HalfY,NextAt: Integer;
  DotOk: Boolean;
  Seg: Integer;
  SegData: Array[0..7] of Integer;
begin
  with fWorkCanvas do
  begin
    Brush.Color := SColor;
    Pen.Color := SColor;
    if fDotDisplay then
    begin
      { Draw a dotted segment }
      for Count := 0 to 7 do SegData[Count] := LCDDisplayData[AValue,Count];
      Seg := SegData[SegmentNo];
      BeginAt := StartX;
      while BeginAt < (StartX + XSpace) do
      begin
        { Some dots may be shared by segments, so we shouldn't overwrite them
          by accident }
        DotOk := True;
        NextAt := BeginAt + fSegmentSize + fDotSpacing;
        if (Seg = 0) and (((BeginAt = StartX) and ((SegData[2] = 1) or (SegData[5] = 1))) or
          ((NextAt >= StartX + XSpace) and ((SegData[3] = 1) or (SegData[6] = 1))))
            then DotOk := False;
        { Draw the dot }
        if DotOk then Ellipse(BeginAt,StartY,BeginAt+fSegmentSize,StartY+fSegmentSize);
        Inc(BeginAt,fSegmentSize + fDotSpacing);
      end;
    end
    else
    begin
      { Draw a normal segment }
      HalfY := StartY + round(YSpace/2);
      Polygon([Point(StartX,HalfY),
               Point(StartX+HW,StartY),
               Point((StartX+XSpace)-HW,StartY),
               Point(StartX+XSpace,HalfY),
               Point((StartX+XSpace)-HW,StartY+YSpace),
               Point(StartX+HW,StartY+YSpace),
               Point(StartX,HalfY)]);
    end;
  end;
end;

{ Draw a horizontal segment - Thanks to Jean Pierre for his help }
procedure DrawHorizontalSegment(StartX,StartY,XSpace,YSpace: Integer; SColor:TColor);
var
  BeginAt,Count,HalfX,NextAt,TheSpace: Integer;
  DotOk: Boolean;
  Seg: Integer;
  SegData: Array[0..7] of Integer;
begin
  with fWorkCanvas do
  begin
    Brush.Color := SColor;
    Pen.Color := SColor;
    if fDotDisplay then
    begin
      { Draw a dotted segment }
      for Count := 0 to 7 do SegData[Count] := LCDDisplayData[AValue,Count];
      Seg := SegData[SegmentNo];
      TheSpace := fSegmentSize;
      if XSpace < 0 then
        TheSpace := -(TheSpace);
      BeginAt := StartY;
      while BeginAt < (StartY + YSpace) do
      begin
        { Some dots may be shared by segments, so we shouldn't overwrite them
          by accident }
        DotOk := True;
        NextAt := BeginAt + fSegmentSize + fDotSpacing;
        if Seg = 0 then
          case SegmentNo of
            2: if ((BeginAt = StartY) and (SegData[1] = 1)) or
                 ((NextAt >= StartY + YSpace) and ((SegData[4] = 1) or (SegData[5] = 1)))
                   then DotOk := False;
            3: if ((BeginAt = StartY) and (SegData[1] = 1)) or
                 ((NextAt >= StartY + YSpace) and ((SegData[4] = 1) or (SegData[6] = 1)))
                   then DotOk := False;
            5: if ((BeginAt = StartY) and ((SegData[2] = 1) or (SegData[4] = 1))) or
                 ((NextAt >= StartY + YSpace) and (SegData[7] = 1))
                   then DotOk := False;
            6: if ((BeginAt = StartY) and ((SegData[3] = 1) or (SegData[4] = 1))) or
                 ((NextAt >= StartY + YSpace) and (SegData[7] = 1))
                   then DotOk := False;
        end;
        { Draw the dot }
        if DotOk then Ellipse(StartX,BeginAt,StartX+TheSpace,BeginAt+fSegmentSize);
        Inc(BeginAt,fSegmentSize + fDotSpacing);
      end;
    end
    else
    begin
      { Draw a normal segment }
      HalfX := StartX + round(XSpace/4);
      Polygon([Point(HalfX,StartY),
               Point(StartX,StartY+(VH div 2)),
               Point(StartX,(StartY+YSpace)-(VH div 2)),
               Point(HalfX,StartY+YSpace),
               Point(StartX+XSpace,(StartY+YSpace)-VH),
               Point(StartX+XSpace,StartY+VH),
               Point(HalfX,StartY)]);
    end;
  end;
end;

{ Draw a colon - Thanks to Daniel Szasz for his help }
procedure DrawColon(StartX1,StartY1,StartX2,StartY2,XSpace,YSpace: Integer; SColor:TColor);
begin
  with fWorkCanvas do
  begin
    Brush.Color := SColor;
    Pen.Color := SColor;
    Ellipse(StartX1,StartY1,StartX1+XSpace,StartY1+YSpace);
    Ellipse(StartX2,StartY2,StartX2+XSpace,StartY2+YSpace);
  end;
end;

{ And here's the clever procedure that draws the digits WITHOUT using
  bitmaps! }
procedure DrawDigit(Animation, Speed: Integer;SkipSome:Boolean);
var
  AnimationCount: Byte;
  CH, CW, DelayCorrection, DigitNumber, DrawX, DrawY, SegmentSpaceX,
    SegmentSpaceY, Temp: Integer;
  SColor: TColor;
  DigitOn: Boolean;
  SegmentDelay,DelayTicks,FirstSegmentTickCount:{$IFDEF WIN32}DWord{$ELSE}Longint{$ENDIF};
begin
  with fWorkCanvas do
  begin
    { We start counting the whole delay here, as time can be wasted
      drawing the display.  A delay of 1000 MUST last around 1 second }
    { Work out segment sizes }
    Spacing := fDigitSpacing + Integer(Not fDotDisplay);
    { Just to ensure that everything is drawn.  The canvas doesn't always
      draw along the edges, rather irritatingly.  We also need to give an
      extra space for the left decimal point }
    CH := Height - 3; // EP add a small space on top and bottom (was - 1)
    CW := Width - 1 - Spacing;
    { Prepare to draw }
    Brush.Style := bsSolid;
    AnimationCount := 0;
    if fDotDisplay then
    begin
      { Each dotted segments must contain the same number of dots horizontally
        and vertically as the other segments.  So here we do some (very)
        mind-boggling calculations - how I achieved this I will never know!!! }
      Temp := (((CW + Spacing) div fDigitNum) - Spacing + fDotSpacing)
        div (fSegmentSize + fDotSpacing);
      SegmentSpaceX := Temp * (fSegmentSize + fDotSpacing) - fDotSpacing;
      Temp := ((CH - fSegmentSize) div 2) div (fSegmentSize + fDotSpacing) ;
      SegmentSpaceY := ((Temp * (fSegmentSize + fDotSpacing)) * 2) + fSegmentSize ;
    end
    else
    begin
      { Normal segments aren't dotted, so we do a simple divide }
      SegmentSpaceX := (CW - (fDigitNum * Spacing)) div fDigitNum;
      SegmentSpaceY := CH ;
    end;
    { Each segment needs a corner gap.  We use this to work out the segment
      height and width.  These next variables are named as follows: -

      H- = Horizontal Segment, V- = Vertical Segment
      -G = Corner gap, -H = Height, -W = Width }
    if fDotDisplay then
    begin
      { Dotted segments don't use gaps }
      HG := 0;
      VG := 0;
    end
    else
    begin
      { Normal segments do use gaps }
      HG := fGapY;
      VG := fGapX;
    end;
    VH := fSegmentSize ; 
    HW := fSegmentSize ;
    VW := SegmentSpaceX - (2 * VG);
    HH := (SegmentSpaceY div 2) - (2 * HG);
    { Draw the decimal points }
    for DigitNumber := 1 to fDigitNum do
      if (tmp3[DigitNumber] <> tmp4[DigitNumber]) or (not SkipSome) then
      begin
        if tmp3[DigitNumber] = '.' then
        begin
          Brush.Color := fOnColor;
          Pen.Color := fOnColor;
        end
        else
        begin
          Brush.Color := fOffColor;
          Pen.Color := fOffColor;
        end;
        { Make sure the decimal point size matches the segment size }
        if fSegmentSize > (Spacing - 2) then Temp := Spacing - 2
          else Temp := fSegmentSize;
        DrawX := (((DigitNumber - 1) * (SegmentSpaceX + Spacing)) + Spacing)
          - ((Spacing + Temp) div 2);
        if fDotDisplay then
          Ellipse(DrawX,SegmentSpaceY - Temp,DrawX + Temp,SegmentSpaceY)
        else
          Rectangle(DrawX,SegmentSpaceY - Temp,DrawX + Temp,SegmentSpaceY);
      end;
    { Here we see how much delay time is left }
    Speed := (Speed - Integer(GetTickCount - FirstTickCount)) div 7;
    if Speed < 0 then Speed := 0;
    DelayCorrection := 0;
    repeat
      { Now we start the delay count.  The reason for this is that some
        of the delay time can be spent updating the actual segments, so
        a delay of 1000 MUST last 1 second.  The delay is equally divided
        by 8, as there are 8 segments to update }
      FirstSegmentTickCount := GetTickCount;
      { Continue the animation }
      Inc(AnimationCount);
      if Animation <> 0 then
        SegmentNo := LCDAnimationData[Animation,AnimationCount]
      else
        SegmentNo := LCDAnimationData[3,AnimationCount];
      for DigitNumber := 1 to fDigitNum do
        if (tmp[DigitNumber] <> tmp2[DigitNumber]) or (not SkipSome) then
        begin
          { Reset the digit index }
          AValue := 8;
          { Get the current digit details }
          if tmp[DigitNumber] = ' ' then
          begin
            AValue := 8;
            DigitOn := False;
          end
          else
          begin
            { Convert the letters, numbers and minus sign
              to the correct digit index }
            case tmp[DigitNumber] of
              '-': AValue := 10;
              ':': AValue := 11;
              '0'..'9': AValue := StrToInt(tmp[DigitNumber]);
              'A'..'Z': AValue := Ord(tmp[DigitNumber]) - 53;
            end;
            DigitOn := true;
          end;
          { Set the color }
          if (DigitOn) and (LCDDisplayData[AValue,SegmentNo] = 1) then
            SColor := fOnColor
          else
            SColor := fOffColor;
          { Now we set the positions and draw the segment }
          DrawX := Spacing + ((DigitNumber - 1) * (SegmentSpaceX + Spacing));
          case SegmentNo of
            1,7: begin
                   { Top and bottom segments }
                   Inc(DrawX,VG);
                   if SegmentNo = 1 then
                   begin
                     DrawY := 0;
                     DrawVerticalSegment(DrawX,DrawY+1,VW,VH,SColor); // EP
                   end
                   else
                   begin
                     DrawY := SegmentSpaceY;
                     DrawVerticalSegment(DrawX,DrawY+1,VW,-(VH),SColor); // EP
                   end;
                 end;
            4: begin
                 { Centre segment }
                 Inc(DrawX,VG);
                 DrawY := (SegmentSpaceY div 2) - (VH div 2);
                 DrawCenterSegment(DrawX,DrawY+1,VW,VH,SColor); // EP
               end;
            2,5: begin
                   { Left segments }
                   if SegmentNo = 2 then DrawY := HG else
                   begin
                     if fDotDisplay then
                       DrawY := (SegmentSpaceY div 2) - (VH div 2)
                     else
                       DrawY := (SegmentSpaceY div 2) + HG ;
                   end;
                   DrawHorizontalSegment(DrawX,DrawY+1,HW,HH,SColor); // EP +1 added
                 end;
            3,6: begin
                   { Right segments }
                   Inc(DrawX,SegmentSpaceX);
                   if SegmentNo = 3 then DrawY := HG else
                   begin
                     if fDotDisplay then
                       DrawY := (SegmentSpaceY div 2) - (VH div 2)
                     else
                       DrawY := (SegmentSpaceY div 2) + HG;
                   end;
                   DrawHorizontalSegment(DrawX,DrawY+1,-(HW),HH,SColor); // EP +1 added
                 end;
            8: begin
                 { Colon }
                 Inc(DrawX,(SegmentSpaceX - (VW div 3)) div 2);
                 DrawColon(DrawX,(SegmentSpaceY div 2)-((HH div 3) * 2) + 1, // EP +1 added
                             DrawX,(SegmentSpaceY div 2)+(HH div 3),
                             (VW div 3),(HH div 3),SColor);
               end;
          end;
        end;
      { Now we wait for the rest of the delay to complete if there is
        an animation }
      if (Animation <> 0) and (Speed>0) then
        begin
         { Draw what we've achieved so far if double buffering }
         if fDoBuffer then
           Canvas.CopyRect(ClientRect,fWorkCanvas,ClientRect);
         { Delay & process messages up until last segment }
         if AnimationCount < 8 then
         begin
           SegmentDelay:= Speed - DelayCorrection;
           repeat
             { Process messages if not in design mode }
             if not (csDesigning in ComponentState) then
               Application.ProcessMessages;
             DelayTicks := GetTickCount - FirstSegmentTickCount;
           until DelayTicks >= SegmentDelay;
           { Trim back delay speed to compensate for over long delays }
           DelayCorrection := DelayTicks-SegmentDelay;
           if DelayCorrection > Speed then
              DelayCorrection := Speed;
          end;
        end
    until AnimationCount = 8;
  end;
end;

procedure SplitValue(AValue: String; var Value, Dots: String);
var
  Count: Integer;
  Dot: Boolean;
begin
  Count := 1;
  Value := '';
  Dots := '';
  Dot := False;
  while Count <= length(AValue) do
  begin
    if AValue[Count] <> '.' then
    begin
      if not Dot then
        Dots := Dots + ' '
      else
        Dot := False;
      Value := Value + AValue[Count];
    end
    else
    begin
      Dots := Dots + '.';
      if Dot then
        Value := Value + ' '
      else
        Dot := True;
    end;
    Inc(Count);
  end;
end;

begin
  { Exit if the control is already painting }
  if fIsPainting then Exit;
  { Set the painting flag }
  fIsPainting := True;
  FirstTickCount := GetTickCount;
  PaintStart := GetTickCount;
  { Set working canvas to default; }
  fWorkCanvas := Canvas;
  { Get double buffer status }
  fDoBuffer := fDoubleBuffer;
  { Disable animation in design mode if preview off }
  if csDesigning in ComponentState then
    fIsChanging:=fPreview;
  { Attempt to create bitmap for double buffer if required }
  if fDoBuffer then
  begin
    try
      if fBufferBM = nil then
        fBufferBM := TBitMap.Create;
      { Set working canvas to bitmap }
      fWorkCanvas := fBufferBM.Canvas;
      { Set bitmap size to match client area }
      fBufferBM.Width := ClientWidth;
      fBufferBM.Height := ClientHeight;
    except
      { Set to normal draw if an error occurs }
      fBufferBM.Free;
      fBufferBM := nil;
      fDoBuffer := False;
      fWorkCanvas := Canvas;
    end;
  end;
  with fWorkCanvas do
  begin
    { Fill control background }
    Brush.Color := Color;
    Brush.Style := bsSolid;
    FillRect(ClientRect);
    { Select the animation to use }
    AnimationNo := 0;
    case fAnimation of
      anSpiral: AnimationNo := 1;
      anUp: AnimationNo := 2;
      anDown: AnimationNo := 3;
      anLeft: AnimationNo := 4;
      anRight: AnimationNo := 5;
      anRandom: AnimationNo := Random(5)+1;
    end;
    { Update the display.  Tmp is the used string, which is
      compared with tmp2 }
    if fValue = '' then
    begin
      { Clear the display }
      SplitValue('',tmp,tmp3);
      SplitValue(fOldValue,tmp2,tmp4);
      while length(tmp) < fDigitNum do tmp := ' ' + tmp;
      while length(tmp2) < fDigitNum do tmp2 := ' ' + tmp2;
      while length(tmp3) < fDigitNum do tmp3 := ' ' + tmp3;
      while length(tmp4) < fDigitNum do tmp4 := ' ' + tmp4;
      if fIsChanging then
        DrawDigit(AnimationNo,fAnimationDelay,False)
      else
        DrawDigit(0,0,False);
    end
    else
    begin
      { We start counting the whole delay here, as time can be wasted
        drawing the display.  A delay of 1000 MUST last around 1 second }
      { Here, we draw over the old value, but first we need
        to quickly redraw the old value incase the component
        is blank and is using an animation }
      SplitValue(fOldValue,tmp,tmp3);
      SplitValue('',tmp2,tmp4);
      while length(tmp) < fDigitNum do tmp := ' ' + tmp;
      while length(tmp2) < fDigitNum do tmp2 := ' ' + tmp2;
      while length(tmp3) < fDigitNum do tmp3 := ' ' + tmp3;
      while length(tmp4) < fDigitNum do tmp4 := ' ' + tmp4;
      DrawDigit(0,0,False);
      { Now draw the new value }
      SplitValue(fValue,tmp,tmp3);
      SplitValue(fOldValue,tmp2,tmp4);
      while length(tmp) < fDigitNum do tmp := ' ' + tmp;
      while length(tmp2) < fDigitNum do tmp2 := ' ' + tmp2;
      while length(tmp3) < fDigitNum do tmp3 := ' ' + tmp3;
      while length(tmp4) < fDigitNum do tmp4 := ' ' + tmp4;
      { Here we see how much delay time is left }
      tmpDelay := fAnimationDelay - Integer(GetTickCount-FirstTickCount);
      if tmpDelay < 0 then tmpDelay := 0;
      if fIsChanging then
        DrawDigit(AnimationNo,tmpDelay,True)
      else
        DrawDigit(0,0,True);
    end;
    { Store the value that we just used }
    if fValue = '' then fOldValue := '' else
      fOldValue := fValue;
  end;
  { Copy from buffer to screen & free memory if double buffering }
  if fDoBuffer then
  begin
     Canvas.CopyRect(ClientRect,fWorkCanvas,ClientRect);
     { Get rid of bitmap }
     fBufferBM.Free;
     fBufferBM := nil;
  end;
  { Disable animation }
  fIsChanging:=false;
  { Allow drawing }
  fIsPainting:=false;
  { Set paint duration value }
  fPaintDuration:=GetTickCount - PaintStart;
end;

{$IFNDEF PACKAGE_LEVEL_REGISTRATION}
procedure Register;
begin
  RegisterComponents('User components', [TLCD99]);
end;
{$ENDIF}

end.


