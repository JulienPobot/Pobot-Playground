object MissionEditor: TMissionEditor
  Left = 466
  Top = 348
  Width = 750
  Height = 553
  Caption = 'Medusa - Editeur de mission'
  Color = clBtnFace
  Constraints.MinHeight = 550
  Constraints.MinWidth = 750
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label24: TLabel
    Left = 20
    Top = 28
    Width = 37
    Height = 13
    Caption = 'Num'#233'ro'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label25: TLabel
    Left = 80
    Top = 28
    Width = 53
    Height = 13
    Caption = 'Description'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label46: TLabel
    Left = 436
    Top = 295
    Width = 29
    Height = 13
    Caption = 'x^3 + '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label47: TLabel
    Left = 524
    Top = 295
    Width = 29
    Height = 13
    Caption = 'x^2 + '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label48: TLabel
    Left = 612
    Top = 295
    Width = 17
    Height = 13
    Caption = 'x + '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object pcMain: TRzPageControl
    Left = 0
    Top = 0
    Width = 742
    Height = 480
    ActivePage = tsSensors
    ActivePageDefault = tsInfos
    Align = alClient
    Color = clWhite
    UseColoredTabs = True
    ParentColor = False
    ShowCardFrame = False
    TabIndex = 1
    TabOrder = 0
    FixedDimension = 19
    object tsInfos: TRzTabSheet
      Color = clWindow
      Caption = 'Informations'
      object RzPanel1: TRzPanel
        Left = 108
        Top = 300
        Width = 185
        Height = 41
        TabOrder = 0
      end
      object RzPanel2: TRzPanel
        Left = 0
        Top = 0
        Width = 742
        Height = 461
        Align = alClient
        BorderOuter = fsFlat
        BorderColor = clWindow
        BorderHighlight = clWhite
        BorderShadow = 13290186
        BorderWidth = 10
        Color = clWindow
        FlatColorAdjustment = 0
        FrameController = RzFrameController
        TabOrder = 1
        object grpNoticeInfos: TRzGroupBox
          Left = 11
          Top = 11
          Width = 720
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
          FrameController = RzFrameController
          GroupStyle = gsStandard
          ParentColor = True
          ParentFont = False
          TabOrder = 0
          Transparent = True
          ThemeAware = False
          object hvGeneralInfos: THTMLViewer
            Left = 9
            Top = 17
            Width = 702
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
        object RzPanel3: TRzPanel
          Left = 11
          Top = 161
          Width = 720
          Height = 289
          Align = alClient
          BorderOuter = fsNone
          ParentColor = True
          TabOrder = 1
          DesignSize = (
            720
            289)
          object RzGroupBox2: TRzGroupBox
            Left = 12
            Top = 14
            Width = 690
            Height = 135
            Anchors = [akLeft, akTop, akRight]
            BorderSides = [sdTop]
            BorderWidth = 5
            Caption = 'Informations g'#233'n'#233'rales '
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clNavy
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            FrameController = RzFrameController
            GroupStyle = gsTopLine
            ParentFont = False
            TabOrder = 0
            Transparent = True
            ThemeAware = False
            DesignSize = (
              690
              135)
            object Label2: TLabel
              Left = 20
              Top = 36
              Width = 45
              Height = 13
              AutoSize = False
              Caption = 'Lieu'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label3: TLabel
              Left = 20
              Top = 68
              Width = 57
              Height = 13
              AutoSize = False
              Caption = 'Description'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object edLocation: TRzEdit
              Left = 84
              Top = 32
              Width = 606
              Height = 21
              Anchors = [akLeft, akTop, akRight]
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 0
              OnChange = edChange
            end
            object edDescription: TRzEdit
              Left = 84
              Top = 64
              Width = 606
              Height = 21
              Anchors = [akLeft, akTop, akRight]
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 1
              OnChange = edChange
            end
          end
        end
      end
    end
    object tsSensors: TRzTabSheet
      Color = clWhite
      Caption = 'Capteurs'
      object RzPanel4: TRzPanel
        Left = 0
        Top = 0
        Width = 742
        Height = 461
        Align = alClient
        BorderOuter = fsFlat
        BorderColor = clWindow
        BorderHighlight = clWhite
        BorderShadow = 13290186
        BorderWidth = 10
        Color = clWindow
        FlatColorAdjustment = 0
        FrameController = RzFrameController
        TabOrder = 0
        object RzGroupBox1: TRzGroupBox
          Left = 11
          Top = 11
          Width = 720
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
          FrameController = RzFrameController
          GroupStyle = gsStandard
          ParentColor = True
          ParentFont = False
          TabOrder = 0
          Transparent = True
          ThemeAware = False
          object hvSensorList: THTMLViewer
            Left = 9
            Top = 17
            Width = 702
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
        object RzPanel8: TRzPanel
          Left = 11
          Top = 161
          Width = 720
          Height = 289
          Align = alClient
          BorderOuter = fsNone
          ParentColor = True
          TabOrder = 1
          DesignSize = (
            720
            289)
          object gbSensors: TRzGroupBox
            Left = 8
            Top = 10
            Width = 706
            Height = 221
            Anchors = [akLeft, akTop, akRight]
            BorderSides = [sdTop]
            Caption = 'Configuration des capteurs de surface '
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
            object Label4: TLabel
              Left = 36
              Top = 28
              Width = 37
              Height = 13
              Caption = 'Num'#233'ro'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label5: TLabel
              Left = 80
              Top = 28
              Width = 22
              Height = 13
              Caption = 'Nom'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label6: TLabel
              Left = 288
              Top = 28
              Width = 30
              Height = 13
              Caption = 'Unit'#233's'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label7: TLabel
              Left = 352
              Top = 28
              Width = 107
              Height = 13
              Caption = 'Formule de conversion'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            inline SurfSensorEditor1: TSensorEditor
              Tag = 1
              Left = 0
              Top = 49
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 0
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
            inline SurfSensorEditor2: TSensorEditor
              Tag = 2
              Left = 0
              Top = 73
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 1
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
            inline SurfSensorEditor3: TSensorEditor
              Tag = 3
              Left = 0
              Top = 97
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 2
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
            inline SurfSensorEditor4: TSensorEditor
              Tag = 4
              Left = 0
              Top = 121
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 3
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
            inline SurfSensorEditor5: TSensorEditor
              Tag = 5
              Left = 0
              Top = 145
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 4
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
            inline SurfSensorEditor6: TSensorEditor
              Tag = 6
              Left = 0
              Top = 169
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 5
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
          end
        end
      end
    end
    object tsDeepSensors: TRzTabSheet
      Color = clWhite
      Caption = 'Sonde'
      object RzPanel10: TRzPanel
        Left = 0
        Top = 0
        Width = 742
        Height = 461
        Align = alClient
        BorderOuter = fsFlat
        BorderColor = clWindow
        BorderHighlight = clWhite
        BorderShadow = 13290186
        BorderWidth = 10
        Color = clWindow
        FlatColorAdjustment = 0
        FrameController = RzFrameController
        TabOrder = 0
        object RzGroupBox6: TRzGroupBox
          Left = 11
          Top = 11
          Width = 720
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
          FrameController = RzFrameController
          GroupStyle = gsStandard
          ParentColor = True
          ParentFont = False
          TabOrder = 0
          Transparent = True
          ThemeAware = False
          object hvDeepSensors: THTMLViewer
            Left = 9
            Top = 17
            Width = 702
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
        object RzPanel11: TRzPanel
          Left = 11
          Top = 161
          Width = 720
          Height = 289
          Align = alClient
          BorderOuter = fsNone
          ParentColor = True
          TabOrder = 1
          DesignSize = (
            720
            289)
          object RzGroupBox7: TRzGroupBox
            Left = 8
            Top = 10
            Width = 706
            Height = 221
            Anchors = [akLeft, akTop, akRight]
            BorderSides = [sdTop]
            Caption = 'Configuration des capteurs de la sonde '
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
            object Label1: TLabel
              Left = 36
              Top = 72
              Width = 37
              Height = 13
              Caption = 'Num'#233'ro'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label30: TLabel
              Left = 80
              Top = 72
              Width = 22
              Height = 13
              Caption = 'Nom'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label31: TLabel
              Left = 288
              Top = 72
              Width = 30
              Height = 13
              Caption = 'Unit'#233's'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label32: TLabel
              Left = 352
              Top = 72
              Width = 107
              Height = 13
              Caption = 'Formule de conversion'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label39: TLabel
              Left = 20
              Top = 40
              Width = 40
              Height = 13
              Caption = 'Sonde : '
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object cbPredefProbes: TRzComboBox
              Left = 64
              Top = 36
              Width = 145
              Height = 21
              Style = csDropDownList
              Ctl3D = False
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ItemHeight = 13
              ParentCtl3D = False
              ParentFont = False
              TabOrder = 0
              OnClick = cbPredefProbesClick
            end
            inline DeepSensorEditor1: TSensorEditor
              Tag = 1
              Left = 1
              Top = 88
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 1
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
            inline DeepSensorEditor2: TSensorEditor
              Tag = 2
              Left = 1
              Top = 112
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 2
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
            inline DeepSensorEditor3: TSensorEditor
              Tag = 3
              Left = 1
              Top = 136
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 3
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
            inline DeepSensorEditor4: TSensorEditor
              Tag = 4
              Left = 1
              Top = 160
              Width = 693
              Height = 25
              AutoSize = True
              Color = clWhite
              ParentColor = False
              TabOrder = 4
              inherited btnPickSensor: TRzToolButton
                DropDownMenu = mnuPredefSensors
                Images = ImageList
              end
              inherited btnCoeffCalc: TRzToolButton
                Images = ImageList
                OnClick = nil
              end
              inherited edName: TRzEdit
                OnChange = nil
              end
              inherited edUnits: TRzEdit
                OnChange = nil
              end
              inherited edCoeff3: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff2: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff1: TRzNumericEdit
                OnChange = nil
              end
              inherited edCoeff0: TRzNumericEdit
                OnChange = nil
              end
            end
          end
        end
      end
    end
    object tsRelays: TRzTabSheet
      Color = clWhite
      Caption = 'Relais'
      object RzPanel6: TRzPanel
        Left = 0
        Top = 0
        Width = 742
        Height = 461
        Align = alClient
        BorderOuter = fsFlat
        BorderColor = clWindow
        BorderHighlight = clWhite
        BorderShadow = 13290186
        BorderWidth = 10
        Color = clWindow
        FlatColorAdjustment = 0
        FrameController = RzFrameController
        TabOrder = 0
        object RzGroupBox3: TRzGroupBox
          Left = 11
          Top = 11
          Width = 720
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
          FrameController = RzFrameController
          GroupStyle = gsStandard
          ParentColor = True
          ParentFont = False
          TabOrder = 0
          Transparent = True
          ThemeAware = False
          object hvRelays: THTMLViewer
            Left = 9
            Top = 17
            Width = 726
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
        object RzPanel7: TRzPanel
          Left = 11
          Top = 161
          Width = 720
          Height = 289
          Align = alClient
          BorderOuter = fsNone
          ParentColor = True
          TabOrder = 1
          DesignSize = (
            720
            289)
          object RzGroupBox4: TRzGroupBox
            Left = 12
            Top = 14
            Width = 690
            Height = 163
            Anchors = [akLeft, akTop, akRight]
            BorderSides = [sdTop]
            Caption = 'Configuration des relais '
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
            object Label14: TLabel
              Left = 20
              Top = 28
              Width = 37
              Height = 13
              Caption = 'Num'#233'ro'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label15: TLabel
              Left = 80
              Top = 28
              Width = 53
              Height = 13
              Caption = 'Description'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label18: TLabel
              Left = 28
              Top = 52
              Width = 25
              Height = 13
              Alignment = taCenter
              AutoSize = False
              Caption = '1'
              Color = clBtnFace
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clDefault
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = True
            end
            object Label19: TLabel
              Left = 28
              Top = 76
              Width = 25
              Height = 13
              Alignment = taCenter
              AutoSize = False
              Caption = '2'
              Color = clBtnFace
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clDefault
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = True
            end
            object Label20: TLabel
              Left = 28
              Top = 100
              Width = 25
              Height = 13
              Alignment = taCenter
              AutoSize = False
              Caption = '3'
              Color = clBtnFace
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clDefault
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = True
            end
            object Label21: TLabel
              Left = 28
              Top = 124
              Width = 25
              Height = 13
              Alignment = taCenter
              AutoSize = False
              Caption = '4'
              Color = clBtnFace
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clDefault
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = True
            end
            object Label22: TLabel
              Left = 328
              Top = 52
              Width = 25
              Height = 13
              Alignment = taCenter
              AutoSize = False
              Caption = '5'
              Color = clBtnFace
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clDefault
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = True
            end
            object Label23: TLabel
              Left = 328
              Top = 76
              Width = 25
              Height = 13
              Alignment = taCenter
              AutoSize = False
              Caption = '6'
              Color = clBtnFace
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clDefault
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = True
            end
            object Label16: TLabel
              Left = 328
              Top = 100
              Width = 25
              Height = 13
              Alignment = taCenter
              AutoSize = False
              Caption = '7'
              Color = clBtnFace
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clDefault
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = True
            end
            object Label17: TLabel
              Left = 328
              Top = 124
              Width = 25
              Height = 13
              Alignment = taCenter
              AutoSize = False
              Caption = '8'
              Color = clBtnFace
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clDefault
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = True
            end
            object Label26: TLabel
              Left = 320
              Top = 28
              Width = 37
              Height = 13
              Caption = 'Num'#233'ro'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label27: TLabel
              Left = 380
              Top = 28
              Width = 53
              Height = 13
              Caption = 'Description'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object edRelayName1: TRzEdit
              Left = 80
              Top = 48
              Width = 201
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 0
              OnChange = edChange
            end
            object edRelayName2: TRzEdit
              Left = 80
              Top = 72
              Width = 201
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 1
              OnChange = edChange
            end
            object edRelayName3: TRzEdit
              Left = 80
              Top = 96
              Width = 201
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 2
              OnChange = edChange
            end
            object edRelayName4: TRzEdit
              Left = 80
              Top = 120
              Width = 201
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 3
              OnChange = edChange
            end
            object edRelayName5: TRzEdit
              Left = 380
              Top = 48
              Width = 201
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 4
              OnChange = edChange
            end
            object edRelayName6: TRzEdit
              Left = 380
              Top = 72
              Width = 201
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 5
              OnChange = edChange
            end
            object edRelayName7: TRzEdit
              Left = 380
              Top = 96
              Width = 201
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 6
              OnChange = edChange
            end
            object edRelayName8: TRzEdit
              Left = 380
              Top = 120
              Width = 201
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 7
              OnChange = edChange
            end
          end
        end
      end
    end
    object tsRecorderOptions: TRzTabSheet
      Color = clWhite
      Caption = 'Enregistreur'
      object RzPanel12: TRzPanel
        Left = 0
        Top = 0
        Width = 742
        Height = 461
        Align = alClient
        BorderOuter = fsFlat
        BorderColor = clWindow
        BorderHighlight = clWhite
        BorderShadow = 13290186
        BorderWidth = 10
        Color = clWindow
        FlatColorAdjustment = 0
        FrameController = RzFrameController
        TabOrder = 0
        object RzGroupBox8: TRzGroupBox
          Left = 11
          Top = 11
          Width = 720
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
          FrameController = RzFrameController
          GroupStyle = gsStandard
          ParentColor = True
          ParentFont = False
          TabOrder = 0
          Transparent = True
          ThemeAware = False
          object hvRecorder: THTMLViewer
            Left = 9
            Top = 17
            Width = 702
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
        object RzPanel13: TRzPanel
          Left = 11
          Top = 161
          Width = 720
          Height = 289
          Align = alClient
          BorderOuter = fsNone
          ParentColor = True
          TabOrder = 1
          DesignSize = (
            720
            289)
          object RzGroupBox9: TRzGroupBox
            Left = 12
            Top = 14
            Width = 690
            Height = 163
            Anchors = [akLeft, akTop, akRight]
            BorderSides = [sdTop]
            Caption = 'Options '
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
            object Label37: TLabel
              Left = 72
              Top = 80
              Width = 173
              Height = 13
              Caption = 'Intervalle entre deux enregistrements'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object Label38: TLabel
              Left = 308
              Top = 80
              Width = 46
              Height = 13
              Caption = 'secondes'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              ParentFont = False
              Transparent = True
            end
            object chkPeriodicRecording: TRzCheckBox
              Left = 72
              Top = 44
              Width = 197
              Height = 17
              Alignment = taLeftJustify
              Caption = 'Enregistrement automatique'
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              HotTrack = True
              ParentFont = False
              State = cbUnchecked
              TabOrder = 0
              Transparent = True
              OnClick = edChange
            end
            object edRecordInterval: TRzNumericEdit
              Left = 256
              Top = 76
              Width = 45
              Height = 21
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clNavy
              Font.Height = -11
              Font.Name = 'MS Sans Serif'
              Font.Style = []
              FrameController = RzFrameController
              ParentFont = False
              TabOrder = 1
              OnChange = edChange
              Max = 3600.000000000000000000
              Min = 1.000000000000000000
              DisplayFormat = ',0;(,0)'
            end
          end
        end
      end
    end
  end
  object RzStatusBar1: TRzStatusBar
    Left = 0
    Top = 480
    Width = 742
    Height = 19
    BorderInner = fsNone
    BorderOuter = fsNone
    BorderSides = [sdLeft, sdTop, sdRight, sdBottom]
    BorderWidth = 0
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object RzVersionInfoStatus1: TRzVersionInfoStatus
      Left = 624
      Top = 0
      Height = 19
      FlatColorAdjustment = 0
      Align = alRight
      FieldLabel = 'Version '
      FieldLabelColor = clNavy
      Field = vifFileVersion
      VersionInfo = RzVersionInfo
    end
    object RzStatusPane1: TRzStatusPane
      Left = 724
      Top = 0
      Width = 18
      Height = 19
      FrameStyle = fsNone
      Align = alRight
    end
    object spMsg: TRzStatusPane
      Left = 0
      Top = 0
      Width = 205
      Height = 19
      Align = alLeft
    end
    object fsDocument: TRzFieldStatus
      Left = 205
      Top = 0
      Width = 419
      Height = 19
      Align = alClient
      FieldLabel = 'Document en cours :'
      FieldLabelColor = clNavy
    end
  end
  object MainMenu: TMainMenu
    Left = 596
    Top = 36
    object Fichier1: TMenuItem
      Caption = 'Fichier'
      object Nouvellemission1: TMenuItem
        Action = actFileNew
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object Ouvrir1: TMenuItem
        Action = actFileOpen
      end
      object Enregistrer1: TMenuItem
        Action = actFileSave
      end
      object Enregistrersous1: TMenuItem
        Action = actFileSaveAs
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object Quitter1: TMenuItem
        Action = actFileExit
      end
    end
    object Aide1: TMenuItem
      Caption = 'Aide'
      object Apropos1: TMenuItem
        Action = actAbout
      end
    end
  end
  object ActionList: TActionList
    Left = 596
    Top = 68
    object actAbout: TAction
      Category = 'help'
      Caption = 'A propos'
      OnExecute = actAboutExecute
    end
    object actFileOpen: TFileOpen
      Category = 'Fichier'
      Caption = '&Ouvrir...'
      Dialog.OnShow = actFileOpenOpenDialogShow
      Dialog.DefaultExt = 'msdmis'
      Dialog.Filter = 'Fichiers Mission Medusa (*.mdsmis)|*.mdsmis'
      Hint = 'Ouvrir|Ouvrir un fichier existant'
      ImageIndex = 7
      ShortCut = 16463
      BeforeExecute = actFileOpenBeforeExecute
      OnAccept = actFileOpenAccept
    end
    object actFileSaveAs: TFileSaveAs
      Category = 'Fichier'
      Caption = 'Enregistrer &sous...'
      Dialog.OnShow = actFileSaveAsSaveDialogShow
      Dialog.DefaultExt = 'mdsmis'
      Dialog.Filter = 'Fichiers mission Medusa (*.mdsmis)|*.mdsmis'
      Dialog.Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
      Hint = 
        'Enregistrer sous|Enregistrer le fichier actif sous un nouveau no' +
        'm'
      ImageIndex = 30
      OnAccept = actFileSaveAsAccept
    end
    object actFileExit: TFileExit
      Category = 'Fichier'
      Caption = '&Quitter'
      Hint = 'Quitter|Quitter l'#39'application'
      ImageIndex = 43
    end
    object actFileSave: TAction
      Category = 'Fichier'
      Caption = 'Enregistrer'
      Hint = 'Enregistrer|Enregistrer la mission'
      ShortCut = 16467
      OnExecute = actFileSaveExecute
      OnUpdate = actFileSaveUpdate
    end
    object actFileNew: TAction
      Category = 'Fichier'
      Caption = 'Nouvelle mission'
      Hint = 'Nouvelle mission|Cr'#233'e une nouvelle mission'
      OnExecute = actFileNewExecute
    end
  end
  object RzFrameController: TRzFrameController
    FrameVisible = True
    Left = 121
    Top = 444
  end
  object mnuPredefSensors: TPopupMenu
    AutoHotkeys = maManual
    Left = 19
    Top = 442
  end
  object RzVersionInfo: TRzVersionInfo
    Left = 55
    Top = 444
  end
  object ImageList: TImageList
    Left = 87
    Top = 444
    Bitmap = {
      494C010103000400040010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      000000000000000000000000000000000000000000000000000000000000CC99
      6600CC996600CC996600CC996600CC996600CC996600CC996600CC996600CC99
      6600CC996600CC996600CC996600CC9966000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990099999900999999000000000099999900999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990099999900999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC00FFFFFF00996633009966
      3300996633009966330099663300996633009966330099663300996633009966
      33009966330099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      99000000000000000000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00000000000000000099999900CCCCCC00FFFFFF0099663300C0C0
      C000C0C0C000C0C0C000C0C0C00099663300C0C0C000C0C0C00099663300C0C0
      C000C0C0C00099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000099000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC9966009999990000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC00FFFFFF0099663300FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0099663300FFFFFF00FFFFFF0099663300FFFF
      FF00FFFFFF0099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000CC000000990000000000CC99
      6600FFFFFF009933000099330000993300009933000099330000993300009933
      00009933000099330000FFFFFF00CC996600B2B2B20099999900000000009999
      9900000000009999990099999900999999009999990099999900999999009999
      990099999900999999000000000099999900CCCCCC00FFFFFF0099663300C0C0
      C000C0C0C000C0C0C000C0C0C00099663300C0C0C000C0C0C00099663300C0C0
      C000C0C0C00099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000FF000000CC0000009900CC99
      6600FFFFFF0099330000CC996600CC996600CC996600CC996600CC996600CC99
      6600CC99660099330000FFFFFF00CC996600CCCCCC00B2B2B200999999009999
      99000000000099999900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00999999000000000099999900CCCCCC00FFFFFF0099663300FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0099663300FFFFFF00FFFFFF0099663300FFFF
      FF00FFFFFF0099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000FF000000CC0000000000CC99
      6600FFFFFF009933000099330000993300009933000099330000993300009933
      00009933000099330000FFFFFF00CC996600CCCCCC00B2B2B200000000009999
      9900000000009999990099999900999999009999990099999900999999009999
      990099999900999999000000000099999900CCCCCC00FFFFFF0099663300C0C0
      C000C0C0C000C0C0C000C0C0C00099663300C0C0C000C0C0C00099663300C0C0
      C000C0C0C00099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000FF000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC996600CCCCCC0000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC00FFFFFF0099663300FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0099663300FFFFFF00FFFFFF0099663300FFFF
      FF00FFFFFF0099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      99000000000000000000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00000000000000000099999900CCCCCC00FFFFFF00996633009966
      3300996633009966330099663300996633009966330099663300996633009966
      33009966330099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC00FFFFFF00CC996600CC99
      6600CC996600CC996600CC996600CC996600CC996600CC996600CC996600CC99
      6600CC996600CC996600FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      99000000000000000000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00000000000000000099999900CCCCCC00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC00FFFFFF00996633009966
      33009966330099663300FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      99000000000000000000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00000000000000000099999900CCCCCC00FFFFFF00CC996600CC99
      6600CC996600CC996600FFFFFF00FFFFFF00FFFFFF00FFFFFF00996633009966
      33009966330099663300FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC996600CC99
      6600CC996600CC996600FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600CC996600CC996600CC996600CC996600CC996600CC996600CC996600CC99
      6600CC996600CC996600CC996600CC9966000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      990099999900999999009999990099999900CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF00E000E00080000000E000EFFE00000000
      E000EFFE00000000E000EC060000000060006FFE000000002000280200000000
      0000080200000000200028020000000060006FFE00000000E000EC0600000000
      E000EFFE00000000E000EC0600000000E000EFFE00000000E000EC0600000000
      E000EFFE00000000E000E0000000000000000000000000000000000000000000
      000000000000}
  end
end
