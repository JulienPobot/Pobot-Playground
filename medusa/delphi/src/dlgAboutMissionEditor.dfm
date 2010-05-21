object dlgAbout: TdlgAbout
  Left = 409
  Top = 244
  BorderStyle = bsDialog
  Caption = 'A propos'
  ClientHeight = 182
  ClientWidth = 424
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 12
    Width = 397
    Height = 113
    AutoSize = False
    Caption = 
      'Cette application est un '#233'diteur de mission. Une mission est d'#233'f' +
      'inie par :'#13'- une date'#13'- un lieu'#13'- une configuration de capteurs ' +
      'et d'#39'actionneurs'#13#13'Ces informations peuvent '#234'tre saisies sur les ' +
      'diff'#233'rentes pages de cette fen'#234'tre et enregistr'#233'es ensuite dans ' +
      'un fichier mission qui peut '#234'tre charg'#233' ensuite par le logiciel ' +
      'de contr'#244'le du radeau.'#13#13'Vous trouverz '#233'galement un outil vous pe' +
      'rmettant d'#39#233'talonner des capteurs simples et d'#39'utiliser les para' +
      'm'#232'tres calcul'#233's par cet outil dans le logiciel de contr'#244'le afin ' +
      'de traduire la mesure '#233'lectrique faite par le capteur en valeur ' +
      'physique correspondante.'
    WordWrap = True
  end
  object btnOk: TRzButton
    Left = 292
    Top = 148
    ModalResult = 1
    Caption = 'OK'
    HotTrack = True
    TabOrder = 0
  end
end
