object Form2: TForm2
  Left = 478
  Top = 378
  Width = 576
  Height = 298
  Caption = 'Com test'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  DesignSize = (
    568
    264)
  PixelsPerInch = 96
  TextHeight = 13
  object btnStart: TButton
    Left = 4
    Top = 4
    Width = 75
    Height = 25
    Caption = 'Start'
    TabOrder = 0
    OnClick = btnStartClick
  end
  object btnStop: TButton
    Left = 4
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 1
    OnClick = btnStopClick
  end
  object Memo1: TMemo
    Left = 88
    Top = 4
    Width = 472
    Height = 257
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 2
    WordWrap = False
  end
  object ApdComPort: TApdComPort
    TraceName = 'APRO.TRC'
    LogName = 'APRO.LOG'
    OnPortClose = ApdComPortPortClose
    OnPortOpen = ApdComPortPortOpen
    Left = 4
    Top = 64
  end
  object tmrSend: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrSendTimer
    Left = 4
    Top = 100
  end
  object dpMFrame: TApdDataPacket
    Enabled = True
    EndCond = [ecPacketSize]
    StartString = #2'M'
    IgnoreCase = False
    ComPort = ApdComPort
    PacketSize = 40
    IncludeStrings = False
    Left = 4
    Top = 172
  end
  object dpXFrame: TApdDataPacket
    Enabled = True
    EndCond = [ecPacketSize]
    StartString = #2'X'#28
    IgnoreCase = False
    ComPort = ApdComPort
    PacketSize = 31
    IncludeStrings = False
    OnPacket = dpXFramePacket
    Left = 4
    Top = 140
  end
end
