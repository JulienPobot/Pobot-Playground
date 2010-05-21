unit fComTest ;

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
  OoMisc,
  AdPort,
  dmRaftController,
  AdPacket
  ;

type
  TForm2 = class(TForm)
    ApdComPort : TApdComPort ;
    btnStart : TButton ;
    btnStop : TButton ;
    tmrSend : TTimer ;
    Memo1 : TMemo ;
    dpMFrame : TApdDataPacket ;
    dpXFrame : TApdDataPacket ;
    procedure btnStartClick(Sender : TObject) ;
    procedure btnStopClick(Sender : TObject) ;
    procedure FormCreate(Sender : TObject) ;
    procedure tmrSendTimer(Sender : TObject) ;
    procedure ApdComPortTriggerAvail(CP : TObject ; Count : Word) ;
    procedure ApdComPortPortOpen(Sender : TObject) ;
    procedure ApdComPortPortClose(Sender : TObject) ;
    procedure dpXFramePacket(Sender : TObject ; Data : Pointer ;
      Size : Integer) ;
    procedure FormCloseQuery(Sender : TObject ; var CanClose : Boolean) ;
  private
    { Déclarations privées }
    txFrame : TCommandFrame ;
    rxBuffer : array[0..50] of char ;
    rxHead : Integer ;

    procedure ProcessReply() ;
  public
    { Déclarations publiques }
  end ;

var
  Form2             : TForm2 ;

implementation

uses
  RaftData
  ;

{$R *.dfm}

procedure TForm2.btnStartClick(Sender : TObject) ;
begin
  Memo1.Clear ;
  ApdComPort.Open := true ;
  tmrSend.Enabled := true ;
end ;

procedure TForm2.btnStopClick(Sender : TObject) ;
begin
  tmrSend.Enabled := false ;
  rxHead := 0 ;
  ApdComPort.Open := false ;
end ;

procedure TForm2.FormCreate(Sender : TObject) ;
begin
  txFrame.header := #02 ;
  txFrame.trailer := #03 ;
  with txFrame.payload do begin
    expectedReply := repExtended ;
    motorCommand[motLeft].direction := dirStop ;
    motorCommand[motLeft].power := '0' ;
    motorCommand[motRight].direction := dirStop ;
    motorCommand[motRight].power := '0' ;
    motorCommand[motWinch].direction := dirStop ;
    motorCommand[motWinch].power := '0' ;
    IO47 := '0' ;
    IO03 := '0' ;
  end ;

  ApdComPort.TraceName := ExtractFilePath(Application.ExeName) + 'APRO.TRC' ;
end ;

procedure TForm2.tmrSendTimer(Sender : TObject) ;
begin
  ApdComPort.PutBlock(txFrame, sizeof(txFrame)) ;
end ;

procedure TForm2.ApdComPortTriggerAvail(CP : TObject ; Count : Word) ;
var
  i                 : Integer ;
begin
  for i := 1 to Count do begin
    rxBuffer[rxHead] := ApdComPort.GetChar ;
    Inc(rxHead) ;
    if rxHead = 31 then begin
      rxBuffer[rxHead] := #0 ;
      ProcessReply() ;
      rxHead := 0 ;
    end ;
  end ;
end ;

procedure TForm2.ProcessReply ;
var
  s                 : string ;
  i                 : Integer ;
begin
  for i := 0 to 31 do begin
    s := s + Format('%2.2x ', [Ord(rxBuffer[i])]) ;
  end ;
  if Memo1.Lines.Count = 20 then
    Memo1.Clear ;
  Memo1.Lines.Add(s) ;
end ;

procedure TForm2.ApdComPortPortOpen(Sender : TObject) ;
begin
  btnStart.Enabled := false ;
  btnStop.Enabled := true ;
end ;

procedure TForm2.ApdComPortPortClose(Sender : TObject) ;
begin
  btnStart.Enabled := true ;
  btnStop.Enabled := false ;
end ;

procedure TForm2.dpXFramePacket(Sender : TObject ; Data : Pointer ;
  Size : Integer) ;
var
  p                 : PChar ;
  i                 : Integer ;
begin
  p := PChar(Data) ;
  for i := 0 to Size do begin
    rxBuffer[i] := p^ ;
    Inc(p) ;
  end ;
  ProcessReply ;
end ;

procedure TForm2.FormCloseQuery(Sender : TObject ; var CanClose : Boolean) ;
begin
  btnStop.Click ;
end ;

end.

