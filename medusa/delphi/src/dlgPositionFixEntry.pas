unit dlgPositionFixEntry ;

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
  StdCtrls,
  Mask,
  RzEdit,
  RzButton,
  RaftData ;

type
  TPositionFixEntryDialog = class(TForm)
    Label1 : TLabel ;
    Label2 : TLabel ;
    Label3 : TLabel ;
    edLatDeg : TRzNumericEdit ;
    edLonDeg : TRzNumericEdit ;
    Label4 : TLabel ;
    Label5 : TLabel ;
    edLatMin : TRzNumericEdit ;
    Label6 : TLabel ;
    edLonMin : TRzNumericEdit ;
    Label7 : TLabel ;
    edLatSec : TRzNumericEdit ;
    Label8 : TLabel ;
    edLonSec : TRzNumericEdit ;
    Label9 : TLabel ;
    Label10 : TLabel ;
    Label11 : TLabel ;
    RzButton1 : TRzButton ;
    RzButton2 : TRzButton ;
    lbErrMsg : TLabel ;
    procedure edRangeError(Sender : TObject ; EnteredValue,
      AdjustedValue : Extended ; var AutoCorrect : Boolean) ;
    procedure edChange(Sender : TObject) ;
  private
    { Déclarations privées }
  public
    { Déclarations publiques }
    class function GetRealPosition(realPos : TGeoPosition) : Boolean ;
  end ;

var
  PositionFixEntryDialog : TPositionFixEntryDialog ;

implementation

{$R *.dfm}

{ TPositionFixEntry }

class function TPositionFixEntryDialog.GetRealPosition(realPos : TGeoPosition) : Boolean ;
begin
  with TPositionFixEntryDialog.Create(nil) do try
    with realPos.lon do begin
      edLonDeg.IntValue := degrees ;
      edLonMin.IntValue := minutes ;
      edLonSec.IntValue := seconds ;
    end ;
    with realPos.lat do begin
      edLatDeg.IntValue := degrees ;
      edLatMin.IntValue := minutes ;
      edLatSec.IntValue := seconds ;
    end ;

    if ShowModal = mrOk then begin
      with realPos.lon do begin
        degrees := edLonDeg.IntValue ;
        minutes := edLonMin.IntValue ;
        seconds := edLonSec.IntValue ;
      end ;
      with realPos.lat do begin
        degrees := edLatDeg.IntValue ;
        minutes := edLatMin.IntValue ;
        seconds := edLatSec.IntValue ;
      end ;
      Result := true ;
    end

    else
      Result := false ;

  finally
    Release ;
  end ;
end ;

procedure TPositionFixEntryDialog.edRangeError(Sender : TObject ;
  EnteredValue, AdjustedValue : Extended ; var AutoCorrect : Boolean) ;
var
  ctl               : TRzNumericEdit ;
begin
  ctl := Sender as TRzNumericEdit ;
  ctl.SelectAll ;
  lbErrMsg.Caption := Format('Valeur incorrecte. Doit être entre %.0f et %.0f', [ctl.Min, ctl.Max]) ;
  AutoCorrect := false ;
  Beep ;
end ;

procedure TPositionFixEntryDialog.edChange(Sender : TObject) ;
begin
  lbErrMsg.Caption := '' ;
end ;

end.

