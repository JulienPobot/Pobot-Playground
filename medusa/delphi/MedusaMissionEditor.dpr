program MedusaMissionEditor ;

uses
  Forms,
  fMissionEditor in 'src\fMissionEditor.pas' {MissionEditor},
  MissionDocument in 'src\MissionDocument.pas',
  MedusaUtils in 'src\MedusaUtils.pas',
  fAboutMissionEditor in 'src\fAboutMissionEditor.pas' {AboutMissionEditor},
  polyreg2 in 'src\polyreg2.pas',
  frSensorEditor in 'src\frSensorEditor.pas' {SensorEditor: TFrame},
  fSensorCalibration in 'src\fSensorCalibration.pas' {SensorCalibration} ;

{$R *.res}

begin
  Application.Initialize ;
  Application.Title := 'Editeur de mission Medusa' ;
  Application.CreateForm(TMissionEditor, MissionEditor) ;
  Application.Run ;
end.

