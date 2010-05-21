unit fAboutMissionEditor ;

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
  PngImage1,
  jpeg,
  ExtCtrls,
  VrControls,
  VrDeskTop,
  StdCtrls ;

type
  TAboutMissionEditor = class(TForm)
    Panel1 : TPanel ;
    Image1 : TImage ;
    imgActiveArea : TImage ;
    Image5 : TImage ;
    VrDeskTop1 : TVrDeskTop ;
    Label1 : TLabel ;
    Label2 : TLabel ;
    Image3 : TImage ;
    Image6 : TImage ;
    procedure imgActiveAreaClick(Sender : TObject) ;
    procedure FormKeyPress(Sender : TObject ; var Key : Char) ;
    procedure FormCreate(Sender : TObject) ;
  private
    { Déclarations privées }
  public
    { Déclarations publiques }
    class procedure Display ;
  end ;

var
  AboutMissionEditor             : TAboutMissionEditor ;

implementation

{$R *.dfm}

{ TAbout }

class procedure TAboutMissionEditor.Display ;
begin
  with TAboutMissionEditor.Create(nil) do try
    ShowModal ;
  finally
    Release ;
  end ;
end ;

procedure TAboutMissionEditor.imgActiveAreaClick(Sender : TObject) ;
begin
  ModalResult := mrOk ;
end ;

procedure TAboutMissionEditor.FormKeyPress(Sender : TObject ; var Key : Char) ;
begin
  ModalResult := mrOk ;
end ;

procedure TAboutMissionEditor.FormCreate(Sender : TObject) ;
begin
  imgActiveArea.Align := alClient ;
end ;

end.

