unit fMapDisplay ;

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
  ExtCtrls,
  StdCtrls ;

type
  TForm1 = class(TForm)
    ScrollBox1 : TScrollBox ;
    Image1 : TImage ;
    Image2 : TImage ;
    ScrollBar1 : TScrollBar ;
    ScrollBar2 : TScrollBar ;
    Timer1 : TTimer ;
    procedure FormShow(Sender : TObject) ;
    procedure ScrollBar2Change(Sender : TObject) ;
    procedure ScrollBar1Change(Sender : TObject) ;
    procedure Timer1Timer(Sender : TObject) ;
  private
    { Déclarations privées }
  public
    { Déclarations publiques }
  end ;

var
  Form1             : TForm1 ;

implementation

{$R *.dfm}

procedure TForm1.FormShow(Sender : TObject) ;
var
  ImagePath         : string ;
begin
  ImagePath := ExtractFilePath(Application.ExeName) + 'map.png' ;
  Image1.Picture.LoadFromFile(ImagePath) ;
  ScrollBar1.Max := Image1.Picture.Width ;
  ScrollBar2.Max := Image1.Picture.Height ;
  ScrollBar1Change(nil) ;
  ScrollBar2Change(nil) ;
end ;

procedure TForm1.ScrollBar2Change(Sender : TObject) ;
begin
  Image2.Top := ScrollBar2.Position - Image2.Height div 2 ;
end ;

procedure TForm1.ScrollBar1Change(Sender : TObject) ;
begin
  Image2.Left := ScrollBar1.Position - Image2.Width div 2 ;
end ;

procedure TForm1.Timer1Timer(Sender : TObject) ;
begin
  Image2.Visible := not Image2.Visible ;
end ;

end.

