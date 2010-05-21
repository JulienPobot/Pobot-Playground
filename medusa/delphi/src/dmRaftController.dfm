object RaftController: TRaftController
  OldCreateOrder = False
  OnCreate = DataModuleCreate
  OnDestroy = DataModuleDestroy
  Left = 912
  Top = 673
  Height = 352
  Width = 304
  object ApdComPort: TApdComPort
    PromptForPort = False
    AutoOpen = False
    TraceName = 'APRO.TRC'
    TraceAllHex = True
    LogName = 'APRO.LOG'
    OnPortClose = ApdComPortPortClose
    OnPortOpen = ApdComPortPortOpen
    Left = 32
    Top = 12
  end
  object tmrCom: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrComTimer
    Left = 32
    Top = 72
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
    Left = 96
    Top = 12
  end
  object dpMFrame: TApdDataPacket
    Enabled = True
    EndCond = [ecPacketSize]
    StartString = #2'M'
    IgnoreCase = False
    ComPort = ApdComPort
    PacketSize = 40
    IncludeStrings = False
    OnPacket = dpMFramePacket
    Left = 96
    Top = 72
  end
end
