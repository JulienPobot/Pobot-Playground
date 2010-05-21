object PositionFixEntryDialog: TPositionFixEntryDialog
  Left = 525
  Top = 481
  BorderStyle = bsDialog
  Caption = 'Correction de position GPS'
  ClientHeight = 132
  ClientWidth = 287
  Color = clBtnFace
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
    Left = 8
    Top = 12
    Width = 114
    Height = 13
    Caption = 'Entrez la position r'#233'elle :'
  end
  object Label2: TLabel
    Left = 20
    Top = 36
    Width = 40
    Height = 13
    Caption = 'latitude :'
  end
  object Label3: TLabel
    Left = 20
    Top = 60
    Width = 49
    Height = 13
    Caption = 'longitude :'
  end
  object Label4: TLabel
    Left = 132
    Top = 36
    Width = 9
    Height = 13
    AutoSize = False
    Caption = #176
  end
  object Label5: TLabel
    Left = 132
    Top = 60
    Width = 9
    Height = 13
    AutoSize = False
    Caption = #176
  end
  object Label6: TLabel
    Left = 188
    Top = 36
    Width = 9
    Height = 13
    AutoSize = False
    Caption = #39
  end
  object Label7: TLabel
    Left = 188
    Top = 60
    Width = 9
    Height = 13
    AutoSize = False
    Caption = #39
  end
  object Label8: TLabel
    Left = 240
    Top = 36
    Width = 9
    Height = 13
    AutoSize = False
    Caption = '"'
  end
  object Label9: TLabel
    Left = 240
    Top = 60
    Width = 9
    Height = 13
    AutoSize = False
    Caption = '"'
  end
  object Label10: TLabel
    Left = 252
    Top = 36
    Width = 8
    Height = 13
    Caption = 'N'
  end
  object Label11: TLabel
    Left = 252
    Top = 60
    Width = 7
    Height = 13
    Caption = 'E'
  end
  object lbErrMsg: TLabel
    Left = 12
    Top = 84
    Width = 265
    Height = 13
    Alignment = taCenter
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object edLatDeg: TRzNumericEdit
    Left = 88
    Top = 32
    Width = 41
    Height = 21
    FrameVisible = True
    TabOrder = 0
    OnChange = edChange
    CheckRange = True
    Max = 90.000000000000000000
    DisplayFormat = '0'
    OnRangeError = edRangeError
  end
  object edLonDeg: TRzNumericEdit
    Left = 88
    Top = 56
    Width = 41
    Height = 21
    FrameVisible = True
    TabOrder = 3
    OnChange = edChange
    CheckRange = True
    Max = 180.000000000000000000
    DisplayFormat = '0'
    OnRangeError = edRangeError
  end
  object edLatMin: TRzNumericEdit
    Left = 144
    Top = 32
    Width = 41
    Height = 21
    FrameVisible = True
    TabOrder = 1
    OnChange = edChange
    CheckRange = True
    Max = 59.000000000000000000
    DisplayFormat = '0'
    OnRangeError = edRangeError
  end
  object edLonMin: TRzNumericEdit
    Left = 144
    Top = 56
    Width = 41
    Height = 21
    FrameVisible = True
    TabOrder = 4
    OnChange = edChange
    CheckRange = True
    Max = 59.000000000000000000
    DisplayFormat = '0'
    OnRangeError = edRangeError
  end
  object edLatSec: TRzNumericEdit
    Left = 196
    Top = 32
    Width = 41
    Height = 21
    FrameVisible = True
    TabOrder = 2
    OnChange = edChange
    CheckRange = True
    Max = 59.000000000000000000
    DisplayFormat = '0'
    OnRangeError = edRangeError
  end
  object edLonSec: TRzNumericEdit
    Left = 196
    Top = 56
    Width = 41
    Height = 21
    FrameVisible = True
    TabOrder = 5
    OnChange = edChange
    CheckRange = True
    Max = 59.000000000000000000
    DisplayFormat = '0'
    OnRangeError = edRangeError
  end
  object RzButton1: TRzButton
    Left = 68
    Top = 104
    Default = True
    ModalResult = 1
    Caption = 'OK'
    HotTrack = True
    TabOrder = 6
  end
  object RzButton2: TRzButton
    Left = 148
    Top = 104
    Cancel = True
    ModalResult = 2
    Caption = 'Annuler'
    HotTrack = True
    TabOrder = 7
  end
end
