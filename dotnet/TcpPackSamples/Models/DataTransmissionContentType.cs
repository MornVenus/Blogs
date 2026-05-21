using System.ComponentModel;

namespace Models;

public enum DataTransmissionContentType : byte
{
    HeaderStream = 0,
    Pattern = 1,
    TimingSet = 2,
    AmbientTemperatureSetting = 3,
    GeneralSetting = 4,
    PowerSequence = 5,
    StartSignal = 6,
    StopSignal = 7,
    DftResult = 8,
    ExceptionInfo = 9,
    Query = 10,
    BIB = 11,
    SocketV = 12,
    SocketTemp = 13,
    TimingLevel = 14,
    VectorStepLoop = 15,
    Register = 16,
    Debug = 17,
    AutoDiagItem = 18,
    Initialize = 19,
    RunPowerSeq = 20,
    BibSlotCheck = 21,
    FpgaReportStatus = 0x16,
    ReadHardwareInfo = 0x17,
    ReceiveStatusAndResend = 0x18,
    OvenTemperature = 0x19,
    HeartAlive = 0x1A,
    SelfWarmTemp = 0x1B,
    SendDFTresult = 0x1C,
    ReplyHardwareInfo = 0x1D,
    CheckDriverBoard = 0x1E,
    DriverBoardAllDown = 0x1F,
    ReportTimingError = 0x20,
    ModifyFPGAIPPORT = 0x21,
    UpgradeFPGAOnline = 0x22,
    QueryDftState = 0x23,
    Unknown1 = 0x24,
    [Description("芯片在位检测")]
    DutStateCheck = 0x25,
    [Description("DUT电压电流询问")]
    SocketDuty = 0x26,
    [Description("单Site上下电")]
    SingleSitePower = 0x27,
    [Description("自检查询RegualChannel")]
    QueryRegualChannel = 0x28,
    [Description("电源板在位检测")]
    PowerBoardCheck = 0x29,
    [Description("电源开关询问")]
    PowerQuery = 0x30,
    [Description("设置风扇")]
    SetSocketFan = 0x31,
    [Description("风扇询问响应")]
    SocketFanResp = 0x32,
    [Description("环境温度响应")]
    AmbTempResp = 0x33,
    [Description("校准温度")]
    CalibrateTemp = 0x34,
    [Description("TMP451校准值通信")]
    TMP451 = 0x35,
}
