object FormDesigner: TJvDesignPanel
  Left = 0
  Top = 0
  Width = 572
  Height = 453
  Align = alClient
  AutoSize = True
  PopupMenu = Form0.PopupMenu1
  TabOrder = 0
  object Inclination_and_alt: TLabel
    Left = 33
    Top = 49
    Width = 89
    Height = 13
    Caption = 'Inclination_and_alt'
  end
  object Latitude: TLabel
    Left = 234
    Top = 47
    Width = 38
    Height = 13
    Caption = 'Latitude'
  end
  object Body: TLabel
    Left = 385
    Top = 48
    Width = 24
    Height = 13
    Caption = 'Body'
  end
  object Orbit_alt: TLabel
    Left = 57
    Top = 283
    Width = 39
    Height = 13
    Caption = 'Orbit_alt'
  end
  object Calculator: TLabel
    Left = 196
    Top = 6
    Width = 47
    Height = 13
    Caption = 'Calculator'
  end
  object Mass: TLabel
    Left = 463
    Top = 84
    Width = 25
    Height = 17
    Caption = 'Mass'
  end
  object Equator_radius: TLabel
    Left = 466
    Top = 133
    Width = 71
    Height = 13
    Caption = 'Equator_radius'
  end
  object Sidereal_day: TLabel
    Left = 464
    Top = 176
    Width = 61
    Height = 13
    Caption = 'Sidereal_day'
  end
  object ListBox_Body: TListBox
    Left = 369
    Top = 82
    Width = 60
    Height = 128
    ItemHeight = 13
    ScrollWidth = 1
    TabOrder = 0
  end
  object ListBox_Lat: TListBox
    Left = 209
    Top = 82
    Width = 92
    Height = 124
    ItemHeight = 13
    ScrollWidth = 1
    TabOrder = 1
  end
  object ListBox_inclin: TListBox
    Left = 19
    Top = 82
    Width = 124
    Height = 124
    ItemHeight = 13
    ScrollWidth = 1
    TabOrder = 2
  end
  object Edit_Inclin: TEdit
    Left = 19
    Top = 226
    Width = 124
    Height = 21
    TabOrder = 3
    Text = 'Edit_Inclin'
  end
  object Edit_Lat: TEdit
    Left = 209
    Top = 226
    Width = 92
    Height = 21
    TabOrder = 4
    Text = 'Edit_Lat'
  end
  object Edit_Orbit_Alt: TEdit
    Left = 18
    Top = 310
    Width = 124
    Height = 21
    TabOrder = 5
    Text = 'Edit_Orbit_Alt'
  end
  object Calculate: TButton
    Left = 24
    Top = 349
    Width = 112
    Height = 36
    Caption = 'Calculate'
    TabOrder = 6
  end
  object Quit: TButton
    Left = 25
    Top = 398
    Width = 112
    Height = 36
    Caption = 'Quit'
    TabOrder = 7
  end
  object Edit_Output: TEdit
    Left = 208
    Top = 274
    Width = 224
    Height = 157
    AutoSize = False
    ReadOnly = True
    TabOrder = 8
    Text = 'Edit_Output'
  end
  object Edit_Mass: TEdit
    Left = 464
    Top = 101
    Width = 96
    Height = 16
    TabOrder = 9
    Text = 'Edit_Mass'
  end
  object Edit_Equator_Rad: TEdit
    Left = 464
    Top = 148
    Width = 96
    Height = 16
    TabOrder = 10
    Text = 'Edit_Equator_Rad'
  end
  object Edit_Sid_Day: TEdit
    Left = 466
    Top = 192
    Width = 92
    Height = 16
    TabOrder = 11
    Text = 'Edit_Sid_Day'
  end
end
