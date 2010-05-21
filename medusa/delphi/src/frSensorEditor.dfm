object SensorEditor: TSensorEditor
  Left = 0
  Top = 0
  Width = 693
  Height = 25
  AutoSize = True
  Color = clWhite
  ParentColor = False
  TabOrder = 0
  object btnPickSensor: TRzToolButton
    Tag = 1
    Left = 0
    Top = 0
    Width = 33
    Hint = 'S'#233'lectionne un capteur parmi ceux'#13'qui sont pr'#233'-d'#233'finis'
    ImageIndex = 0
    ToolStyle = tsDropDown
    OnMouseDown = btnPickSensorMouseDown
  end
  object lbSensorNum: TLabel
    Left = 36
    Top = 6
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
  object Label40: TLabel
    Left = 412
    Top = 5
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
  object Label41: TLabel
    Left = 500
    Top = 5
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
  object Label42: TLabel
    Left = 588
    Top = 5
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
  object btnCoeffCalc: TRzToolButton
    Tag = 1
    Left = 672
    Top = 2
    Width = 21
    Height = 21
    ImageIndex = 2
    UseToolbarButtonSize = False
    OnClick = btnCoeffCalcClick
  end
  object edName: TRzEdit
    Left = 80
    Top = 2
    Width = 201
    Height = 21
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FrameVisible = True
    ParentFont = False
    TabOrder = 0
    OnChange = frameEditorChange
  end
  object edUnits: TRzEdit
    Left = 288
    Top = 2
    Width = 57
    Height = 21
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FrameVisible = True
    ParentFont = False
    TabOrder = 1
    OnChange = frameEditorChange
  end
  object edCoeff3: TRzNumericEdit
    Left = 352
    Top = 2
    Width = 57
    Height = 21
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FrameVisible = True
    ParentFont = False
    TabOrder = 2
    OnChange = frameEditorChange
    OnKeyPress = edCoeffKeyPress
    AllowBlank = True
    IntegersOnly = False
    DisplayFormat = '0.######;-0.######'
  end
  object edCoeff2: TRzNumericEdit
    Left = 440
    Top = 2
    Width = 57
    Height = 21
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FrameVisible = True
    ParentFont = False
    TabOrder = 3
    OnChange = frameEditorChange
    OnKeyPress = edCoeffKeyPress
    AllowBlank = True
    IntegersOnly = False
    DisplayFormat = '0.######;-0.######'
  end
  object edCoeff1: TRzNumericEdit
    Left = 528
    Top = 2
    Width = 57
    Height = 21
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FrameVisible = True
    ParentFont = False
    TabOrder = 4
    OnChange = frameEditorChange
    OnKeyPress = edCoeffKeyPress
    AllowBlank = True
    IntegersOnly = False
    DisplayFormat = '0.######;-0.######'
  end
  object edCoeff0: TRzNumericEdit
    Left = 608
    Top = 2
    Width = 57
    Height = 21
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FrameVisible = True
    ParentFont = False
    TabOrder = 5
    OnChange = frameEditorChange
    OnKeyPress = edCoeffKeyPress
    AllowBlank = True
    IntegersOnly = False
    DisplayFormat = '0.######;-0.######'
  end
end
