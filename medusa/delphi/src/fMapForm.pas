unit fMapForm ;

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
  ExtCtrls,
  StdCtrls,
  RzStatus,
  RzPanel ;

type
  TMapDisplay = class(TForm)
    ScrollBox1 : TScrollBox ;
    imgMap : TImage ;
    imgMarker : TImage ;
    tmrBlink : TTimer ;
    RzStatusBar1 : TRzStatusBar ;
    spGeomPos : TRzStatusPane ;
    procedure tmrBlinkTimer(Sender : TObject) ;
    procedure sbVerticalChange(Sender : TObject) ;
    procedure sbHorizontalChange(Sender : TObject) ;
    procedure FormShow(Sender : TObject) ;
    procedure FormClose(Sender : TObject ; var Action : TCloseAction) ;
    procedure imgMapMouseMove(Sender : TObject ; Shift : TShiftState ; X,
      Y : Integer) ;
  private
    { Déclarations privées }
    FMapFile : string ;
  public
    { Déclarations publiques }
    class procedure ShowWindow ;
    class function Setup(const usrConfigPath : string) : boolean ;
  end ;

var
  MapDisplay        : TMapDisplay ;

implementation

uses
  IniFiles
  ;

{$R *.dfm}

const
  SECT_MAP          = 'carte' ;
  VAR_MAP_FILE      = 'fichier' ;

class function TMapDisplay.Setup(const usrConfigPath : string) : boolean ;
var
  cfgFile           : TIniFile ;
  mapFile           : string ;
begin
  if MapDisplay <> nil then MapDisplay.Release ;
  Result := false ;

  cfgFile := TIniFile.Create(usrConfigPath) ;
  try
    if not cfgFile.SectionExists(SECT_MAP) then Exit ;

    mapFile := cfgFile.ReadString(SECT_MAP, VAR_MAP_FILE, '') ;
    if mapFile = '' then Exit ;
    if not FileExists(mapFile) then raise EResNotFound.Create(mapFile) ;

    MapDisplay := TMapDisplay.Create(nil) ;
    MapDisplay.FMapFile := mapFile ;

    Result := true ;

  finally
    cfgFile.Free ;
  end ;
end ;

class procedure TMapDisplay.ShowWindow ;
begin
  if MapDisplay <> nil then begin
    MapDisplay.Show ;
    MapDisplay.BringToFront ;
  end ;
end ;

procedure TMapDisplay.tmrBlinkTimer(Sender : TObject) ;
begin
  imgMarker.Visible := not imgMarker.Visible ;
end ;

procedure TMapDisplay.sbVerticalChange(Sender : TObject) ;
begin
  imgMap.Top := (Sender as TScrollBar).Position - imgMap.Height div 2 ;
end ;

procedure TMapDisplay.sbHorizontalChange(Sender : TObject) ;
begin
  imgMap.Left := (Sender as TScrollBar).Position - imgMap.Width div 2 ;
end ;

procedure TMapDisplay.FormShow(Sender : TObject) ;
begin
  if FMapFile <> '' then imgMap.Picture.LoadFromFile(FMapFile) ;
end ;

procedure TMapDisplay.FormClose(Sender : TObject ; var Action : TCloseAction) ;
begin
  Self.Hide ;
end ;

procedure TMapDisplay.imgMapMouseMove(Sender : TObject ; Shift : TShiftState ;
  X, Y : Integer) ;
begin
  spGeomPos.Caption := Format('X=%d - Y=%d', [X, Y]) ;
end ;

end.

