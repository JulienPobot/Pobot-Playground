object SensorCalibration: TSensorCalibration
  Left = 523
  Top = 224
  Width = 716
  Height = 522
  BorderWidth = 5
  Caption = 'Etalonage capteur'
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object gbCalibration: TRzGroupBox
    Left = 0
    Top = 150
    Width = 698
    Height = 328
    Align = alClient
    BorderSides = [sdTop]
    BorderWidth = 5
    Caption = 'Etalonnage capteur # '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    GroupStyle = gsTopLine
    ParentFont = False
    TabOrder = 0
    Transparent = True
    ThemeAware = False
    object Label28: TLabel
      Left = 48
      Top = 40
      Width = 65
      Height = 13
      AutoSize = False
      Caption = 'Valeur brute'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object Label29: TLabel
      Left = 116
      Top = 40
      Width = 81
      Height = 13
      AutoSize = False
      Caption = 'Valeur physique'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object lbUnits: TLabel
      Left = 196
      Top = 40
      Width = 16
      Height = 13
      Caption = '(xx)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object lbFormula: TLabel
      Left = 252
      Top = 276
      Width = 429
      Height = 21
      Alignment = taCenter
      AutoSize = False
      Caption = 'lbFormula'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clGreen
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      Transparent = True
      Visible = False
      WordWrap = True
    end
    object edRawValue1: TRzNumericEdit
      Tag = 1
      Left = 52
      Top = 60
      Width = 49
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 0
      OnExit = edRawValue1Exit
      AllowBlank = True
      Max = 1024.000000000000000000
      DisplayFormat = ',0;(,0)'
    end
    object edPhysValue1: TRzNumericEdit
      Left = 132
      Top = 60
      Width = 69
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 1
      OnKeyPress = edPhysValueKeyPress
      IntegersOnly = False
      DisplayFormat = '0.###'
    end
    object edRawValue2: TRzNumericEdit
      Tag = 2
      Left = 52
      Top = 84
      Width = 49
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 2
      OnExit = edRawValue1Exit
      AllowBlank = True
      Max = 1024.000000000000000000
      DisplayFormat = ',0;(,0)'
    end
    object edPhysValue2: TRzNumericEdit
      Left = 132
      Top = 84
      Width = 69
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 3
      OnKeyPress = edPhysValueKeyPress
      IntegersOnly = False
      DisplayFormat = '0.###'
    end
    object edRawValue3: TRzNumericEdit
      Tag = 3
      Left = 52
      Top = 108
      Width = 49
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 4
      OnExit = edRawValue1Exit
      AllowBlank = True
      Max = 1024.000000000000000000
      DisplayFormat = ',0;(,0)'
    end
    object edPhysValue3: TRzNumericEdit
      Left = 132
      Top = 108
      Width = 69
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 5
      OnKeyPress = edPhysValueKeyPress
      IntegersOnly = False
      DisplayFormat = '0.###'
    end
    object edRawValue4: TRzNumericEdit
      Tag = 4
      Left = 52
      Top = 132
      Width = 49
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 6
      OnExit = edRawValue1Exit
      AllowBlank = True
      Max = 1024.000000000000000000
      DisplayFormat = ',0;(,0)'
    end
    object edPhysValue4: TRzNumericEdit
      Left = 132
      Top = 132
      Width = 69
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 7
      OnKeyPress = edPhysValueKeyPress
      IntegersOnly = False
      DisplayFormat = '0.###'
    end
    object edRawValue5: TRzNumericEdit
      Tag = 5
      Left = 52
      Top = 156
      Width = 49
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 8
      OnExit = edRawValue1Exit
      AllowBlank = True
      Max = 1024.000000000000000000
      DisplayFormat = ',0;(,0)'
    end
    object edPhysValue5: TRzNumericEdit
      Left = 132
      Top = 156
      Width = 69
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      FrameController = RzFrameController1
      ParentFont = False
      TabOrder = 9
      OnKeyPress = edPhysValueKeyPress
      IntegersOnly = False
      DisplayFormat = '0.###'
    end
    object Chart: TChart
      Left = 256
      Top = 16
      Width = 421
      Height = 257
      BackWall.Brush.Color = clWhite
      BackWall.Brush.Style = bsClear
      Title.Text.Strings = (
        'TChart')
      Title.Visible = False
      View3D = False
      BevelOuter = bvNone
      Color = clWindow
      TabOrder = 10
      object Series1: TLineSeries
        Marks.ArrowLength = 8
        Marks.Visible = False
        SeriesColor = clGreen
        Title = 'formule'
        Pointer.InflateMargins = True
        Pointer.Style = psRectangle
        Pointer.Visible = False
        XValues.DateTime = False
        XValues.Name = 'X '
        XValues.Multiplier = 1.000000000000000000
        XValues.Order = loAscending
        YValues.DateTime = False
        YValues.Name = 'Y '
        YValues.Multiplier = 1.000000000000000000
        YValues.Order = loNone
      end
      object Series2: TPointSeries
        Marks.ArrowLength = 0
        Marks.Visible = False
        SeriesColor = clGreen
        Title = 'mesures'
        Pointer.Brush.Color = clRed
        Pointer.HorizSize = 3
        Pointer.InflateMargins = True
        Pointer.Pen.Visible = False
        Pointer.Style = psRectangle
        Pointer.VertSize = 3
        Pointer.Visible = True
        XValues.DateTime = False
        XValues.Name = 'X '
        XValues.Multiplier = 1.000000000000000000
        XValues.Order = loAscending
        YValues.DateTime = False
        YValues.Name = 'Y '
        YValues.Multiplier = 1.000000000000000000
        YValues.Order = loNone
      end
    end
    object btnCalcFormula: TRzButton
      Left = 52
      Top = 192
      Width = 145
      Caption = 'Calculer la formule'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      HotTrack = True
      ParentFont = False
      TabOrder = 11
      OnClick = btnCalcFormulaClick
    end
    object btnSaveFormula: TRzButton
      Left = 340
      Top = 300
      Width = 105
      ModalResult = 1
      Caption = 'Enregistrer'
      Enabled = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      HotTrack = True
      ParentFont = False
      TabOrder = 12
    end
    object btnCancel: TRzButton
      Left = 456
      Top = 300
      Width = 105
      Cancel = True
      ModalResult = 2
      Caption = 'Annuler'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      HotTrack = True
      ParentFont = False
      TabOrder = 13
    end
    object btnClear: TRzButton
      Left = 52
      Top = 228
      Width = 145
      Cancel = True
      Caption = 'Effacer'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      HotTrack = True
      ParentFont = False
      TabOrder = 14
      OnClick = btnClearClick
    end
  end
  object RzGroupBox1: TRzGroupBox
    Left = 0
    Top = 0
    Width = 698
    Height = 150
    Align = alTop
    BorderColor = clWindow
    BorderOuter = fsGroove
    BorderWidth = 5
    Caption = ' Aide '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    GroupStyle = gsStandard
    ParentColor = True
    ParentFont = False
    TabOrder = 1
    Transparent = True
    ThemeAware = False
    object hvHelp: THTMLViewer
      Left = 9
      Top = 17
      Width = 680
      Height = 124
      TabOrder = 0
      Align = alClient
      DefBackground = clInfoBk
      BorderStyle = htNone
      HistoryMaxCount = 0
      DefFontName = 'Arial'
      DefPreFontName = 'Courier New'
      DefFontSize = 8
      DefFontColor = clNavy
      NoSelect = True
      ScrollBars = ssVertical
      CharSet = DEFAULT_CHARSET
      PrintMarginLeft = 2.000000000000000000
      PrintMarginRight = 2.000000000000000000
      PrintMarginTop = 2.000000000000000000
      PrintMarginBottom = 2.000000000000000000
      PrintScale = 1.000000000000000000
    end
  end
  object RzFrameController1: TRzFrameController
    FrameVisible = True
    Left = 8
    Top = 280
  end
end
