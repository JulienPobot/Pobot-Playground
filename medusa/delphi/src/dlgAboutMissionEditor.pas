unit dlgAboutMissionEditor;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, RzButton, StdCtrls;

type
  TdlgAbout = class(TForm)
    Label1: TLabel;
    btnOk: TRzButton;
  private
    { D�clarations priv�es }
  public
    { D�clarations publiques }
  end;

var
  dlgAbout: TdlgAbout;

implementation

{$R *.dfm}

end.
