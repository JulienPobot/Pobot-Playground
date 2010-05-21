program Medusa1;

uses
  Forms,
  fMainForm in 'src\fMainForm.pas' {MainForm},
  AnalogMeter in 'C:\lib\epPack\analogmeter\AnalogMeter.pas',
  dmRaftController in 'src\dmRaftController.pas' {RaftController: TDataModule},
  RaftData in 'src\RaftData.pas',
  InputDevMapping in 'src\InputDevMapping.pas',
  fMapForm in 'src\fMapForm.pas' {MapDisplay},
  dlgPositionFixEntry in 'src\dlgPositionFixEntry.pas' {PositionFixEntryDialog},
  MedusaUtils in 'src\MedusaUtils.pas',
  fAbout in 'src\fAbout.pas' {About},
  MissionDocument in 'src\MissionDocument.pas',
  dlgSelComPort in 'src\dlgSelComPort.pas' {ComPortSelector},
  epPowerIndicator in 'src\epPowerIndicator.pas';

{$R *.res}
{$R medusa-icons.res}

begin
  Application.Initialize;
  Application.Title := 'Medusa1 Control Center';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
