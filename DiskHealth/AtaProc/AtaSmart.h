/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/
// Reference : http://www.usefullcode.net/2007/02/hddsmart.html (ja)

#pragma once

#include "winioctl.h"
#include "SPTIUtil.h"
#include "SlotSpeedGetter.h"
#include "NVMeInterpreter.h"
#include "StorageQuery.h"

// ******************************************************************************************
// 命令类型字符串数组（用于标识不同的磁盘操作命令类型，便于日志输出和调试）
// 说明：每个字符串对应一种特定的命令类型，与内部命令枚举值一一对应，用于将枚举值转换为易读字符串
// ******************************************************************************************
static const TCHAR* commandTypeString[] =
{
	_T("un"),   // 未知命令类型（Unknown）
	_T("pd"),   // 物理磁盘命令（Physical Disk）
	_T("sm"),   // S.M.A.R.T命令（Self-Monitoring, Analysis and Reporting Technology）
	_T("si"),   // Silicon Image控制器命令（Silicon Image）
	_T("sa"),   // SAT协议命令（SCSI-ATA Translation）
	_T("sp"),   // SCSI协议命令（SCSI Protocol）
	_T("io"),   // IO控制命令（IO Control）
	_T("lo"),   // 日志命令（Log Operation）
	_T("pr"),   // 预设命令（Preset）
	_T("jm"),   // JMicron芯片命令（JMicron）
	_T("cy"),   // 循环命令（Cycle）
	_T("pr"),   // 预设命令（Preset，重复定义可能用于兼容旧版本）
	_T("cs"),   // CSMI协议命令（Common Storage Management Interface）
	_T("cp"),   // 复制命令（Copy）
	_T("wm"),   // WMI命令（Windows Management Instrumentation）
	_T("ns"),   // NVMe三星设备命令（NVMe Samsung）
	_T("ni"),   // NVMe英特尔设备命令（NVMe Intel）
	_T("sq"),   // NVMe存储查询命令（NVMe Storage Query）
	_T("nj"),   // NVMe JMicron芯片命令（NVMe JMicron）
	_T("na"),   // NVMe ASMedia芯片命令（NVMe ASMedia）
	_T("nr"),   // NVMe瑞昱芯片命令（NVMe Realtek）
	_T("nt"),   // NVMe英特尔RST命令（NVMe Intel RST）
	_T("mr")    // MegaRAID阵列卡命令（MegaRAID SAS）
};

// ******************************************************************************************
// SSD厂商标识字符串数组（用于快速标识不同厂商的SSD设备，通常与厂商枚举值对应）
// 说明：每个字符串为厂商英文名称的缩写，便于在日志和状态信息中简洁表示厂商信息
// ******************************************************************************************
static const TCHAR* ssdVendorString[] =
{
	_T(""),      // 未定义/未知厂商
	_T(""),      // 保留位
	_T("mt"),    // MTron（迈创）
	_T("ix"),    // Indilinx（英迪凌，早期SSD主控厂商）
	_T("jm"),    // JMicron（智微科技）
	_T("il"),    // Intel（英特尔）
	_T("sg"),    // SAMSUNG（三星）
	_T("sf"),    // SandForce（晟碟，被希捷收购）
	_T("mi"),    // Micron（美光）
	_T("oz"),    // OCZ（饥饿鲨，被东芝收购）
	_T("st"),    // SEAGATE（希捷）
	_T("wd"),    // WDC（西部数据）
	_T("px"),    // PLEXTOR（浦科特）
	_T("sd"),    // SanDisk（闪迪）
	_T("oz"),    // OCZ Vector（OCZ的Vector系列）
	_T("to"),    // TOSHIABA（东芝）
	_T("co"),    // Corsair（海盗船）
	_T("ki"),    // Kingston（金士顿）
	_T("m2"),    // Micron MU02（美光MU02系列）
	_T("nv"),    // NVMe（通用NVMe设备标识）
	_T("re"),    // Realtek（瑞昱）
	_T("sk"),    // SKhynix（海力士）
	_T("ki"),    // KIOXIA（铠侠，原东芝存储）
	_T("ss"),    // SSSTC（深圳国科微）
	_T("id"),    // Intel DC（英特尔数据中心级产品）
	_T("ap"),    // Apacer（宇瞻）
	_T("sm"),    // SiliconMotion（慧荣科技）
	_T("ph"),    // Phison（群联电子）
	_T("ma"),    // Marvell（美满电子）
	_T("mk"),    // Maxiotek（迈络科技）
	_T("ym"),    // YMTC（长江存储）
	_T("")       // 保留位
};

// ******************************************************************************************
// SMART属性字符串数组（用于标识不同厂商/类型SSD的SMART属性处理逻辑，与厂商枚举值对应）
// 说明：每个字符串表示针对特定厂商SSD的SMART属性处理模块，便于代码中动态调用对应逻辑
// ******************************************************************************************
static const TCHAR* attributeString[] =
{
	_T("Smart"),                 // 通用SMART属性处理
	_T("SmartSsd"),              // 通用SSD的SMART属性处理
	_T("SmartMtron"),            // Mtron SSD的SMART属性处理
	_T("SmartIndilinx"),         // Indilinx主控SSD的SMART属性处理
	_T("SmartJMicron"),          // JMicron主控SSD的SMART属性处理
	_T("SmartIntel"),            // 英特尔SSD的SMART属性处理
	_T("SmartSamsung"),          // 三星SSD的SMART属性处理
	_T("SmartSandForce"),        // SandForce主控SSD的SMART属性处理
	_T("SmartMicron"),           // 美光SSD的SMART属性处理
	_T("SmartOcz"),              // OCZ SSD的SMART属性处理
	_T("SmartSeagate"),          // 希捷SSD的SMART属性处理
	_T("SmartWdc"),              // 西部数据SSD的SMART属性处理
	_T("SmartPlextor"),          // 浦科特SSD的SMART属性处理
	_T("SmartSanDisk"),          // 闪迪SSD的SMART属性处理
	_T("SmartOczVector"),        // OCZ Vector系列SSD的SMART属性处理
	_T("SmartToshiba"),          // 东芝SSD的SMART属性处理
	_T("SmartCorsair"),          // 海盗船SSD的SMART属性处理
	_T("SmartKingston"),         // 金士顿SSD的SMART属性处理
	_T("SmartMicronMU02"),       // 美光MU02系列SSD的SMART属性处理
	_T("SmartNVMe"),             // 通用NVMe设备的SMART属性处理
	_T("SmartRealtek"),          // 瑞昱主控SSD的SMART属性处理
	_T("SmartSKhynix"),          // 海力士SSD的SMART属性处理
	_T("SmartKioxia"),           // 铠侠SSD的SMART属性处理
	_T("SmartSsstc"),            // 国科微SSD的SMART属性处理
	_T("SmartIntelDc"),          // 英特尔数据中心级SSD的SMART属性处理
	_T("SmartApacer"),           // 宇瞻SSD的SMART属性处理
	_T("SmartSiliconMotion"),    // 慧荣主控SSD的SMART属性处理
	_T("SmartPhison"),           // 群联主控SSD的SMART属性处理
	_T("SmartMarvell"),          // 美满主控SSD的SMART属性处理
	_T("SmartMaxiotek"),         // 迈络科技SSD的SMART属性处理
	_T("SmartYmtc")              // 长江存储SSD的SMART属性处理
};

// ******************************************************************************************
// 设备外形规格字符串数组（用于标识磁盘的物理尺寸，与外形规格枚举值对应）
// 说明：将磁盘的物理尺寸枚举值转换为易读字符串，便于UI显示和报告生成
// ******************************************************************************************
static const TCHAR* deviceFormFactorString[] =
{
	_T(""),               // 未定义/未知规格
	_T("5.25 inch"),      // 5.25英寸（早期台式机硬盘常用规格）
	_T("3.5 inch"),       // 3.5英寸（主流台式机硬盘规格）
	_T("2.5 inch"),       // 2.5英寸（笔记本硬盘及多数SSD规格）
	_T("1.8 inch"),       // 1.8英寸（早期小型设备硬盘规格）
	_T("< 1.8 inch")      // 小于1.8英寸（微型设备存储规格）
};

// ******************************************************************************************
// 卷磁盘扩展信息结构体（扩展版）
// 功能：存储卷（逻辑分区）所跨越的物理磁盘范围信息，支持最多4个磁盘扩展区
// 说明：继承自Windows的DISK_EXTENT结构，扩展后可支持更多磁盘范围，用于处理跨多物理盘的卷
// ******************************************************************************************
typedef struct _VOLUME_DISK_EXTENTS_LX {
	DWORD       NumberOfDiskExtents;  // 磁盘扩展区数量（实际使用的扩展区数目，最大为4）
	DISK_EXTENT Extents[4];           // 磁盘扩展区数组（每个元素描述卷在一个物理盘上的范围）
	// DISK_EXTENT结构通常包含：
	// - DiskNumber：物理磁盘编号
	// - StartingOffset：在该物理盘上的起始偏移量
	// - ExtentLength：该物理盘上的卷长度
} VOLUME_DISK_EXTENTS_LX, * PVOLUME_DISK_EXTENTS_LX;  // 结构体别名及指针类型

// **********************************************************************************************
// 类名：CAtaSmart
// 功能描述：封装ATA/SATA/NVMe等存储设备的SMART（自我监测、分析与报告技术）功能
// 核心用途：读取磁盘SMART参数、识别设备类型（HDD/SSD）与厂商、判断磁盘健康状态、获取传输模式等
// 适用平台：Windows（依赖Windows内核IO控制码、SCSI迷你端口协议等）
// **********************************************************************************************
class CAtaSmart
{

public:
	// ******************************************************************************************
	// 静态常量：磁盘与SMART属性相关的数量限制（硬件与协议层面的最大支持量）
	// ******************************************************************************************
	static const int MAX_DISK = 64;// 最大支持的磁盘数量（硬件枚举上限）
	static const int MAX_ATTRIBUTE = 30; // 最大支持的SMART属性数量（多数磁盘实际支持20-30个核心属性）
	static const int MAX_SEARCH_PHYSICAL_DRIVE = 56; // 最大支持的物理驱动器搜索数量
	static const int MAX_SEARCH_SCSI_PORT = 16; // 最大支持的SCSI端口搜索数量
	static const int MAX_SEARCH_SCSI_TARGET_ID = 8; // 最大支持的SCSI目标ID搜索数量

	static const int SCSI_MINIPORT_BUFFER_SIZE = 512; // SCSI迷你端口协议的缓冲区大小（符合SCSI协议数据传输的最小单位要求）

public:
	CAtaSmart();
	virtual ~CAtaSmart();

	// ******************************************************************************************
	// 枚举：VENDOR_ID
	// 功能：定义存储设备的厂商类型与接口类型标识
	// 分类：
	//  1. 内置磁盘厂商（HDD/SSD）：涵盖主流品牌（Intel、三星、希捷等）及通用类型
	//  2. USB外置磁盘厂商：单独标识USB接口的外置存储设备厂商（如Buffalo、Logitech等）
	// ******************************************************************************************
	enum VENDOR_ID
	{
		HDD_GENERAL = 0,              // 通用机械硬盘（无特定厂商标识时使用）
		SSD_GENERAL = 1,              // 通用固态硬盘（无特定厂商标识时使用）
		SSD_VENDOR_MTRON = 2,         // SSD厂商：MTRON（早期SSD厂商，以高性能产品著称）
		SSD_VENDOR_INDILINX = 3,      // SSD厂商：Indilinx（早期SSD控制器厂商，后被OCZ收购）
		SSD_VENDOR_JMICRON = 4,       // SSD厂商：JMicron（存储控制器厂商，常见于入门级SSD）
		SSD_VENDOR_INTEL = 5,         // SSD厂商：Intel（消费级/企业级SSD主流品牌）
		SSD_VENDOR_SAMSUNG = 6,       // SSD厂商：三星（消费级/企业级SSD头部品牌）
		SSD_VENDOR_SANDFORCE = 7,     // SSD厂商：SandForce（SSD控制器厂商，后被希捷收购）
		SSD_VENDOR_MICRON = 8,        // SSD厂商：美光（存储颗粒与SSD一体化厂商）
		SSD_VENDOR_OCZ = 9,           // SSD厂商：OCZ（早期消费级SSD品牌，后被东芝收购）
		SSD_VENDOR_SEAGATE = 10,      // HDD/SSD厂商：希捷（机械硬盘头部品牌，亦产SSD）
		SSD_VENDOR_WDC = 11,          // HDD/SSD厂商：西部数据（机械硬盘头部品牌，含闪迪SSD）
		SSD_VENDOR_PLEXTOR = 12,      // SSD厂商：浦科特（以高性能SSD和光存储产品著称）
		SSD_VENDOR_SANDISK = 13,      // SSD厂商：闪迪（存储颗粒与SSD厂商，后被西部数据收购）
		SSD_VENDOR_OCZ_VECTOR = 14,   // SSD型号：OCZ Vector系列（OCZ经典高端SSD产品线）
		SSD_VENDOR_TOSHIBA = 15,      // HDD/SSD厂商：东芝（存储颗粒与SSD、机械硬盘厂商）
		SSD_VENDOR_CORSAIR = 16,      // SSD厂商：海盗船（以电竞级存储产品著称）
		SSD_VENDOR_KINGSTON = 17,     // SSD/HDD厂商：金士顿（消费级存储产品主流品牌）
		SSD_VENDOR_MICRON_MU02 = 18,  // SSD型号：美光MU02系列（美光企业级SSD产品线）
		SSD_VENDOR_NVME = 19,         // 通用NVMe设备（无特定厂商标识的NVMe SSD）
		SSD_VENDOR_REALTEK = 20,      // 存储控制器厂商：瑞昱（常见于USB-SATA桥接芯片）
		SSD_VENDOR_SKHYNIX = 21,      // SSD厂商：SK海力士（存储颗粒与SSD厂商）
		SSD_VENDOR_KIOXIA = 22,       // SSD厂商：铠侠（原东芝存储，独立后更名）
		SSD_VENDOR_SSSTC = 23,        // SSD厂商：江波龙（国产存储品牌，含消费级/工业级SSD）
		SSD_VENDOR_INTEL_DC = 24,     // SSD系列：Intel数据中心级（Intel企业级SSD产品线）
		SSD_VENDOR_APACER = 25,       // SSD/HDD厂商：宇瞻（存储产品品牌，含消费级/工业级）
		SSD_VENDOR_SILICONMOTION = 26, // SSD控制器厂商：慧荣（主流SSD控制器供应商）
		SSD_VENDOR_PHISON = 27,       // SSD控制器厂商：群联（主流SSD控制器供应商）
		SSD_VENDOR_MARVELL = 28,      // 存储控制器厂商：迈威（企业级存储控制器供应商）
		SSD_VENDOR_MAXIOTEK = 29,     // 存储设备厂商：迈胜（小众存储品牌，含工业级产品）
		SSD_VENDOR_YMTC = 30,         // SSD厂商：长江存储（国产存储颗粒与SSD厂商）
		SSD_VENDOR_MAX = 99,          // 厂商ID上限（用于枚举边界判断）

		VENDOR_UNKNOWN = 0x0000,      // 未知厂商（无法识别的设备厂商）
		USB_VENDOR_BUFFALO = 0x0411,  // USB存储厂商：巴法络（外置硬盘主流品牌）
		USB_VENDOR_IO_DATA = 0x04BB,  // USB存储厂商：IO-DATA（日本外置存储品牌）
		USB_VENDOR_LOGITEC = 0x0789,  // USB存储厂商：罗技（含外置存储产品线）
		USB_VENDOR_INITIO = 0x13FD,   // USB桥接芯片厂商：Initio（USB-SATA桥接方案供应商）
		USB_VENDOR_SUNPLUS = 0x04FC,  // USB桥接芯片厂商：凌阳（USB存储控制芯片供应商）
		USB_VENDOR_JMICRON = 0x152D,  // USB桥接芯片厂商：JMicron（USB-SATA桥接方案供应商）
		USB_VENDOR_CYPRESS = 0x04B4,  // USB芯片厂商：赛普拉斯（USB控制器与桥接芯片供应商）
		USB_VENDOR_OXFORD = 0x0928,   // USB桥接芯片厂商：Oxford（早期USB-SATA桥接方案供应商）
		USB_VENDOR_PROLIFIC = 0x067B, // USB芯片厂商：旺玖（USB转串口/存储控制芯片供应商）
		USB_VENDOR_REALTEK = 0x0BDA,  // USB桥接芯片厂商：瑞昱（USB-SATA桥接方案供应商）
		USB_VENDOR_ALL = 0xFFFF       // 所有USB厂商（枚举时用于筛选全部USB存储设备）
	};

	// ******************************************************************************************
	// 枚举：SMART_STATUS
	// 功能：定义SMART参数的变化状态（用于判断磁盘健康度的变化趋势）
	// ******************************************************************************************
	enum SMART_STATUS
	{
		SMART_STATUS_NO_CHANGE = 0,
		SMART_STATUS_MINOR_CHANGE,
		SMART_STATUS_MAJOR_CHANGE
	};

	// ******************************************************************************************
	// 枚举：TRANSFER_MODE
	// 功能：定义存储设备的传输模式（基于ATA/SATA协议的速率等级）
	// 说明：涵盖PIO、DMA、Ultra DMA、SATA等不同代际的传输标准，速率从低到高递增
	// ******************************************************************************************
	enum TRANSFER_MODE
	{
		TRANSFER_MODE_UNKNOWN = 0,
		TRANSFER_MODE_PIO,
		TRANSFER_MODE_PIO_DMA,
		TRANSFER_MODE_ULTRA_DMA_16,
		TRANSFER_MODE_ULTRA_DMA_25,
		TRANSFER_MODE_ULTRA_DMA_33,
		TRANSFER_MODE_ULTRA_DMA_44,
		TRANSFER_MODE_ULTRA_DMA_66,
		TRANSFER_MODE_ULTRA_DMA_100,
		TRANSFER_MODE_ULTRA_DMA_133,
		TRANSFER_MODE_SATA_150,
		TRANSFER_MODE_SATA_300,
		TRANSFER_MODE_SATA_600
	};

	// ******************************************************************************************
	// 枚举：DISK_STATUS
	// 功能：定义磁盘的整体健康状态（基于SMART参数综合判断结果）
	// ******************************************************************************************
	enum DISK_STATUS
	{
		DISK_STATUS_UNKNOWN = 0,
		DISK_STATUS_GOOD,
		DISK_STATUS_CAUTION,
		DISK_STATUS_BAD
	};

	// ******************************************************************************************
	// 枚举：POWER_ON_HOURS_UNIT
	// 功能：定义磁盘通电时间的计量单位（不同厂商SMART参数中通电时间的单位可能不同）
	// ******************************************************************************************
	enum POWER_ON_HOURS_UNIT
	{
		POWER_ON_UNKNOWN = 0,
		POWER_ON_HOURS,
		POWER_ON_MINUTES,
		POWER_ON_HALF_MINUTES,
		POWER_ON_SECONDS,
		POWER_ON_10_MINUTES,
		POWER_ON_MILLI_SECONDS,
	};

	// ******************************************************************************************
	// 枚举：HOST_READS_WRITES_UNIT
	// 功能：定义磁盘主机读写量的计量单位（不同厂商SMART参数中读写量的单位可能不同）
	// ******************************************************************************************
	enum HOST_READS_WRITES_UNIT
	{
		HOST_READS_WRITES_UNKNOWN = 0,
		HOST_READS_WRITES_512B,
		HOST_READS_WRITES_1MB,
		HOST_READS_WRITES_32MB,
		HOST_READS_WRITES_GB,
	};

	// ******************************************************************************************
	// 枚举：COMMAND_TYPE
	// 功能：定义SMART命令的发送方式（基于不同硬件接口与协议的命令传输类型）
	// 说明：不同接口（物理磁盘、SCSI、NVMe等）需使用对应的命令类型才能正确读取SMART数据
	// ******************************************************************************************
	enum COMMAND_TYPE
	{
		CMD_TYPE_UNKNOWN = 0,
		CMD_TYPE_PHYSICAL_DRIVE,
		CMD_TYPE_SCSI_MINIPORT,
		CMD_TYPE_SILICON_IMAGE,
		CMD_TYPE_SAT,			// SAT = SCSI_ATA_TRANSLATION
		CMD_TYPE_SUNPLUS,
		CMD_TYPE_IO_DATA,
		CMD_TYPE_LOGITEC,
		CMD_TYPE_PROLIFIC,
		CMD_TYPE_JMICRON,
		CMD_TYPE_CYPRESS,
		CMD_TYPE_SAT_ASM1352R,	// AMS1352 2nd drive
		CMD_TYPE_CSMI,				// CSMI = Common Storage Management Interface
		CMD_TYPE_CSMI_PHYSICAL_DRIVE, // CSMI = Common Storage Management Interface 
		CMD_TYPE_WMI,
		CMD_TYPE_NVME_SAMSUNG,
		CMD_TYPE_NVME_INTEL,
		CMD_TYPE_NVME_STORAGE_QUERY,
		CMD_TYPE_NVME_JMICRON,
		CMD_TYPE_NVME_ASMEDIA,
		CMD_TYPE_NVME_REALTEK,
		CMD_TYPE_NVME_INTEL_RST,
		CMD_TYPE_MEGARAID,
		CMD_TYPE_DEBUG
	};

	// ******************************************************************************************
	// 枚举：CSMI_TYPE
	// 功能：定义CSMI（通用存储管理接口）的启用模式（针对支持CSMI协议的控制器）
	// ******************************************************************************************
	enum CSMI_TYPE
	{
		CSMI_TYPE_DISABLE = 0,
		CSMI_TYPE_ENABLE_AUTO,
		CSMI_TYPE_ENABLE_RAID,
		CSMI_TYPE_ENABLE_ALL,
	};

	// ******************************************************************************************
	// 枚举：SMART_WMI_TYPE
	// 功能：定义通过WMI（Windows Management Instrumentation）获取的SMART数据类型
	// 说明：WMI是Windows系统管理接口，部分磁盘通过WMI提供SMART数据，需区分数据本体与阈值
	// ******************************************************************************************
	enum SMART_WMI_TYPE
	{
		WMI_SMART_DATA = 0,	// WMI获取的SMART属性当前值数据（如当前值、最差值、数据值等）
		WMI_SMART_THRESHOLD // WMI获取的SMART属性阈值数据（各属性的临界警告值）
	};

	// ******************************************************************************************
	// 枚举：INTERFACE_TYPE
	// 功能：定义存储设备的物理接口类型（区分设备与主机连接的硬件协议）
	// 说明：涵盖主流存储接口，包括并行、串行、外置接口等，影响SMART数据的读取方式
	// ******************************************************************************************
	enum INTERFACE_TYPE
	{
		INTERFACE_TYPE_UNKNOWN = 0,    // 未知接口类型（无法识别设备的物理连接方式）
		INTERFACE_TYPE_PATA,           // PATA接口（Parallel ATA，并行ATA，旧称IDE，已淘汰）
		INTERFACE_TYPE_SATA,           // SATA接口（Serial ATA，串行ATA，主流内置硬盘接口）
		INTERFACE_TYPE_USB,            // USB接口（Universal Serial Bus，通用串行总线，外置存储主流接口）
		INTERFACE_TYPE_IEEE1394,       // IEEE 1394接口（又称FireWire，火线接口，早期外置存储接口）
		// INTERFACE_TYPE_UASP,        // UASP协议（USB Attached SCSI Protocol，USB存储加速协议，暂未启用）
		INTERFACE_TYPE_SCSI,           // SCSI接口（Small Computer System Interface，小型计算机系统接口，服务器级存储）
		INTERFACE_TYPE_NVME,           // NVMe接口（Non-Volatile Memory Express，专为SSD设计的高速接口）
		// INTERFACE_TYPE_USB_NVME,    // USB-NVMe接口（通过USB桥接的NVMe设备，暂未启用）
	};

protected:
	// ******************************************************************************************
	// 枚举：IO_CONTROL_CODE
	// 功能：定义Windows系统中与存储设备通信的IO控制码（内核级API接口标识）
	// 说明：这些控制码用于通过DeviceIoControl函数向磁盘驱动发送命令，获取SMART数据等
	// ******************************************************************************************
	enum IO_CONTROL_CODE
	{
		DFP_SEND_DRIVE_COMMAND = 0x0007C084,  // 向磁盘发送命令的控制码（用于初始化SMART等操作）
		DFP_RECEIVE_DRIVE_DATA = 0x0007C088,  // 从磁盘接收数据的控制码（用于读取SMART结果等）
		IOCTL_SCSI_MINIPORT = 0x0004D008,     // SCSI迷你端口设备控制码（用于SCSI设备通信）
		IOCTL_IDE_PASS_THROUGH = 0x0004D028,  // IDE设备命令透传控制码（支持Windows 2000及以上系统）
		IOCTL_ATA_PASS_THROUGH = 0x0004D02C   // ATA设备命令透传控制码（支持Windows XP SP2及2003以上系统）
	};

	// 设置结构体按1字节对齐（取消默认字节对齐，确保结构体在内存中紧凑排列，与硬件通信时字节级兼容）
#pragma pack(push,1)
	// ******************************************************************************************
	// 结构体：_IDENTIFY_DEVICE_OUTDATA
	// 功能：存储"识别设备"命令的输出数据（ATA协议中IDENTIFY DEVICE命令的返回结果）
	// 成员：
	//  - SendCmdOutParam：命令输出参数（包含命令执行状态、数据长度等元信息）
	//  - Data：设备识别数据缓冲区（存储厂商信息、容量、支持功能等，大小为IDENTIFY_BUFFER_SIZE-1）
	// ******************************************************************************************
	typedef	struct _IDENTIFY_DEVICE_OUTDATA
	{
		SENDCMDOUTPARAMS	SendCmdOutParam;
		BYTE				Data[IDENTIFY_BUFFER_SIZE - 1];
	} IDENTIFY_DEVICE_OUTDATA, * PIDENTIFY_DEVICE_OUTDATA;

	// ******************************************************************************************
	// 结构体：_SMART_READ_DATA_OUTDATA
	// 功能：存储"读取SMART数据"命令的输出数据（ATA协议中读取SMART属性的返回结果）
	// 成员：
	//  - SendCmdOutParam：命令输出参数（包含命令执行状态、数据长度等元信息）
	//  - Data：SMART属性数据缓冲区（存储各SMART属性的当前值、阈值等，大小为READ_ATTRIBUTE_BUFFER_SIZE-1）
	// ******************************************************************************************
	typedef	struct _SMART_READ_DATA_OUTDATA
	{
		SENDCMDOUTPARAMS	SendCmdOutParam;
		BYTE				Data[READ_ATTRIBUTE_BUFFER_SIZE - 1];
	} SMART_READ_DATA_OUTDATA, * PSMART_READ_DATA_OUTDATA;

	// ******************************************************************************************
	// 结构体：_CMD_IDE_PATH_THROUGH
	// 功能：存储IDE设备命令透传的数据结构（用于向IDE设备发送原始命令）
	// 成员：
	//  - reg：IDE寄存器集合（包含命令寄存器、状态寄存器等，定义IDE命令的具体操作）
	//  - length：缓冲区数据长度（buffer字段的有效数据大小）
	//  - buffer：命令数据缓冲区（存储命令的输入/输出数据，柔性数组实现动态大小）
	// ******************************************************************************************
	typedef struct _CMD_IDE_PATH_THROUGH
	{
		IDEREGS	reg;
		DWORD   length;
		BYTE    buffer[1];
	} CMD_IDE_PATH_THROUGH, * PCMD_IDE_PATH_THROUGH;

	// ******************************************************************************************
	// ATA命令标志位（用于定义ATA命令的传输方向、模式等属性）
	// ******************************************************************************************
	static const int ATA_FLAGS_DRDY_REQUIRED = 0x01;
	static const int ATA_FLAGS_DATA_IN = 0x02;
	static const int ATA_FLAGS_DATA_OUT = 0x04;
	static const int ATA_FLAGS_48BIT_COMMAND = 0x08;

	// ******************************************************************************************
	// 结构体：_ATA_PASS_THROUGH_EX
	// 功能：扩展的ATA命令透传结构体（用于通过IOCTL_ATA_PASS_THROUGH控制码向ATA设备发送命令）
	// 适用场景：支持48位寻址和更大数据传输，兼容Windows XP SP2及以上系统的ATA/SATA设备
	// ******************************************************************************************
	typedef struct _ATA_PASS_THROUGH_EX
	{
		WORD    Length;
		WORD    AtaFlags; //ATA命令标志（组合ATA_FLAGS_xxx常量，如数据方向、48位模式等）
		BYTE    PathId;
		BYTE    TargetId;
		BYTE    Lun;
		BYTE    ReservedAsUchar;
		DWORD   DataTransferLength;
		DWORD   TimeOutValue;
		DWORD   ReservedAsUlong;
		//	DWORD   DataBufferOffset;
#ifdef _WIN64
		DWORD	padding;
#endif
		DWORD_PTR   DataBufferOffset;
		IDEREGS PreviousTaskFile;
		IDEREGS CurrentTaskFile;
	} ATA_PASS_THROUGH_EX, * PCMD_ATA_PASS_THROUGH_EX;

	// ******************************************************************************************
	// 结构体：ATA_PASS_THROUGH_EX_WITH_BUFFERS
	// 功能：带数据缓冲区的ATA命令透传结构体（将命令结构与数据缓冲区整合，简化内存管理）
	// 适用场景：无需单独分配数据缓冲区，直接通过结构体自带的Buf数组传输数据（最大512字节）
	// ******************************************************************************************
	typedef struct
	{
		ATA_PASS_THROUGH_EX Apt;
		DWORD Filer;
		BYTE  Buf[512];
	} ATA_PASS_THROUGH_EX_WITH_BUFFERS;

	// ******************************************************************************************
	// 结构体：_SMART_THRESHOLD
	// 功能：存储SMART属性的阈值信息（各属性触发警告的临界值）
	// 说明：与SMART属性的当前值配合使用，判断属性是否接近或超过警告阈值
	// ******************************************************************************************
	typedef struct _SMART_THRESHOLD
	{
		BYTE    Id;                 // SMART属性ID（对应具体监测指标，如05为扇区重映射计数）
		BYTE    ThresholdValue;     // 属性阈值（当前值低于此值时可能触发警告）
		BYTE    Reserved[10];       // 保留字段（厂商自定义或对齐用，通常为0）
	} SMART_THRESHOLD;

	// ******************************************************************************************
	// 结构体：_SRB_IO_CONTROL
	// 功能：SCSI请求块（SRB）的IO控制头结构（用于SCSI迷你端口设备的命令交互）
	// 说明：定义SCSI命令的元信息，如超时时间、控制码等，是SCSI设备通信的基础结构
	// ******************************************************************************************
	typedef struct _SRB_IO_CONTROL
	{
		ULONG	HeaderLength;
		UCHAR	Signature[8];
		ULONG	Timeout;
		ULONG	ControlCode;
		ULONG	ReturnCode;
		ULONG	Length;
	} SRB_IO_CONTROL;

	// ******************************************************************************************
	// 结构体：_SRB_IO_COMMAND
	// 功能：封装SCSI命令与IDE寄存器的结构（用于通过SCSI协议向IDE设备发送命令）
	// 说明：结合SCSI控制头与IDE寄存器，实现SCSI到IDE的命令转换（如SAT协议场景）
	// ******************************************************************************************
	typedef struct _SRB_IO_COMMAND
	{
		SRB_IO_CONTROL	Cntrol;
		IDEREGS			IdeRegs;
		BYTE			Data[512];
	} SRB_IO_COMMAND;

	// ******************************************************************************************
	// 结构体：SilIdentDev
	// 功能：Silicon Image控制器的设备识别结构（用于获取该品牌控制器连接的设备信息）
	// 说明：针对Silicon Image芯片组的专用结构，包含控制器端口信息与设备识别数据
	// ******************************************************************************************
	typedef struct {
		SRB_IO_CONTROL sic;
		USHORT port;
		USHORT maybe_always1;
		ULONG unknown[5];
		//IDENTIFY_DEVICE id_data ;
		WORD id_data[256];
	} SilIdentDev;

	// ******************************************************************************************
	// 结构体：BIN_IDENTIFY_DEVICE
	// 功能：二进制格式的设备识别数据（原始字节流形式存储设备信息）
	// 说明：用于直接存储或传输设备识别命令的原始二进制结果，不解析具体字段
	// ******************************************************************************************
	struct BIN_IDENTIFY_DEVICE
	{
		BYTE		Bin[4096];  // 4096字节二进制缓冲区（存储原始设备识别数据）
	};

	// ******************************************************************************************
	// 结构体：NVME_IDENTIFY_DEVICE
	// 功能：NVMe设备的识别信息结构（存储NVMe SSD的基本信息）
	// 说明：遵循NVMe协议规范，包含序列号、型号等厂商与设备特性信息
	// ******************************************************************************************
	struct NVME_IDENTIFY_DEVICE
	{
		CHAR		Reserved1[4];
		CHAR		SerialNumber[20];
		CHAR		Model[40];
		CHAR		FirmwareRev[8];
		CHAR		Reserved2[9];
		CHAR		MinorVersion;
		SHORT		MajorVersion;
		CHAR		Reserved3[428];
		CHAR		Reserved4[3584];
	};

	// ******************************************************************************************
	// 结构体：ATA_IDENTIFY_DEVICE
	// 功能：ATA设备的识别信息结构（存储ATA/SATA设备的详细参数与特性）
	// 说明：遵循ATA协议规范，包含设备配置、支持功能、厂商信息等256个WORD字段
	// ******************************************************************************************
	struct ATA_IDENTIFY_DEVICE
	{
		WORD		GeneralConfiguration;					//0
		WORD		LogicalCylinders;						//1	Obsolete
		WORD		SpecificConfiguration;					//2
		WORD		LogicalHeads;							//3 Obsolete
		WORD		Retired1[2];							//4-5
		WORD		LogicalSectors;							//6 Obsolete
		DWORD		ReservedForCompactFlash;				//7-8
		WORD		Retired2;								//9
		CHAR		SerialNumber[20];						//10-19
		WORD		Retired3;								//20
		WORD		BufferSize;								//21 Obsolete
		WORD		Obsolute4;								//22
		CHAR		FirmwareRev[8];							//23-26
		CHAR		Model[40];								//27-46
		WORD		MaxNumPerInterupt;						//47
		WORD		Reserved1;								//48
		WORD		Capabilities1;							//49
		WORD		Capabilities2;							//50
		DWORD		Obsolute5;								//51-52
		WORD		Field88and7064;							//53
		WORD		Obsolute6[5];							//54-58
		WORD		MultSectorStuff;						//59
		DWORD		TotalAddressableSectors;				//60-61
		WORD		Obsolute7;								//62
		WORD		MultiWordDma;							//63
		WORD		PioMode;								//64
		WORD		MinMultiwordDmaCycleTime;				//65
		WORD		RecommendedMultiwordDmaCycleTime;		//66
		WORD		MinPioCycleTimewoFlowCtrl;				//67
		WORD		MinPioCycleTimeWithFlowCtrl;			//68
		WORD		Reserved2[6];							//69-74
		WORD		QueueDepth;								//75
		WORD		SerialAtaCapabilities;					//76
		WORD		SerialAtaAdditionalCapabilities;		//77
		WORD		SerialAtaFeaturesSupported;				//78
		WORD		SerialAtaFeaturesEnabled;				//79
		WORD		MajorVersion;							//80
		WORD		MinorVersion;							//81
		WORD		CommandSetSupported1;					//82
		WORD		CommandSetSupported2;					//83
		WORD		CommandSetSupported3;					//84
		WORD		CommandSetEnabled1;						//85
		WORD		CommandSetEnabled2;						//86
		WORD		CommandSetDefault;						//87
		WORD		UltraDmaMode;							//88
		WORD		TimeReqForSecurityErase;				//89
		WORD		TimeReqForEnhancedSecure;				//90
		WORD		CurrentPowerManagement;					//91
		WORD		MasterPasswordRevision;					//92
		WORD		HardwareResetResult;					//93
		WORD		AcoustricManagement;					//94
		WORD		StreamMinRequestSize;					//95
		WORD		StreamingTimeDma;						//96
		WORD		StreamingAccessLatency;					//97
		DWORD		StreamingPerformance;					//98-99
		ULONGLONG	MaxUserLba;								//100-103
		WORD		StremingTimePio;						//104
		WORD		Reserved3;								//105
		WORD		SectorSize;								//106
		WORD		InterSeekDelay;							//107
		WORD		IeeeOui;								//108
		WORD		UniqueId3;								//109
		WORD		UniqueId2;								//110
		WORD		UniqueId1;								//111
		WORD		Reserved4[4];							//112-115
		WORD		Reserved5;								//116
		DWORD		WordsPerLogicalSector;					//117-118
		WORD		Reserved6[8];							//119-126
		WORD		RemovableMediaStatus;					//127
		WORD		SecurityStatus;							//128
		WORD		VendorSpecific[31];						//129-159
		WORD		CfaPowerMode1;							//160
		WORD		ReservedForCompactFlashAssociation[7];	//161-167
		WORD		DeviceNominalFormFactor;				//168
		WORD		DataSetManagement;						//169
		WORD		AdditionalProductIdentifier[4];			//170-173
		WORD		Reserved7[2];							//174-175
		CHAR		CurrentMediaSerialNo[60];				//176-205
		WORD		SctCommandTransport;					//206
		WORD		ReservedForCeAta1[2];					//207-208
		WORD		AlignmentOfLogicalBlocks;				//209
		DWORD		WriteReadVerifySectorCountMode3;		//210-211
		DWORD		WriteReadVerifySectorCountMode2;		//212-213
		WORD		NvCacheCapabilities;					//214
		DWORD		NvCacheSizeLogicalBlocks;				//215-216
		WORD		NominalMediaRotationRate;				//217
		WORD		Reserved8;								//218
		WORD		NvCacheOptions1;						//219
		WORD		NvCacheOptions2;						//220
		WORD		Reserved9;								//221
		WORD		TransportMajorVersionNumber;			//222
		WORD		TransportMinorVersionNumber;			//223
		WORD		ReservedForCeAta2[10];					//224-233
		WORD		MinimumBlocksPerDownloadMicrocode;		//234
		WORD		MaximumBlocksPerDownloadMicrocode;		//235
		WORD		Reserved10[19];							//236-254
		WORD		IntegrityWord;							//255
	};
#pragma	pack(pop)


	///////////////////////////////////////////////////
	// from http://naraeon.net/en/archives/1126
	///////////////////////////////////////////////////

// ******************************************************************************************
// NVMe设备相关宏定义（用于NVMe协议命令交互与数据传输）
// ******************************************************************************************
#define NVME_STORPORT_DRIVER 0xE000
#define NVME_PASS_THROUGH_SRB_IO_CODE \
	CTL_CODE( NVME_STORPORT_DRIVER, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define NVME_SIG_STR "NvmeMini"
#define NVME_SIG_STR_LEN 8
#define NVME_FROM_DEV_TO_HOST 2
#define NVME_IOCTL_VENDOR_SPECIFIC_DW_SIZE 6
#define NVME_IOCTL_CMD_DW_SIZE 16
#define NVME_IOCTL_COMPLETE_DW_SIZE 4
#define NVME_PT_TIMEOUT 40

// SCSI地址获取控制码（用于获取SCSI设备的地址信息）
#define IOCTL_SCSI_GET_ADDRESS \
	CTL_CODE(IOCTL_SCSI_BASE, 0x0406, METHOD_BUFFERED, FILE_ANY_ACCESS)


	// ******************************************************************************************
	// 结构体：_SCSI_ADDRESS
	// 功能：存储SCSI设备的地址信息（标识SCSI设备在系统中的位置）
	// 说明：用于定位SCSI设备，在多设备系统中区分不同的SCSI设备
	// ******************************************************************************************
	typedef struct _SCSI_ADDRESS {
		ULONG Length;
		UCHAR PortNumber;
		UCHAR PathId;
		UCHAR TargetId;
		UCHAR Lun;
	} SCSI_ADDRESS, * PSCSI_ADDRESS;

	// ******************************************************************************************
	// 结构体：NVME_PASS_THROUGH_IOCTL
	// 功能：NVMe命令透传的IO控制结构（封装NVMe命令及数据缓冲区）
	// 说明：用于通过NVME_PASS_THROUGH_SRB_IO_CODE控制码与NVMe设备交互
	// ******************************************************************************************
	struct NVME_PASS_THROUGH_IOCTL {
		SRB_IO_CONTROL SrbIoCtrl;
		DWORD          VendorSpecific[NVME_IOCTL_VENDOR_SPECIFIC_DW_SIZE];
		DWORD          NVMeCmd[NVME_IOCTL_CMD_DW_SIZE];
		DWORD          CplEntry[NVME_IOCTL_COMPLETE_DW_SIZE];
		DWORD          Direction;
		DWORD          QueueId;
		DWORD          DataBufferLen;
		DWORD          MetaDataLen;
		DWORD          ReturnBufferLen;
		UCHAR          DataBuffer[4096];
	};

	///////////////////
	// from csmisas.h
	///////////////////

	// IOCTL 控制码定义
	// (IoctlHeader.ControlCode)

	// 需要 CSMI_ALL_SIGNATURE 的控制码
#define CC_CSMI_SAS_GET_DRIVER_INFO    1      // 获取驱动信息
#define CC_CSMI_SAS_GET_CNTLR_CONFIG   2      // 获取控制器配置信息
#define CC_CSMI_SAS_GET_CNTLR_STATUS   3      // 获取控制器状态
#define CC_CSMI_SAS_FIRMWARE_DOWNLOAD  4      // 固件下载

// 需要 CSMI_RAID_SIGNATURE 的控制码
#define CC_CSMI_SAS_GET_RAID_INFO      10     // 获取 RAID 信息
#define CC_CSMI_SAS_GET_RAID_CONFIG    11     // 获取 RAID 配置信息
#define CC_CSMI_SAS_GET_RAID_FEATURES  12     // 获取 RAID 特性
#define CC_CSMI_SAS_SET_RAID_CONTROL   13     // 设置 RAID 控制
#define CC_CSMI_SAS_GET_RAID_ELEMENT   14     // 获取 RAID 元素
#define CC_CSMI_SAS_SET_RAID_OPERATION 15     // 设置 RAID 操作

// 需要 CSMI_SAS_SIGNATURE 的控制码
#define CC_CSMI_SAS_GET_PHY_INFO       20     // 获取 PHY 信息
#define CC_CSMI_SAS_SET_PHY_INFO       21     // 设置 PHY 信息
#define CC_CSMI_SAS_GET_LINK_ERRORS    22     // 获取链路错误
#define CC_CSMI_SAS_SMP_PASSTHRU       23     // SMP 透传
#define CC_CSMI_SAS_SSP_PASSTHRU       24     // SSP 透传
#define CC_CSMI_SAS_STP_PASSTHRU       25     // STP 透传
#define CC_CSMI_SAS_GET_SATA_SIGNATURE 26     // 获取 SATA 签名
#define CC_CSMI_SAS_GET_SCSI_ADDRESS   27     // 获取 SCSI 地址
#define CC_CSMI_SAS_GET_DEVICE_ADDRESS 28     // 获取设备地址
#define CC_CSMI_SAS_TASK_MANAGEMENT    29     // 任务管理
#define CC_CSMI_SAS_GET_CONNECTOR_INFO 30     // 获取连接器信息
#define CC_CSMI_SAS_GET_LOCATION       31     // 获取位置信息

// 需要 CSMI_PHY_SIGNATURE 的控制码
#define CC_CSMI_SAS_PHY_CONTROL        60     // PHY 控制

// IOCTL 头部类型定义
#define IOCTL_HEADER SRB_IO_CONTROL
#define PIOCTL_HEADER PSRB_IO_CONTROL

/*************************************************************************/
/* OS INDEPENDENT CODE                                                   */
/*************************************************************************/

/* * * * * * * * * * Class Independent IOCTL Constants * * * * * * * * * */

// Return codes for all IOCTL's regardless of class
// (IoctlHeader.ReturnCode)

#define CSMI_SAS_STATUS_SUCCESS              0
#define CSMI_SAS_STATUS_FAILED               1
#define CSMI_SAS_STATUS_BAD_CNTL_CODE        2
#define CSMI_SAS_STATUS_INVALID_PARAMETER    3
#define CSMI_SAS_STATUS_WRITE_ATTEMPTED      4

// Signature value
// (IoctlHeader.Signature)

#define CSMI_ALL_SIGNATURE    "CSMIALL"

// Timeout value default of 60 seconds
// (IoctlHeader.Timeout)

#define CSMI_ALL_TIMEOUT      60

//  Direction values for data flow on this IOCTL
// (IoctlHeader.Direction, Linux only)
#define CSMI_SAS_DATA_READ    0
#define CSMI_SAS_DATA_WRITE   1

// I/O Bus Types
// ISA and EISA bus types are not supported
// (bIoBusType)

#define CSMI_SAS_BUS_TYPE_PCI       3
#define CSMI_SAS_BUS_TYPE_PCMCIA    4

// Controller Status
// (uStatus)

#define CSMI_SAS_CNTLR_STATUS_GOOD     1
#define CSMI_SAS_CNTLR_STATUS_FAILED   2
#define CSMI_SAS_CNTLR_STATUS_OFFLINE  3
#define CSMI_SAS_CNTLR_STATUS_POWEROFF 4

// Offline Status Reason
// (uOfflineReason)

#define CSMI_SAS_OFFLINE_REASON_NO_REASON             0
#define CSMI_SAS_OFFLINE_REASON_INITIALIZING          1
#define CSMI_SAS_OFFLINE_REASON_BACKSIDE_BUS_DEGRADED 2
#define CSMI_SAS_OFFLINE_REASON_BACKSIDE_BUS_FAILURE  3

// Controller Class
// (bControllerClass)

#define CSMI_SAS_CNTLR_CLASS_HBA    5

// Controller Flag bits
// (uControllerFlags)

#define CSMI_SAS_CNTLR_SAS_HBA          0x00000001
#define CSMI_SAS_CNTLR_SAS_RAID         0x00000002
#define CSMI_SAS_CNTLR_SATA_HBA         0x00000004
#define CSMI_SAS_CNTLR_SATA_RAID        0x00000008
#define CSMI_SAS_CNTLR_SMART_ARRAY      0x00000010

// for firmware download
#define CSMI_SAS_CNTLR_FWD_SUPPORT      0x00010000
#define CSMI_SAS_CNTLR_FWD_ONLINE       0x00020000
#define CSMI_SAS_CNTLR_FWD_SRESET       0x00040000
#define CSMI_SAS_CNTLR_FWD_HRESET       0x00080000
#define CSMI_SAS_CNTLR_FWD_RROM         0x00100000

// for RAID configuration supported
#define CSMI_SAS_CNTLR_RAID_CFG_SUPPORT 0x01000000

// Download Flag bits
// (uDownloadFlags)
#define CSMI_SAS_FWD_VALIDATE       0x00000001
#define CSMI_SAS_FWD_SOFT_RESET     0x00000002
#define CSMI_SAS_FWD_HARD_RESET     0x00000004

// Firmware Download Status
// (usStatus)
#define CSMI_SAS_FWD_SUCCESS        0
#define CSMI_SAS_FWD_FAILED         1
#define CSMI_SAS_FWD_USING_RROM     2
#define CSMI_SAS_FWD_REJECT         3
#define CSMI_SAS_FWD_DOWNREV        4

// Firmware Download Severity
// (usSeverity>
#define CSMI_SAS_FWD_INFORMATION    0
#define CSMI_SAS_FWD_WARNING        1
#define CSMI_SAS_FWD_ERROR          2
#define CSMI_SAS_FWD_FATAL          3

/* * * * * * * * * * SAS RAID Class IOCTL Constants  * * * * * * * * */

// Return codes for the RAID IOCTL's regardless of class
// (IoctlHeader.ReturnCode)

#define CSMI_SAS_RAID_SET_OUT_OF_RANGE       1000
#define CSMI_SAS_RAID_SET_BUFFER_TOO_SMALL   1001
#define CSMI_SAS_RAID_SET_DATA_CHANGED       1002

// Signature value
// (IoctlHeader.Signature)

#define CSMI_RAID_SIGNATURE    "CSMIARY"

// Timeout value default of 60 seconds
// (IoctlHeader.Timeout)

#define CSMI_RAID_TIMEOUT      60

// RAID Types
// (bRaidType)
#define CSMI_SAS_RAID_TYPE_NONE     0
#define CSMI_SAS_RAID_TYPE_0        1
#define CSMI_SAS_RAID_TYPE_1        2
#define CSMI_SAS_RAID_TYPE_10       3
#define CSMI_SAS_RAID_TYPE_5        4
#define CSMI_SAS_RAID_TYPE_15       5
#define CSMI_SAS_RAID_TYPE_6        6
#define CSMI_SAS_RAID_TYPE_50       7
#define CSMI_SAS_RAID_TYPE_VOLUME   8
#define CSMI_SAS_RAID_TYPE_1E       9
#define CSMI_SAS_RAID_TYPE_OTHER    255
// the last value 255 was already defined for other
// so end is defined as 254
#define CSMI_SAS_RAID_TYPE_END      254

// RAID Status
// (bStatus)
#define CSMI_SAS_RAID_SET_STATUS_OK             0
#define CSMI_SAS_RAID_SET_STATUS_DEGRADED       1
#define CSMI_SAS_RAID_SET_STATUS_REBUILDING     2
#define CSMI_SAS_RAID_SET_STATUS_FAILED         3
#define CSMI_SAS_RAID_SET_STATUS_OFFLINE        4
#define CSMI_SAS_RAID_SET_STATUS_TRANSFORMING   5
#define CSMI_SAS_RAID_SET_STATUS_QUEUED_FOR_REBUILD         6
#define CSMI_SAS_RAID_SET_STATUS_QUEUED_FOR_TRANSFORMATION  7

// RAID Drive Count
// (bDriveCount, 0xF1 to 0xFF are reserved)
#define CSMI_SAS_RAID_DRIVE_COUNT_TOO_BIG   0xF1
#define CSMI_SAS_RAID_DRIVE_COUNT_SUPRESSED 0xF2

// RAID Data Type
// (bDataType)
#define CSMI_SAS_RAID_DATA_DRIVES           0
#define CSMI_SAS_RAID_DATA_DEVICE_ID        1
#define CSMI_SAS_RAID_DATA_ADDITIONAL_DATA  2

// RAID Drive Status
// (bDriveStatus)
#define CSMI_SAS_DRIVE_STATUS_OK          0
#define CSMI_SAS_DRIVE_STATUS_REBUILDING  1
#define CSMI_SAS_DRIVE_STATUS_FAILED      2
#define CSMI_SAS_DRIVE_STATUS_DEGRADED    3
#define CSMI_SAS_DRIVE_STATUS_OFFLINE     4
#define CSMI_SAS_DRIVE_STATUS_QUEUED_FOR_REBUILD 5

// RAID Drive Usage
// (bDriveUsage)
#define CSMI_SAS_DRIVE_CONFIG_NOT_USED      0
#define CSMI_SAS_DRIVE_CONFIG_MEMBER        1
#define CSMI_SAS_DRIVE_CONFIG_SPARE         2
#define CSMI_SAS_DRIVE_CONFIG_SPARE_ACTIVE  3

// RAID Drive Type
// (bDriveType)
#define CSMI_SAS_DRIVE_TYPE_UNKNOWN         0
#define CSMI_SAS_DRIVE_TYPE_SINGLE_PORT_SAS 1
#define CSMI_SAS_DRIVE_TYPE_DUAL_PORT_SAS   2
#define CSMI_SAS_DRIVE_TYPE_SATA            3
#define CSMI_SAS_DRIVE_TYPE_SATA_PS         4
#define CSMI_SAS_DRIVE_TYPE_OTHER           255

// RAID Write Protect
// (bWriteProtect)
#define CSMI_SAS_RAID_SET_WRITE_PROTECT_UNKNOWN     0
#define CSMI_SAS_RAID_SET_WRITE_PROTECT_UNCHANGED   0
#define CSMI_SAS_RAID_SET_WRITE_PROTECT_ENABLED     1
#define CSMI_SAS_RAID_SET_WRITE_PROTECT_DISABLED    2

// RAID Cache Setting
// (bCacheSetting)
#define CSMI_SAS_RAID_SET_CACHE_UNKNOWN             0
#define CSMI_SAS_RAID_SET_CACHE_UNCHANGED           0
#define CSMI_SAS_RAID_SET_CACHE_ENABLED             1
#define CSMI_SAS_RAID_SET_CACHE_DISABLED            2
#define CSMI_SAS_RAID_SET_CACHE_CORRUPT             3

// RAID Features
// (uFeatures)
#define CSMI_SAS_RAID_FEATURE_TRANSFORMATION    0x00000001
#define CSMI_SAS_RAID_FEATURE_REBUILD           0x00000002
#define CSMI_SAS_RAID_FEATURE_SPLIT_MIRROR      0x00000004
#define CSMI_SAS_RAID_FEATURE_MERGE_MIRROR      0x00000008
#define CSMI_SAS_RAID_FEATURE_LUN_RENUMBER      0x00000010
#define CSMI_SAS_RAID_FEATURE_SURFACE_SCAN      0x00000020
#define CSMI_SAS_RAID_FEATURE_SPARES_SHARED     0x00000040

// RAID Priority
// (bDefaultTransformPriority, etc.)
#define CSMI_SAS_PRIORITY_UNKNOWN   0
#define CSMI_SAS_PRIORITY_UNCHANGED 0
#define CSMI_SAS_PRIORITY_AUTO      1
#define CSMI_SAS_PRIORITY_OFF       2
#define CSMI_SAS_PRIORITY_LOW       3
#define CSMI_SAS_PRIORITY_MEDIUM    4
#define CSMI_SAS_PRIORITY_HIGH      5

// RAID Transformation Rules
// (uRaidSetTransformationRules)
#define CSMI_SAS_RAID_RULE_AVAILABLE_MEMORY     0x00000001
#define CSMI_SAS_RAID_RULE_OVERLAPPED_EXTENTS   0x00000002

// RAID Cache Ratios Supported
// (bCacheRatiosSupported)
// from 0 to 100 defines the write to read ratio, 0 is 100% write
#define CSMI_SAS_RAID_CACHE_RATIO_RANGE     101
#define CSMI_SAS_RAID_CACHE_RATIO_FIXED     102
#define CSMI_SAS_RAID_CACHE_RATIO_AUTO      103
#define CSMI_SAS_RAID_CACHE_RATIO_END       255

// RAID Cache Ratio Flag
// (bCacheRatioFlag)
#define CSMI_SAS_RAID_CACHE_RATIO_DISABLE   0
#define CSMI_SAS_RAID_CACHE_RATIO_ENABLE    1

// RAID Clear Configuration Signature
// (bClearConfiguration)
#define CSMI_SAS_RAID_CLEAR_CONFIGURATION_SIGNATURE "RAIDCLR"

// RAID Failure Codes
// (uFailureCode)
#define CSMI_SAS_FAIL_CODE_OK                           0
#define CSMI_SAS_FAIL_CODE_PARAMETER_INVALID            1000
#define CSMI_SAS_FAIL_CODE_TRANSFORM_PRIORITY_INVALID   1001
#define CSMI_SAS_FAIL_CODE_REBUILD_PRIORITY_INVALID     1002
#define CSMI_SAS_FAIL_CODE_CACHE_RATIO_INVALID          1003
#define CSMI_SAS_FAIL_CODE_SURFACE_SCAN_INVALID         1004
#define CSMI_SAS_FAIL_CODE_CLEAR_CONFIGURATION_INVALID  1005
#define CSMI_SAS_FAIL_CODE_ELEMENT_INDEX_INVALID        1006
#define CSMI_SAS_FAIL_CODE_SUBELEMENT_INDEX_INVALID     1007
#define CSMI_SAS_FAIL_CODE_EXTENT_INVALID               1008
#define CSMI_SAS_FAIL_CODE_BLOCK_COUNT_INVALID          1009
#define CSMI_SAS_FAIL_CODE_DRIVE_INDEX_INVALID          1010
#define CSMI_SAS_FAIL_CODE_EXISTING_LUN_INVALID         1011
#define CSMI_SAS_FAIL_CODE_RAID_TYPE_INVALID            1012
#define CSMI_SAS_FAIL_CODE_STRIPE_SIZE_INVALID          1013
#define CSMI_SAS_FAIL_CODE_TRANSFORMATION_INVALID       1014
#define CSMI_SAS_FAIL_CODE_CHANGE_COUNT_INVALID         1015
#define CSMI_SAS_FAIL_CODE_ENUMERATION_TYPE_INVALID     1016

#define CSMI_SAS_FAIL_CODE_EXCEEDED_RAID_SET_COUNT      2000
#define CSMI_SAS_FAIL_CODE_DUPLICATE_LUN                2001

#define CSMI_SAS_FAIL_CODE_WAIT_FOR_OPERATION           3000

// RAID Enumeration Types
// (uEnumerationType)
#define CSMI_SAS_RAID_ELEMENT_TYPE_DRIVE                0
#define CSMI_SAS_RAID_ELEMENT_TYPE_MODULE               1
#define CSMI_SAS_RAID_ELEMENT_TYPE_DRIVE_RAID_SET       2
#define CSMI_SAS_RAID_ELEMENT_TYPE_EXTENT_DRIVE         3

// RAID Extent Types
// (bExtentType)
#define CSMI_SAS_RAID_EXTENT_RESERVED       0
#define CSMI_SAS_RAID_EXTENT_METADATA       1
#define CSMI_SAS_RAID_EXTENT_ALLOCATED      2
#define CSMI_SAS_RAID_EXTENT_UNALLOCATED    3

// RAID Operation Types
// (uOperationType)
#define CSMI_SAS_RAID_SET_CREATE            0
#define CSMI_SAS_RAID_SET_LABEL             1
#define CSMI_SAS_RAID_SET_TRANSFORM         2
#define CSMI_SAS_RAID_SET_DELETE            3
#define CSMI_SAS_RAID_SET_WRITE_PROTECT     4
#define CSMI_SAS_RAID_SET_CACHE             5
#define CSMI_SAS_RAID_SET_ONLINE_STATE      6
#define CSMI_SAS_RAID_SET_SPARE             7

// RAID Transform Types
// (bTransformType)
#define CSMI_SAS_RAID_SET_TRANSFORM_SPLIT_MIRROR    0
#define CSMI_SAS_RAID_SET_TRANSFORM_MERGE_RAID_0    1
#define CSMI_SAS_RAID_SET_TRANSFORM_LUN_RENUMBER    2
#define CSMI_SAS_RAID_SET_TRANSFORM_RAID_SET        3

// RAID Online State
// (bOnlineState)
#define CSMI_SAS_RAID_SET_STATE_UNKNOWN     0
#define CSMI_SAS_RAID_SET_STATE_ONLINE      1
#define CSMI_SAS_RAID_SET_STATE_OFFLINE     2

/* * * * * * * * * * SAS HBA Class IOCTL Constants * * * * * * * * * */

// Return codes for SAS IOCTL's
// (IoctlHeader.ReturnCode)

#define CSMI_SAS_PHY_INFO_CHANGED            CSMI_SAS_STATUS_SUCCESS
#define CSMI_SAS_PHY_INFO_NOT_CHANGEABLE     2000
#define CSMI_SAS_LINK_RATE_OUT_OF_RANGE      2001

#define CSMI_SAS_PHY_DOES_NOT_EXIST          2002
#define CSMI_SAS_PHY_DOES_NOT_MATCH_PORT     2003
#define CSMI_SAS_PHY_CANNOT_BE_SELECTED      2004
#define CSMI_SAS_SELECT_PHY_OR_PORT          2005
#define CSMI_SAS_PORT_DOES_NOT_EXIST         2006
#define CSMI_SAS_PORT_CANNOT_BE_SELECTED     2007
#define CSMI_SAS_CONNECTION_FAILED           2008

#define CSMI_SAS_NO_SATA_DEVICE              2009
#define CSMI_SAS_NO_SATA_SIGNATURE           2010
#define CSMI_SAS_SCSI_EMULATION              2011
#define CSMI_SAS_NOT_AN_END_DEVICE           2012
#define CSMI_SAS_NO_SCSI_ADDRESS             2013
#define CSMI_SAS_NO_DEVICE_ADDRESS           2014

// Signature value
// (IoctlHeader.Signature)

#define CSMI_SAS_SIGNATURE    "CSMISAS"

// Timeout value default of 60 seconds
// (IoctlHeader.Timeout)

#define CSMI_SAS_TIMEOUT      60

// Device types
// (bDeviceType)

#define CSMI_SAS_PHY_UNUSED               0x00
#define CSMI_SAS_NO_DEVICE_ATTACHED       0x00
#define CSMI_SAS_END_DEVICE               0x10
#define CSMI_SAS_EDGE_EXPANDER_DEVICE     0x20
#define CSMI_SAS_FANOUT_EXPANDER_DEVICE   0x30

// Protocol options
// (bInitiatorPortProtocol, bTargetPortProtocol)

#define CSMI_SAS_PROTOCOL_SATA   0x01
#define CSMI_SAS_PROTOCOL_SMP    0x02
#define CSMI_SAS_PROTOCOL_STP    0x04
#define CSMI_SAS_PROTOCOL_SSP    0x08

// Negotiated and hardware link rates
// (bNegotiatedLinkRate, bMinimumLinkRate, bMaximumLinkRate)

#define CSMI_SAS_LINK_RATE_UNKNOWN  0x00
#define CSMI_SAS_PHY_DISABLED       0x01
#define CSMI_SAS_LINK_RATE_FAILED   0x02
#define CSMI_SAS_SATA_SPINUP_HOLD   0x03
#define CSMI_SAS_SATA_PORT_SELECTOR 0x04
#define CSMI_SAS_LINK_RATE_1_5_GBPS 0x08
#define CSMI_SAS_LINK_RATE_3_0_GBPS 0x09
#define CSMI_SAS_LINK_VIRTUAL       0x10

// Discover state
// (bAutoDiscover)

#define CSMI_SAS_DISCOVER_NOT_SUPPORTED   0x00
#define CSMI_SAS_DISCOVER_NOT_STARTED     0x01
#define CSMI_SAS_DISCOVER_IN_PROGRESS     0x02
#define CSMI_SAS_DISCOVER_COMPLETE        0x03
#define CSMI_SAS_DISCOVER_ERROR           0x04

// Phy features

#define CSMI_SAS_PHY_VIRTUAL_SMP          0x01

// Programmed link rates
// (bMinimumLinkRate, bMaximumLinkRate)
// (bProgrammedMinimumLinkRate, bProgrammedMaximumLinkRate)

#define CSMI_SAS_PROGRAMMED_LINK_RATE_UNCHANGED 0x00
#define CSMI_SAS_PROGRAMMED_LINK_RATE_1_5_GBPS  0x08
#define CSMI_SAS_PROGRAMMED_LINK_RATE_3_0_GBPS  0x09

// Link rate
// (bNegotiatedLinkRate in CSMI_SAS_SET_PHY_INFO)

#define CSMI_SAS_LINK_RATE_NEGOTIATE      0x00
#define CSMI_SAS_LINK_RATE_PHY_DISABLED   0x01

// Signal class
// (bSignalClass in CSMI_SAS_SET_PHY_INFO)

#define CSMI_SAS_SIGNAL_CLASS_UNKNOWN     0x00
#define CSMI_SAS_SIGNAL_CLASS_DIRECT      0x01
#define CSMI_SAS_SIGNAL_CLASS_SERVER      0x02
#define CSMI_SAS_SIGNAL_CLASS_ENCLOSURE   0x03

// Link error reset
// (bResetCounts)

#define CSMI_SAS_LINK_ERROR_DONT_RESET_COUNTS   0x00
#define CSMI_SAS_LINK_ERROR_RESET_COUNTS        0x01

// Phy identifier
// (bPhyIdentifier)

#define CSMI_SAS_USE_PORT_IDENTIFIER   0xFF

// Port identifier
// (bPortIdentifier)

#define CSMI_SAS_IGNORE_PORT           0xFF

// Programmed link rates
// (bConnectionRate)

#define CSMI_SAS_LINK_RATE_NEGOTIATED  0x00
#define CSMI_SAS_LINK_RATE_1_5_GBPS    0x08
#define CSMI_SAS_LINK_RATE_3_0_GBPS    0x09

// Connection status
// (bConnectionStatus)

#define CSMI_SAS_OPEN_ACCEPT                          0
#define CSMI_SAS_OPEN_REJECT_BAD_DESTINATION          1
#define CSMI_SAS_OPEN_REJECT_RATE_NOT_SUPPORTED       2
#define CSMI_SAS_OPEN_REJECT_NO_DESTINATION           3
#define CSMI_SAS_OPEN_REJECT_PATHWAY_BLOCKED          4
#define CSMI_SAS_OPEN_REJECT_PROTOCOL_NOT_SUPPORTED   5
#define CSMI_SAS_OPEN_REJECT_RESERVE_ABANDON          6
#define CSMI_SAS_OPEN_REJECT_RESERVE_CONTINUE         7
#define CSMI_SAS_OPEN_REJECT_RESERVE_INITIALIZE       8
#define CSMI_SAS_OPEN_REJECT_RESERVE_STOP             9
#define CSMI_SAS_OPEN_REJECT_RETRY                    10
#define CSMI_SAS_OPEN_REJECT_STP_RESOURCES_BUSY       11
#define CSMI_SAS_OPEN_REJECT_WRONG_DESTINATION        12

// SSP Status
// (bSSPStatus)

#define CSMI_SAS_SSP_STATUS_UNKNOWN     0x00
#define CSMI_SAS_SSP_STATUS_WAITING     0x01
#define CSMI_SAS_SSP_STATUS_COMPLETED   0x02
#define CSMI_SAS_SSP_STATUS_FATAL_ERROR 0x03
#define CSMI_SAS_SSP_STATUS_RETRY       0x04
#define CSMI_SAS_SSP_STATUS_NO_TAG      0x05

// SSP Flags
// (uFlags)

#define CSMI_SAS_SSP_READ           0x00000001
#define CSMI_SAS_SSP_WRITE          0x00000002
#define CSMI_SAS_SSP_UNSPECIFIED    0x00000004

#define CSMI_SAS_SSP_TASK_ATTRIBUTE_SIMPLE         0x00000000
#define CSMI_SAS_SSP_TASK_ATTRIBUTE_HEAD_OF_QUEUE  0x00000010
#define CSMI_SAS_SSP_TASK_ATTRIBUTE_ORDERED        0x00000020
#define CSMI_SAS_SSP_TASK_ATTRIBUTE_ACA            0x00000040

// SSP Data present
// (bDataPresent)

#define CSMI_SAS_SSP_NO_DATA_PRESENT         0x00
#define CSMI_SAS_SSP_RESPONSE_DATA_PRESENT   0x01
#define CSMI_SAS_SSP_SENSE_DATA_PRESENT      0x02

// STP Flags
// (uFlags)

#define CSMI_SAS_STP_READ           0x00000001
#define CSMI_SAS_STP_WRITE          0x00000002
#define CSMI_SAS_STP_UNSPECIFIED    0x00000004
#define CSMI_SAS_STP_PIO            0x00000010
#define CSMI_SAS_STP_DMA            0x00000020
#define CSMI_SAS_STP_PACKET         0x00000040
#define CSMI_SAS_STP_DMA_QUEUED     0x00000080
#define CSMI_SAS_STP_EXECUTE_DIAG   0x00000100
#define CSMI_SAS_STP_RESET_DEVICE   0x00000200

// Task Management Flags
// (uFlags)

#define CSMI_SAS_TASK_IU               0x00000001
#define CSMI_SAS_HARD_RESET_SEQUENCE   0x00000002
#define CSMI_SAS_SUPPRESS_RESULT       0x00000004

// Task Management Functions
// (bTaskManagement)

#define CSMI_SAS_SSP_ABORT_TASK           0x01
#define CSMI_SAS_SSP_ABORT_TASK_SET       0x02
#define CSMI_SAS_SSP_CLEAR_TASK_SET       0x04
#define CSMI_SAS_SSP_LOGICAL_UNIT_RESET   0x08
#define CSMI_SAS_SSP_CLEAR_ACA            0x40
#define CSMI_SAS_SSP_QUERY_TASK           0x80

// Task Management Information
// (uInformation)

#define CSMI_SAS_SSP_TEST           1
#define CSMI_SAS_SSP_EXCEEDED       2
#define CSMI_SAS_SSP_DEMAND         3
#define CSMI_SAS_SSP_TRIGGER        4

// Connector Pinout Information
// (uPinout)

#define CSMI_SAS_CON_UNKNOWN              0x00000001
#define CSMI_SAS_CON_SFF_8482             0x00000002
#define CSMI_SAS_CON_SFF_8470_LANE_1      0x00000100
#define CSMI_SAS_CON_SFF_8470_LANE_2      0x00000200
#define CSMI_SAS_CON_SFF_8470_LANE_3      0x00000400
#define CSMI_SAS_CON_SFF_8470_LANE_4      0x00000800
#define CSMI_SAS_CON_SFF_8484_LANE_1      0x00010000
#define CSMI_SAS_CON_SFF_8484_LANE_2      0x00020000
#define CSMI_SAS_CON_SFF_8484_LANE_3      0x00040000
#define CSMI_SAS_CON_SFF_8484_LANE_4      0x00080000

// Connector Location Information
// (bLocation)

// same as uPinout above...
// #define CSMI_SAS_CON_UNKNOWN              0x01
#define CSMI_SAS_CON_INTERNAL             0x02
#define CSMI_SAS_CON_EXTERNAL             0x04
#define CSMI_SAS_CON_SWITCHABLE           0x08
#define CSMI_SAS_CON_AUTO                 0x10
#define CSMI_SAS_CON_NOT_PRESENT          0x20
#define CSMI_SAS_CON_NOT_CONNECTED        0x80

// Device location identification
// (bIdentify)

#define CSMI_SAS_LOCATE_UNKNOWN           0x00
#define CSMI_SAS_LOCATE_FORCE_OFF         0x01
#define CSMI_SAS_LOCATE_FORCE_ON          0x02

// Location Valid flags
// (uLocationFlags)

#define CSMI_SAS_LOCATE_SAS_ADDRESS_VALID           0x00000001
#define CSMI_SAS_LOCATE_SAS_LUN_VALID               0x00000002
#define CSMI_SAS_LOCATE_ENCLOSURE_IDENTIFIER_VALID  0x00000004
#define CSMI_SAS_LOCATE_ENCLOSURE_NAME_VALID        0x00000008
#define CSMI_SAS_LOCATE_BAY_PREFIX_VALID            0x00000010
#define CSMI_SAS_LOCATE_BAY_IDENTIFIER_VALID        0x00000020
#define CSMI_SAS_LOCATE_LOCATION_STATE_VALID        0x00000040

/* * * * * * * * SAS Phy Control Class IOCTL Constants * * * * * * * * */

// Return codes for SAS Phy Control IOCTL's
// (IoctlHeader.ReturnCode)

// Signature value
// (IoctlHeader.Signature)

#define CSMI_PHY_SIGNATURE    "CSMIPHY"

// Phy Control Functions
// (bFunction)

// values 0x00 to 0xFF are consistent in definition with the SMP PHY CONTROL
// function defined in the SAS spec
#define CSMI_SAS_PC_NOP                   0x00000000
#define CSMI_SAS_PC_LINK_RESET            0x00000001
#define CSMI_SAS_PC_HARD_RESET            0x00000002
#define CSMI_SAS_PC_PHY_DISABLE           0x00000003
// 0x04 to 0xFF reserved...
#define CSMI_SAS_PC_GET_PHY_SETTINGS      0x00000100

// Link Flags
#define CSMI_SAS_PHY_ACTIVATE_CONTROL     0x00000001
#define CSMI_SAS_PHY_UPDATE_SPINUP_RATE   0x00000002
#define CSMI_SAS_PHY_AUTO_COMWAKE         0x00000004

// Device Types for Phy Settings
// (bType)
#define CSMI_SAS_UNDEFINED 0x00
#define CSMI_SAS_SATA      0x01
#define CSMI_SAS_SAS       0x02

// Transmitter Flags
// (uTransmitterFlags)
#define CSMI_SAS_PHY_PREEMPHASIS_DISABLED    0x00000001

// Receiver Flags
// (uReceiverFlags)
#define CSMI_SAS_PHY_EQUALIZATION_DISABLED   0x00000001

// Pattern Flags
// (uPatternFlags)
// #define CSMI_SAS_PHY_ACTIVATE_CONTROL     0x00000001
#define CSMI_SAS_PHY_DISABLE_SCRAMBLING      0x00000002
#define CSMI_SAS_PHY_DISABLE_ALIGN           0x00000004
#define CSMI_SAS_PHY_DISABLE_SSC             0x00000008

#define CSMI_SAS_PHY_FIXED_PATTERN           0x00000010
#define CSMI_SAS_PHY_USER_PATTERN            0x00000020

// Fixed Patterns
// (bFixedPattern)
#define CSMI_SAS_PHY_CJPAT                   0x00000001
#define CSMI_SAS_PHY_ALIGN                   0x00000002

// Type Flags
// (bTypeFlags)
#define CSMI_SAS_PHY_POSITIVE_DISPARITY      0x01
#define CSMI_SAS_PHY_NEGATIVE_DISPARITY      0x02
#define CSMI_SAS_PHY_CONTROL_CHARACTER       0x04

// Miscellaneous
#define SLOT_NUMBER_UNKNOWN   0xFFFF

#pragma pack(8)

	typedef struct _CSMI_SAS_DRIVER_INFO
	{
		UCHAR  szName[81];
		UCHAR  szDescription[81];
		USHORT usMajorRevision;
		USHORT usMinorRevision;
		USHORT usBuildRevision;
		USHORT usReleaseRevision;
		USHORT usCSMIMajorRevision;
		USHORT usCSMIMinorRevision;
	} CSMI_SAS_DRIVER_INFO, * PCSMI_SAS_DRIVER_INFO;

	typedef struct _CSMI_SAS_DRIVER_INFO_BUFFER
	{
		SRB_IO_CONTROL IoctlHeader;
		CSMI_SAS_DRIVER_INFO Information;
	} CSMI_SAS_DRIVER_INFO_BUFFER, * PCSMI_SAS_DRIVER_INFO_BUFFER;

	typedef struct _CSMI_SAS_IDENTIFY
	{
		UCHAR bDeviceType;
		UCHAR bRestricted;
		UCHAR bInitiatorPortProtocol;
		UCHAR bTargetPortProtocol;
		UCHAR bRestricted2[8];
		UCHAR bSASAddress[8];
		UCHAR bPhyIdentifier;
		UCHAR bSignalClass;
		UCHAR bReserved[6];
	} CSMI_SAS_IDENTIFY, * PCSMI_SAS_IDENTIFY;

	typedef struct _CSMI_SAS_PHY_ENTITY
	{
		CSMI_SAS_IDENTIFY Identify;
		UCHAR bPortIdentifier;
		UCHAR bNegotiatedLinkRate;
		UCHAR bMinimumLinkRate;
		UCHAR bMaximumLinkRate;
		UCHAR bPhyChangeCount;
		UCHAR bAutoDiscover;
		UCHAR bPhyFeatures;
		UCHAR bReserved;
		CSMI_SAS_IDENTIFY Attached;
	} CSMI_SAS_PHY_ENTITY, * PCSMI_SAS_PHY_ENTITY;

	typedef struct _CSMI_SAS_PHY_INFO
	{
		UCHAR bNumberOfPhys;
		UCHAR bReserved[3];
		CSMI_SAS_PHY_ENTITY Phy[32];
	} CSMI_SAS_PHY_INFO, * PCSMI_SAS_PHY_INFO;

	typedef struct _CSMI_SAS_PHY_INFO_BUFFER
	{
		IOCTL_HEADER IoctlHeader;
		CSMI_SAS_PHY_INFO Information;
	} CSMI_SAS_PHY_INFO_BUFFER, * PCSMI_SAS_PHY_INFO_BUFFER;

	typedef struct _CSMI_SAS_STP_PASSTHRU
	{
		UCHAR bPhyIdentifier;
		UCHAR bPortIdentifier;
		UCHAR bConnectionRate;
		UCHAR bReserved;
		UCHAR bDestinationSASAddress[8];
		UCHAR bReserved2[4];
		UCHAR bCommandFIS[20];
		ULONG uFlags;
		ULONG uDataLength;
	} CSMI_SAS_STP_PASSTHRU, * PCSMI_SAS_STP_PASSTHRU;

	typedef struct _CSMI_SAS_STP_PASSTHRU_STATUS
	{
		UCHAR bConnectionStatus;
		UCHAR bReserved[3];
		UCHAR bStatusFIS[20];
		ULONG uSCR[16];
		ULONG uDataBytes;
	} CSMI_SAS_STP_PASSTHRU_STATUS, * PCSMI_SAS_STP_PASSTHRU_STATUS;

	typedef struct _CSMI_SAS_STP_PASSTHRU_BUFFER
	{
		SRB_IO_CONTROL IoctlHeader;
		CSMI_SAS_STP_PASSTHRU Parameters;
		CSMI_SAS_STP_PASSTHRU_STATUS Status;
		UCHAR bDataBuffer[1];
	} CSMI_SAS_STP_PASSTHRU_BUFFER, * PCSMI_SAS_STP_PASSTHRU_BUFFER;

	// CC_CSMI_SAS_RAID_INFO

	typedef struct _CSMI_SAS_RAID_INFO
	{
		UINT uNumRaidSets;
		UINT uMaxDrivesPerSet;
		UINT uMaxRaidSets;
		UCHAR  bMaxRaidTypes;
		UCHAR  bReservedByteFields[7];
		struct
		{
			UINT uLowPart;
			UINT uHighPart;
		} ulMinRaidSetBlocks;
		struct
		{
			UINT uLowPart;
			UINT uHighPart;
		} ulMaxRaidSetBlocks;
		UINT uMaxPhysicalDrives;
		UINT uMaxExtents;
		UINT uMaxModules;
		UINT uMaxTransformationMemory;
		UINT uChangeCount;
		UCHAR  bReserved[44];
	} CSMI_SAS_RAID_INFO, * PCSMI_SAS_RAID_INFO;

	typedef struct _CSMI_SAS_RAID_INFO_BUFFER
	{
		IOCTL_HEADER IoctlHeader;
		CSMI_SAS_RAID_INFO Information;
	} CSMI_SAS_RAID_INFO_BUFFER, * PCSMI_SAS_RAID_INFO_BUFFER;

	// CC_CSMI_SAS_GET_RAID_CONFIG

	typedef struct _CSMI_SAS_RAID_DRIVES
	{
		UCHAR  bModel[40];
		UCHAR  bFirmware[8];
		UCHAR  bSerialNumber[40];
		UCHAR  bSASAddress[8];
		UCHAR  bSASLun[8];
		UCHAR  bDriveStatus;
		UCHAR  bDriveUsage;
		USHORT usBlockSize;
		UCHAR  bDriveType;
		UCHAR  bReserved[15];
		UINT uDriveIndex;
		struct
		{
			UINT uLowPart;
			UINT uHighPart;
		} ulTotalUserBlocks;
	} CSMI_SAS_RAID_DRIVES,
		* PCSMI_SAS_RAID_DRIVES;

	typedef struct _CSMI_SAS_RAID_DEVICE_ID {
		UCHAR  bDeviceIdentificationVPDPage[1];
	} CSMI_SAS_RAID_DEVICE_ID,
		* PCSMI_SAS_RAID_DEVICE_ID;

	typedef struct _CSMI_SAS_RAID_SET_ADDITIONAL_DATA {
		UCHAR  bLabel[16];
		UCHAR  bRaidSetLun[8];
		UCHAR  bWriteProtection;
		UCHAR  bCacheSetting;
		UCHAR  bCacheRatio;
		USHORT usBlockSize;
		UCHAR  bReservedBytes[11];
		struct
		{
			UINT uLowPart;
			UINT uHighPart;
		} ulRaidSetExtentOffset;
		struct
		{
			UINT uLowPart;
			UINT uHighPart;
		} ulRaidSetBlocks;
		UINT uStripeSizeInBlocks;
		UINT uSectorsPerTrack;
		UCHAR  bApplicationScratchPad[16];
		UINT uNumberOfHeads;
		UINT uNumberOfTracks;
		UCHAR  bReserved[24];
	} CSMI_SAS_RAID_SET_ADDITIONAL_DATA,
		* PCSMI_SAS_RAID_SET_ADDITIONAL_DATA;

	typedef struct _CSMI_SAS_RAID_CONFIG {
		UINT uRaidSetIndex;
		UINT uCapacity;
		UINT uStripeSize;
		UCHAR  bRaidType;
		UCHAR  bStatus;
		UCHAR  bInformation;
		UCHAR  bDriveCount;
		UCHAR  bDataType;
		UCHAR  bReserved[11];
		UINT uFailureCode;
		UINT uChangeCount;
		union {
			CSMI_SAS_RAID_DRIVES Drives[1];
			CSMI_SAS_RAID_DEVICE_ID DeviceId[1];
			CSMI_SAS_RAID_SET_ADDITIONAL_DATA Data[1];
		};
	} CSMI_SAS_RAID_CONFIG,
		* PCSMI_SAS_RAID_CONFIG;

	typedef struct _CSMI_SAS_RAID_CONFIG_BUFFER {
		IOCTL_HEADER IoctlHeader;
		CSMI_SAS_RAID_CONFIG Configuration;
	} CSMI_SAS_RAID_CONFIG_BUFFER,
		* PCSMI_SAS_RAID_CONFIG_BUFFER;

	// 恢复默认内存对齐方式（结束之前的1字节对齐，避免影响后续结构体内存布局）
#pragma pack()

	// ******************************************************************************************
	// 模块说明：Intel RST NVMe专用结构定义
	// 功能：针对Intel RST（Rapid Storage Technology，快速存储技术）驱动下的NVMe设备，
	//       封装符合其协议规范的命令结构，用于读取SMART数据及设备信息
	// ******************************************************************************************
	// 设置结构体按1字节对齐（匹配Intel RST NVMe协议的硬件数据传输格式，确保字节级兼容）
#pragma pack(push, 1)

	// ******************************************************************************************
	// 联合体：NVME_CDW0
	// 功能：NVMe命令的第0个双字（CDW0）结构（封装NVMe命令的核心控制字段）
	// 说明：采用联合体设计，支持按位解析字段或直接操作完整双字，符合NVMe协议规范
	// ******************************************************************************************
	typedef union
	{
		struct
		{
			ULONG Opcode : 8;    // 操作码（标识NVMe命令类型，如Identify、Get Log Page）
			ULONG FUSE : 2;      // 熔断位（控制命令执行模式，如正常/调试）
			ULONG _Rsvd : 4;     // 保留位（必须为0，供协议扩展）
			ULONG PSDT : 2;      // 元数据传输类型（标识元数据的传输方式）
			ULONG CID : 16;      // 命令标识符（唯一标识当前命令，用于匹配响应）
		} DUMMYSTRUCTNAME;
		ULONG AsDWord;           // 完整双字（用于直接赋值或传输时的整体操作）
	} NVME_CDW0, * PNVME_CDW0;

	// ******************************************************************************************
	// 联合体：NVME_IDENTIFY_CDW10
	// 功能：NVMe "Identify" 命令的第10个双字（CDW10）结构
	// 说明：用于配置Identify命令的参数，指定识别的设备类型（如控制器/命名空间）
	// 参考：NVMe协议1.3c版本 Section 4.2，Figure 10
	// ******************************************************************************************
	typedef union
	{
		struct
		{
			ULONG   CNS : 2;
			ULONG   _Rsvd : 30;
		} DUMMYSTRUCTNAME;
		ULONG AsDWord;
	} NVME_IDENTIFY_CDW10, * PNVME_IDENTIFY_CDW10;

	// ******************************************************************************************
	// 联合体：NVME_GET_LOG_PAGE_CDW10
	// 功能：NVMe "Get Log Page" 命令的第10个双字（CDW10）结构（适用于NVMe协议1.3版本之前）
	// 说明：用于配置Get Log Page命令的参数，指定读取的日志页面ID及数据长度
	// ******************************************************************************************
	typedef union
	{
		struct
		{
			ULONG   LID : 8;
			ULONG   _Rsvd1 : 8;
			ULONG   NUMD : 12;
			ULONG   _Rsvd2 : 4;
		} DUMMYSTRUCTNAME;
		ULONG   AsDWord;
	} NVME_GET_LOG_PAGE_CDW10, * PNVME_GET_LOG_PAGE_CDW10;

	// ******************************************************************************************
	// 联合体：NVME_GET_LOG_PAGE_CDW10_V13
	// 功能：NVMe "Get Log Page" 命令的第10个双字（CDW10）结构（适用于NVMe协议1.3版本及以上）
	// 说明：在旧版本基础上扩展字段，支持更多日志页面控制功能（如日志子页面选择）
	// ******************************************************************************************
	typedef union
	{
		struct
		{
			ULONG   LID : 8;
			ULONG   LSP : 4;
			ULONG   Reserved0 : 3;
			ULONG   RAE : 1;
			ULONG   NUMDL : 16;
		} DUMMYSTRUCTNAME;
		ULONG   AsDWord;
	} NVME_GET_LOG_PAGE_CDW10_V13, * PNVME_GET_LOG_PAGE_CDW10_V13;

	// ******************************************************************************************
	// 结构体：NVME_CMD
	// 功能：完整的NVMe命令结构（封装所有NVMe命令的通用字段与命令专属字段）
	// 说明：采用嵌套联合体设计，支持不同NVMe命令（如Identify、Get Log Page）的参数配置，
	//       符合NVMe协议对命令格式的统一要求
	// ******************************************************************************************
	typedef struct
	{
		// 所有NVMe命令的通用字段（命令头部）
		NVME_CDW0           CDW0;            // 命令第0双字（操作码、命令ID等核心控制字段）
		ULONG               NSID;            // 命名空间ID（标识命令作用的存储命名空间）
		ULONG               _Rsvd[2];        // 保留字段（必须为0，供协议扩展）
		ULONGLONG           MPTR;            // 元数据指针（指向元数据缓冲区的地址）
		ULONGLONG           PRP1;            // 物理区域页指针1（指向数据缓冲区的第一个PRP条目）
		ULONGLONG           PRP2;            // 物理区域页指针2（指向数据缓冲区的第二个PRP条目）

		// 命令专属字段（从CDW10到CDW15，不同命令结构不同，用联合体区分）
		union
		{
			// Admin命令：Identify（命令码6，用于读取控制器/命名空间信息）
			struct
			{
				NVME_IDENTIFY_CDW10 CDW10;    // 命令第10双字（Identify命令专属参数）
				ULONG   CDW11;                // 命令第11双字（保留）
				ULONG   CDW12;                // 命令第12双字（保留）
				ULONG   CDW13;                // 命令第13双字（保留）
				ULONG   CDW14;                // 命令第14双字（保留）
				ULONG   CDW15;                // 命令第15双字（保留）
			} IDENTIFY;

			// Admin命令：Get Log Page（命令码2，用于读取设备日志，如SMART日志）
			struct
			{
				NVME_GET_LOG_PAGE_CDW10 CDW10;// 命令第10双字（Get Log Page命令专属参数，旧协议版本）
				// NVME_GET_LOG_PAGE_CDW10_V13 CDW10; // 协议1.3+版本可替换为此字段
				ULONG   CDW11;                // 命令第11双字（保留）
				ULONG   CDW12;                // 命令第12双字（保留）
				ULONG   CDW13;                // 命令第13双字（保留）
				ULONG   CDW14;                // 命令第14双字（保留）
				ULONG   CDW15;                // 命令第15双字（保留）
			} GET_LOG_PAGE;
		} u;
	} NVME_CMD, * PNVME_CMD;

	// ******************************************************************************************
// 结构体：INTEL_NVME_PAYLOAD
// 功能：Intel RST NVMe命令的负载结构（封装NVMe命令及相关控制参数）
// 说明：Intel RST驱动专用，用于传递NVMe命令的核心参数与结果缓存信息
// 字段偏移备注：括号内为结构体成员在内存中的偏移地址（便于驱动层地址映射）
// ******************************************************************************************
	typedef struct _INTEL_NVME_PAYLOAD
	{
		BYTE    Version;        // 版本号（固定为0x001C，标识Intel RST NVMe协议版本）[0x001C]
		BYTE    PathId;         // 路径ID（标识NVMe设备所在的控制器路径）[0x001D]
		BYTE    TargetID;       // 目标设备ID（标识控制器上的具体NVMe设备）[0x001E]
		BYTE    Lun;            // 逻辑单元号（NVMe设备内的逻辑分区标识，通常为0）[0x001F]
		NVME_CMD Cmd;           // NVMe命令结构（完整的NVMe命令配置）[0x0020 ~ 0x005F]
		DWORD   CplEntry[4];    // 命令完成条目（存储命令执行结果状态）[0x0060 ~ 0x006F]
		DWORD   QueueId;        // 队列ID（标识执行命令的NVMe IO队列）[0x0070 ~ 0x0073]
		DWORD   ParamBufLen;    // 参数缓冲区长度（输入参数的字节数）[0x0074]
		DWORD   ReturnBufferLen;// 返回缓冲区长度（输出结果的字节数）[0x0078]
		BYTE    __rsvd2[0x28];  // 保留字段（Intel RST驱动预留，必须为0）[0x007C ~ 0xA3]
	} INTEL_NVME_PAYLOAD, * PINTEL_NVME_PAYLOAD;

	// ******************************************************************************************
	// 结构体：INTEL_NVME_PASS_THROUGH
	// 功能：Intel RST NVMe命令透传结构（整合SRB控制头、命令负载与数据缓冲区）
	// 说明：Intel RST驱动下与NVMe设备通信的完整数据结构，用于发送命令并接收结果
	// 字段偏移备注：括号内为结构体成员在内存中的偏移地址
	// ******************************************************************************************
	typedef struct _INTEL_NVME_PASS_THROUGH
	{
		SRB_IO_CONTROL SRB;     // SRB IO控制头（包含命令签名、超时时间等元信息）[0x0000 ~ 0x001B]
		INTEL_NVME_PAYLOAD Payload; // NVMe命令负载（命令参数与结果状态）
		BYTE DataBuffer[0x1000];// 4096字节数据缓冲区（存储命令输入/输出数据）
	} INTEL_NVME_PASS_THROUGH, * PINTEL_NVME_PASS_THROUGH;

	// 恢复默认内存对齐方式（结束Intel RST NVMe专用的1字节对齐）
#pragma pack(pop)

#define IOCTL_INTEL_NVME_PASS_THROUGH CTL_CODE(0xf000, 0xA02, METHOD_BUFFERED, FILE_ANY_ACCESS);

	//////////////////////////////////////////////////////////////////
	// for MegaRAID SAS
	//////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

#define MAX_SYS_PDS               240
	// ******************************************************************************************
	// 结构体：MEGARAID_PASS_THROUGH
	// 功能：LSI MegaRAID阵列卡的SCSI命令透传核心结构（封装SCSI命令的关键参数）
	// 说明：用于向MegaRAID阵列卡下的物理磁盘（如SAS/SATA HDD/SSD）发送SCSI命令，
	//       实现底层设备操作（如读取SMART数据、执行磁盘诊断）
	// ******************************************************************************************
	struct MEGARAID_PASS_THROUGH
	{
		UCHAR Cmd;
		UCHAR SenseLength;
		UCHAR CmdStatus;
		UCHAR ScsiStatus;

		UCHAR TargetId;
		UCHAR Lun;
		UCHAR CdbLength;
		UCHAR SenseInfoLength;

		ULONG Context;
		ULONG Padding0;

		USHORT Flags;
		USHORT TimeOutValue;
		ULONG DataTransferLength;

		ULONG SenseInfoOffsetLo;
		ULONG SenseInfoOffsetHi;

		UCHAR Cdb[16];
	};

	// ******************************************************************************************
	// 结构体：MEGARAID_PASS_THROUGH_IOCTL
	// 功能：LSI MegaRAID阵列卡的SCSI命令透传完整IO结构（整合控制头、命令参数与数据缓冲区）
	// 说明：通过IOCTL命令与MegaRAID驱动通信的完整载体，用于发送SCSI命令并接收设备响应
	// ******************************************************************************************
	struct MEGARAID_PASS_THROUGH_IOCTL
	{
		SRB_IO_CONTROL    SrbIoCtrl;
		MEGARAID_PASS_THROUGH Mpt;
		UCHAR             SenseBuf[112];
		UCHAR             DataBuf[4096];
	};

	// ******************************************************************************************
	// 结构体：MEGARAID_DCOMD
	// 功能：LSI MegaRAID阵列卡的设备控制命令（DCOMD）结构（用于执行阵列卡专属控制操作）
	// 说明：区别于通用SCSI命令，用于操作MegaRAID阵列卡自身功能（如阵列配置、磁盘管理）
	// ******************************************************************************************
	struct MEGARAID_DCOMD
	{
		UCHAR Cmd;
		UCHAR Reserved0;
		UCHAR CmdStatus;
		UCHAR Reserved1[4];
		UCHAR SenseInfoLength;

		ULONG Context;
		ULONG Padding0;

		USHORT Flags;
		USHORT TimeOutValue;

		ULONG DataTransferLength;
		ULONG Opcode;

		UCHAR Mbox[12];
	};

	// ******************************************************************************************
	// 结构体：MEGARAID_DCOMD_IOCTL
	// 功能：LSI MegaRAID阵列卡的设备控制命令完整IO结构（整合控制头、命令参数与缓冲区）
	// 说明：用于向MegaRAID阵列卡发送专属控制命令，实现阵列管理功能（如磁盘状态查询、阵列重建）
	// ******************************************************************************************
	struct MEGARAID_DCOMD_IOCTL
	{
		SRB_IO_CONTROL SrbIoCtrl;
		MEGARAID_DCOMD Mpt;
		UCHAR          SenseBuf[120];
		UCHAR          DataBuf[4096];
	};

	// ******************************************************************************************
	// 结构体：MEGARAID_PHYSICAL_DRIVE_ADDRESS
	// 功能：LSI MegaRAID阵列卡下物理磁盘的地址标识结构（唯一定位单块物理磁盘）
	// 说明：在多磁盘、多 enclosure（磁盘柜）的阵列环境中，精准识别每一块物理磁盘
	// ******************************************************************************************
	struct MEGARAID_PHYSICAL_DRIVE_ADDRESS
	{
		USHORT DeviceId;
		USHORT EnclDeviceId;
		UCHAR  EnclIndex;
		UCHAR  SlotNumber;
		UCHAR  ScsiDevType;
		UCHAR  ConnectPortBitmap;
		UINT64 SasAddr[2];
	};

	// ******************************************************************************************
	// 结构体：MEGARAID_PHYSICAL_DRIVE_LIST
	// 功能：LSI MegaRAID阵列卡下所有物理磁盘的列表结构（批量存储物理磁盘地址信息）
	// 说明：用于一次性获取阵列卡管理的所有物理磁盘信息，简化多磁盘枚举操作
	// ******************************************************************************************
	struct MEGARAID_PHYSICAL_DRIVE_LIST
	{
		ULONG Size;
		ULONG Count;
		MEGARAID_PHYSICAL_DRIVE_ADDRESS Addr[MAX_SYS_PDS];
	};
#pragma pack(pop)

	// ******************************************************************************************
	// MFI（MegaRAID Firmware Interface，MegaRAID固件接口）相关宏定义
	// 功能：定义MegaRAID阵列卡与固件通信的命令类型、状态码、参数等，是MegaRAID设备操作的核心常量
	// ******************************************************************************************
#define MFI_CMD_PD_SCSI_IO        0x04
#define MFI_CMD_DCMD              0x05

#define MFI_STAT_OK               0x00

#define MFI_DCMD_PD_GET_LIST      0x02010000

#define MFI_MBOX_SIZE             12
#define MFI_FRAME_DIR_NONE        0x0000
#define MFI_FRAME_DIR_WRITE       0x0008
#define MFI_FRAME_DIR_READ        0x0010
#define MFI_FRAME_DIR_BOTH        0x0018


public:
	// ******************************************************************************************
	// 函数：UpdateSmartInfo
	// 功能：更新指定索引磁盘的SMART信息（重新读取该磁盘的SMART属性数据，如扇区重映射计数、通电时间等）
	// 参数：
	//  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
	// 返回值：DWORD类型，操作结果码（0通常表示成功，非0表示失败，具体值对应错误类型）
	// ******************************************************************************************
	DWORD UpdateSmartInfo(DWORD index);
	// ******************************************************************************************
   // 函数：UpdateIdInfo
   // 功能：更新指定索引磁盘的设备识别信息（重新读取磁盘的厂商、型号、固件版本、容量等基础信息）
   // 参数：
   //  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
   // 返回值：BOOL类型，TRUE表示更新成功，FALSE表示更新失败（如磁盘不存在、读取超时）
   // ******************************************************************************************
	BOOL UpdateIdInfo(DWORD index);
	// ******************************************************************************************
	// 函数：GetAamValue
	// 功能：获取指定索引磁盘的AAM值（AAM：Automatic Acoustic Management，自动声学管理，控制磁盘噪音）
	// 参数：
	//  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
	// 返回值：BYTE类型，当前AAM设置值（范围通常为0-255，值越大噪音越小、性能可能越低，0表示禁用）
	// ******************************************************************************************
	BYTE GetAamValue(DWORD index);
	// ******************************************************************************************
	// 函数：GetApmValue
	// 功能：获取指定索引磁盘的APM值（APM：Advanced Power Management，高级电源管理，控制磁盘功耗）
	// 参数：
	//  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
	// 返回值：BYTE类型，当前APM设置值（范围通常为0-255，值越大功耗越高、响应速度越快，0表示禁用）
	// ******************************************************************************************
	BYTE GetApmValue(DWORD index);
	// ******************************************************************************************
	// 函数：EnableAam
	// 功能：为指定索引磁盘启用AAM（自动声学管理）并设置参数
	// 参数：
	//  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
	//  - param：AAM参数值（目标噪音/性能平衡值，范围0-255，具体值需符合磁盘支持的范围）
	// 返回值：BOOL类型，TRUE表示启用并设置成功，FALSE表示失败（如磁盘不支持AAM、参数无效）
	// ******************************************************************************************
	BOOL EnableAam(DWORD index, BYTE param);
	// ******************************************************************************************
	// 函数：EnableApm
	// 功能：为指定索引磁盘启用APM（高级电源管理）并设置参数
	// 参数：
	//  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
	//  - param：APM参数值（目标功耗/性能平衡值，范围0-255，具体值需符合磁盘支持的范围）
	// 返回值：BOOL类型，TRUE表示启用并设置成功，FALSE表示失败（如磁盘不支持APM、参数无效）
	// ******************************************************************************************
	BOOL EnableApm(DWORD index, BYTE param);
	// ******************************************************************************************
	// 函数：DisableAam
	// 功能：禁用指定索引磁盘的AAM（自动声学管理）功能
	// 参数：
	//  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
	// 返回值：BOOL类型，TRUE表示禁用成功，FALSE表示失败（如磁盘不支持AAM、操作超时）
	// ******************************************************************************************
	BOOL DisableAam(DWORD index);
	// ******************************************************************************************
   // 函数：DisableApm
   // 功能：禁用指定索引磁盘的APM（高级电源管理）功能
   // 参数：
   //  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
   // 返回值：BOOL类型，TRUE表示禁用成功，FALSE表示失败（如磁盘不支持APM、操作超时）
   // ******************************************************************************************
	BOOL DisableApm(DWORD index);
	// ******************************************************************************************
	// 函数：GetRecommendAamValue
	// 功能：获取指定索引磁盘的推荐AAM值（基于磁盘型号和厂商优化的噪音/性能平衡值）
	// 参数：
	//  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
	// 返回值：BYTE类型，推荐的AAM值（若磁盘不支持AAM，可能返回0或无效值）
	// ******************************************************************************************
	BYTE GetRecommendAamValue(DWORD index);
	// ******************************************************************************************
   // 函数：GetRecommendApmValue
   // 功能：获取指定索引磁盘的推荐APM值（基于磁盘型号和厂商优化的功耗/性能平衡值）
   // 参数：
   //  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
   // 返回值：BYTE类型，推荐的APM值（若磁盘不支持APM，可能返回0或无效值）
   // ******************************************************************************************
	BYTE GetRecommendApmValue(DWORD index);
	// ******************************************************************************************
	// 函数：Init
	// 功能：初始化CAtaSmart类的核心配置（设置磁盘枚举、SMART读取的关键参数，为后续操作做准备）
	// 参数：
	//  - useWmi：是否使用WMI接口读取SMART数据（TRUE=使用WMI，FALSE=使用底层IO控制）
	//  - advancedDiskSearch：是否启用高级磁盘搜索（TRUE=枚举更多类型磁盘，如RAID下的物理盘；FALSE=仅枚举基础磁盘）
	//  - flagChangeDisk：磁盘变化标志指针（用于外部感知磁盘列表是否有新增/移除）
	//  - workaroundHD204UI：是否启用HD204UI磁盘兼容性适配（针对该型号磁盘的读取bug修复）
	//  - workaroundAdataSsd：是否启用威刚SSD兼容性适配（针对威刚SSD的SMART读取异常修复）
	//  - flagHideNoSmartDisk：是否隐藏不支持SMART的磁盘（TRUE=仅显示支持SMART的磁盘，FALSE=显示所有磁盘）
	// 返回值：VOID类型，无返回值
	// ******************************************************************************************
	VOID Init(BOOL useWmi, BOOL advancedDiskSearch, PBOOL flagChangeDisk, BOOL workaroundHD204UI, BOOL workaroundAdataSsd, BOOL flagHideNoSmartDisk);
	// ******************************************************************************************
	// 函数：MeasuredTimeUnit
	// 功能：检测磁盘SMART数据中“通电时间”的计量单位（确定磁盘返回的原始通电时间是小时、分钟还是其他单位）
	// 返回值：BOOL类型，TRUE表示检测成功并确定单位，FALSE表示检测失败（使用默认单位小时）
	// ******************************************************************************************
	BOOL MeasuredTimeUnit();
	// ******************************************************************************************
	// 函数：GetPowerOnHours
	// 功能：将磁盘SMART中的原始通电时间值转换为指定单位的时间（基于已知的时间单位类型）
	// 参数：
	//  - rawValue：SMART中读取的原始通电时间值（未转换的原始数据）
	//  - timeUnitType：目标时间单位类型（参考POWER_ON_HOURS_UNIT枚举，如小时、分钟）
	// 返回值：DWORD类型，转换后的通电时间值（单位由timeUnitType指定）
	// ******************************************************************************************
	DWORD GetPowerOnHours(DWORD rawValue, DWORD timeUnitType);
	// ******************************************************************************************
	// 函数：GetPowerOnHoursEx
	// 功能：获取指定索引磁盘的通电时间（自动读取该磁盘的SMART原始值并转换为指定单位）
	// 参数：
	//  - index：磁盘索引（对应系统中枚举的磁盘序号，0开始）
	//  - timeUnitType：目标时间单位类型（参考POWER_ON_HOURS_UNIT枚举，如小时、分钟）
	// 返回值：DWORD类型，转换后的磁盘通电时间值（获取失败时返回0）
	DWORD GetPowerOnHoursEx(DWORD index, DWORD timeUnitType);
	// ******************************************************************************************
	// 函数：SetAtaPassThroughSmart
	// 功能：设置是否使用ATA命令透传方式读取SMART数据（控制SMART数据的读取协议）
	// 参数：
	//  - flag：TRUE=启用ATA命令透传（直接向ATA/SATA设备发送SMART命令），FALSE=禁用（使用其他方式如SCSI转发）
	// 返回值：VOID类型，无返回值
	// 说明：ATA命令透传兼容性更强，适合大多数原生ATA/SATA设备，尤其在RAID环境外使用
	// ******************************************************************************************
	VOID SetAtaPassThroughSmart(BOOL flag);

	// ******************************************************************************************
	// 结构体：DISK_POSITION
	// 功能：存储磁盘的硬件位置信息（用于在多设备环境中精准定位物理磁盘）
	// 说明：整合SCSI总线、端口、目标ID及SAS物理实体信息，支持传统SATA/SCSI与SAS磁盘的定位
	// ******************************************************************************************
	struct DISK_POSITION
	{
		INT                 PhysicalDriveId;  // 物理磁盘ID（系统分配的唯一磁盘标识，用于关联系统级磁盘对象）
		INT                 ScsiPort;         // SCSI端口号（磁盘连接的SCSI控制器端口，多控制器时区分端口）
		INT                 ScsiTargetId;     // SCSI目标ID（SCSI总线上的设备编号，同一端口下区分不同磁盘）
		CSMI_SAS_PHY_ENTITY sasPhyEntity;     // SAS物理实体信息（SAS磁盘专属，包含SAS端口、链路状态等细节）
	};

	// ******************************************************************************************
	// 联合体：IDENTIFY_DEVICE
	// 功能：统一封装不同类型磁盘的设备识别数据（支持ATA/SATA、NVMe及原始二进制格式）
	// 说明：通过联合体共用内存，根据磁盘类型（ATA/NVMe）选择对应的解析结构，避免内存冗余
	// ******************************************************************************************
	union IDENTIFY_DEVICE
	{
		ATA_IDENTIFY_DEVICE A;   // ATA/SATA设备识别结构（存储ATA协议下的磁盘参数，如型号、容量）
		NVME_IDENTIFY_DEVICE N;  // NVMe设备识别结构（存储NVMe协议下的磁盘参数，如序列号、固件版本）
		BIN_IDENTIFY_DEVICE B;   // 二进制原始识别数据（未解析的原始字节流，用于特殊场景或兼容处理）
	};

	// ******************************************************************************************
	// 结构体：ATA_SMART_INFO
	// 功能：存储单块磁盘的完整SMART信息与设备属性（整合SMART数据、硬件参数、状态标识等）
	// 说明：磁盘管理模块的核心数据结构，集中管理磁盘的健康状态、硬件特性、协议支持等所有关键信息
	// ******************************************************************************************
	struct ATA_SMART_INFO
	{
		IDENTIFY_DEVICE     IdentifyDevice;       // 设备识别数据（根据磁盘类型选择ATA/NVMe/二进制解析）
		BYTE                SmartReadData[512];   // 512字节SMART属性原始数据（从磁盘读取的未解析SMART缓冲区）
		BYTE                SmartReadThreshold[512]; // 512字节SMART阈值原始数据（各SMART属性的警告阈值缓冲区）
		SMART_ATTRIBUTE     Attribute[MAX_ATTRIBUTE]; // SMART属性数组（解析后的SMART属性集合，如扇区重映射、温度）
		SMART_THRESHOLD     Threshold[MAX_ATTRIBUTE]; // SMART阈值数组（解析后的SMART阈值集合，对应属性的警告临界值）

		// SMART数据有效性与正确性标识
		BOOL                IsSmartEnabled;       // 是否启用SMART功能（磁盘是否开启SMART监测）
		BOOL                IsIdInfoIncorrect;    // 设备识别信息是否错误（如型号、序列号读取异常）
		BOOL                IsSmartCorrect;       // SMART属性数据是否正确（解析后的数据是否可用）
		BOOL                IsThresholdCorrect;   // SMART阈值数据是否正确（解析后的阈值是否可用）
		BOOL                IsCheckSumError;      // 是否存在校验和错误（SMART数据传输或解析时的校验失败）
		BOOL                IsWord88;             // 是否支持ATA协议的88h字段（标识磁盘的特殊功能支持）
		BOOL                IsWord64_76;          // 是否支持ATA协议的64-76h字段（标识磁盘的扩展特性）
		BOOL                IsRawValues8;         // SMART原始值是否为8字节格式（部分磁盘的原始值存储格式）
		BOOL                IsRawValues7;         // SMART原始值是否为7字节格式（部分老旧磁盘的存储格式）
		BOOL                Is9126MB;             // 是否为9126MB容量限制的磁盘（早期ATA磁盘的容量兼容性标识）
		BOOL                IsThresholdBug;       // 是否存在阈值bug（部分磁盘的阈值数据读取异常）

		// 磁盘功能支持标识
		BOOL                IsSmartSupported;     // 是否支持SMART功能（磁盘硬件是否具备SMART能力）
		BOOL                IsLba48Supported;     // 是否支持LBA48寻址（是否支持超过137GB的大容量存储）
		BOOL                IsAamSupported;       // 是否支持AAM（自动声学管理，控制磁盘噪音）
		BOOL                IsApmSupported;       // 是否支持APM（高级电源管理，控制磁盘功耗）
		BOOL                IsAamEnabled;         // 当前是否启用AAM功能
		BOOL                IsApmEnabled;         // 当前是否启用APM功能
		BOOL                IsNcqSupported;       // 是否支持NCQ（ Native Command Queuing，原生命令队列）
		BOOL                IsNvCacheSupported;   // 是否支持NV缓存（非易失性缓存，提升读写性能）
		BOOL                IsDeviceSleepSupported; // 是否支持设备休眠（磁盘低功耗休眠模式）
		BOOL                IsMaxtorMinute;       // 是否为Maxtor磁盘的分钟级通电时间（Maxtor磁盘的时间单位特殊标识）
		BOOL                IsSsd;                // 是否为SSD（固态硬盘，区分HDD机械硬盘）
		BOOL                IsTrimSupported;      // 是否支持TRIM指令（SSD专属，优化垃圾回收）
		BOOL                IsVolatileWriteCachePresent; // 是否存在易失性写缓存（断电丢失数据的缓存）

		// 磁盘类型与协议标识
		BOOL                IsNVMe;               // 是否为NVMe协议磁盘（区分ATA/SATA协议）
		BOOL                IsUasp;               // 是否支持UASP协议（USB存储的加速协议）

		// 磁盘硬件定位信息
		INT                 PhysicalDriveId;      // 物理磁盘ID（系统级唯一标识）
		INT                 ScsiPort;             // SCSI端口号（连接的控制器端口）
		INT                 ScsiTargetId;         // SCSI目标ID（总线上的设备编号）
		INT                 ScsiBus;              // SCSI总线号（多总线系统中的总线标识）
		INT                 SiliconImageType;     // Silicon Image控制器类型（针对该品牌控制器的兼容标识）

		// 磁盘容量与存储参数
		DWORD               TotalDiskSize;        // 磁盘总容量（单位：MB，综合多种方式检测的容量）
		DWORD               Cylinder;             // 磁盘柱面数（CHS寻址方式的参数，已逐步淘汰）
		DWORD               Head;                 // 磁盘磁头数（CHS寻址方式的参数）
		DWORD               Sector;               // 每磁道扇区数（CHS寻址方式的参数）
		DWORD               Sector28;             // LBA28寻址支持的扇区数（对应最大137GB容量）
		ULONGLONG           Sector48;             // LBA48寻址支持的扇区数（对应大容量存储）
		ULONGLONG           NumberOfSectors;      // 磁盘总扇区数（物理扇区总数）
		DWORD               DiskSizeChs;          // CHS寻址方式计算的容量（单位：MB）
		DWORD               DiskSizeLba28;        // LBA28寻址方式计算的容量（单位：MB）
		DWORD               DiskSizeLba48;        // LBA48寻址方式计算的容量（单位：MB）
		DWORD               LogicalSectorSize;    // 逻辑扇区大小（单位：字节，通常为512字节）
		DWORD               PhysicalSectorSize;   // 物理扇区大小（单位：字节，如4096字节高级格式化）
		DWORD               DiskSizeWmi;          // 通过WMI接口获取的磁盘容量（单位：MB）
		DWORD               BufferSize;           // 磁盘缓存大小（单位：KB，如8MB、16MB）
		ULONGLONG           NvCacheSize;          // NV缓存大小（单位：字节，非易失性缓存容量）

		// 磁盘性能与时间参数
		DWORD               TransferModeType;     // 传输模式类型（如UDMA Mode 6、SATA III等）
		DWORD               DetectedTimeUnitType; // 检测到的时间单位类型（通电时间的原始单位）
		DWORD               MeasuredTimeUnitType; // 校准后的时间单位类型（用于准确计算通电时间）
		DWORD               AttributeCount;       // SMART属性数量（当前磁盘支持的SMART属性总数）
		INT                 DetectedPowerOnHours; // 检测到的通电时间（基于原始数据计算）
		INT                 MeasuredPowerOnHours; // 校准后的通电时间（修正单位后的准确值）
		INT                 PowerOnRawValue;      // 通电时间原始值（SMART中读取的未修正数据）
		INT                 PowerOnStartRawValue; // 通电时间起始原始值（用于计算累计时间）
		DWORD               PowerOnCount;         // 通电次数（磁盘累计的上电次数）
		INT                 Temperature;          // 磁盘当前温度（单位：摄氏度，SMART检测值）
		double              TemperatureMultiplier;// 温度乘数（部分磁盘温度值的校准系数）
		DWORD               NominalMediaRotationRate; // 磁盘标称转速（单位：RPM，如7200、5400，SSD为0）
		INT                 HostWrites;           // 主机写入量（累计从主机写入的数据量）
		INT                 HostReads;            // 主机读取量（累计从主机读取的数据量）
		INT                 GBytesErased;         // SSD擦除量（累计擦除的GB数，SSD寿命关键指标）
		INT                 NandWrites;           // NAND闪存写入量（SSD内部NAND的累计写入量）
		INT                 WearLevelingCount;    // 磨损均衡计数（SSD磨损均衡算法的状态值）

		// SSD寿命相关标识
		INT                 Life;                 // SSD剩余寿命（百分比，如100表示全新，0表示寿命耗尽）
		BOOL                FlagLifeRawValue;     // 寿命原始值标识（是否使用原始值计算寿命）
		BOOL                FlagLifeRawValueIncrement; // 寿命原始值递增标识（原始值是否随使用递增）
		BOOL                FlagLifeSanDisk0_1;   // SanDisk SSD寿命计算标识1（针对该品牌的特殊计算逻辑）
		BOOL                FlagLifeSanDisk1;     // SanDisk SSD寿命计算标识2（补充特殊计算逻辑）
		BOOL                FlagLifeSanDiskLenovo; // 联想定制SanDisk SSD寿命标识（针对定制盘的计算逻辑）

		// 协议版本信息
		DWORD               Major;                // 主版本号（磁盘支持的协议主版本，如ATA-8的8）
		DWORD               Minor;                // 次版本号（协议次版本，如ATA-8-ACS的ACS版本）

		// 磁盘状态与告警信息
		DWORD               DiskStatus;           // 磁盘状态（如正常、警告、错误，对应SMART健康状态）
		DWORD               DriveLetterMap;       // 驱动器号映射（位图形式，标识磁盘对应的系统驱动器号）
		INT                 AlarmTemperature;     // 温度告警阈值（超过该温度触发告警）
		BOOL                AlarmHealthStatus;    // 健康状态告警（是否触发SMART健康告警）

		// 接口与命令类型
		INTERFACE_TYPE      InterfaceType;        // 磁盘接口类型（如SATA、NVMe、USB，对应INTERFACE_TYPE枚举）
		COMMAND_TYPE        CommandType;          // 命令类型（如ATA命令、SCSI命令，标识通信协议）
		HOST_READS_WRITES_UNIT HostReadsWritesUnit; // 主机读写量单位（标识HostWrites/HostReads的单位，如GB、TB）

		// 厂商与硬件标识
		DWORD               DiskVendorId;         // 磁盘厂商ID（如Seagate、Western Digital的厂商编码）
		DWORD               UsbVendorId;          // USB厂商ID（USB接口磁盘的厂商编码，如0x0781为SanDisk）
		DWORD               UsbProductId;         // USB产品ID（USB磁盘的产品编码，区分具体型号）
		BYTE                Target;               // 目标设备标识（部分总线中的设备标识，如SAS目标）

		// 关键SMART属性阈值（常用属性的阈值缓存，便于快速访问）
		WORD                Threshold05;          // 属性05h（重映射扇区计数）的阈值
		WORD                ThresholdC5;          // 属性C5h（当前待映射扇区计数）的阈值
		WORD                ThresholdC6;          // 属性C6h（无法校正的扇区计数）的阈值
		WORD                ThresholdFF;          // 属性FFh（厂商自定义属性）的阈值

		// SAS磁盘专属信息
		CSMI_SAS_PHY_ENTITY sasPhyEntity;         // SAS物理实体信息（包含SAS链路速度、端口状态等）

		// 字符串形式的设备信息（便于显示与日志输出）
		CString             SerialNumber;         // 磁盘序列号（字符串格式，如"1234567890ABCDEF"）
		CString             SerialNumberReverse;  // 反转的序列号（部分磁盘序列号存储为反转格式）
		CString             FirmwareRev;          // 固件版本（字符串格式，如"AX01"）
		CString             FirmwareRevReverse;   // 反转的固件版本（部分磁盘固件版本存储为反转格式）
		CString             Model;                // 磁盘型号（字符串格式，如"ST1000DM010-2EP102"）
		CString             ModelReverse;         // 反转的型号（部分磁盘型号存储为反转格式）
		CString             ModelWmi;             // 通过WMI获取的型号（补充WMI接口的型号信息）
		CString             ModelSerial;          // 型号+序列号组合（用于唯一标识磁盘的字符串）
		CString             DriveMap;             // 驱动器号映射字符串（如"C:, D:"，直观显示驱动器号）
		CString             MaxTransferMode;      // 最大传输模式（字符串格式，如"UDMA Mode 6"）
		CString             CurrentTransferMode;  // 当前传输模式（字符串格式，如"SATA III 6.0 Gb/s"）
		CString             MajorVersion;         // 主版本号字符串（如"ATA-8"）
		CString             MinorVersion;         // 次版本号字符串（如"ACS-3"）
		CString             Interface;            // 接口类型字符串（如"SATA III"、"NVMe 1.4"）
		CString             Enclosure;            // 磁盘柜信息（如"External USB Enclosure"，外部设备的柜标识）
		CString             CommandTypeString;    // 命令类型字符串（如"ATA Command Set"）
		CString             SsdVendorString;      // SSD厂商字符串（如"Samsung SSD"，区分SSD厂商）
		CString             DeviceNominalFormFactor; // 设备标称尺寸（如"2.5 inches"、"M.2 2280"）
		CString             PnpDeviceId;          // PnP设备ID（即插即用设备ID，用于系统识别设备）

		CString             SmartKeyName;         // SMART注册表键名（存储磁盘SMART配置的注册表路径）
	};

	// ******************************************************************************************
	// 结构体：EXTERNAL_DISK_INFO
	// 功能：存储外部磁盘（如USB、eSATA磁盘）的专属信息（聚焦外部设备的物理与厂商标识）
	// 说明：用于管理外部存储设备的附加信息，辅助区分不同外部设备的来源与类型
	// ******************************************************************************************
	struct EXTERNAL_DISK_INFO
	{
		CString Enclosure;       // 磁盘柜名称（标识外部设备的物理容器，如"Western Digital My Passport"）
		DWORD   UsbVendorId;     // USB厂商ID（外部USB磁盘的厂商编码，用于识别厂商）
		DWORD   UsbProductId;    // USB产品ID（外部USB磁盘的产品编码，用于识别具体型号）
	};

	// ******************************************************************************************
	// 磁盘信息数组（存储系统中所有磁盘的完整SMART与属性数据）
	// ******************************************************************************************
	CArray<ATA_SMART_INFO, ATA_SMART_INFO> vars;  // ATA_SMART_INFO类型数组，每个元素对应一块磁盘的完整信息（SMART数据、硬件参数、状态标识等）
	CArray<EXTERNAL_DISK_INFO, EXTERNAL_DISK_INFO> externals;  // EXTERNAL_DISK_INFO类型数组，存储所有外部磁盘（如USB磁盘）的专属信息（磁盘柜、USB厂商/产品ID）

	// ******************************************************************************************
	// 控制器信息数组（存储系统中各类存储控制器的名称/标识，用于设备兼容性判断）
	// ******************************************************************************************
	CStringArray m_IdeController;               // IDE控制器名称数组（存储系统中所有IDE控制器的设备名称，如"Intel(R) 82801H (ICH8 Family) IDE Controller"）
	CStringArray m_ScsiController;              // SCSI控制器名称数组（存储系统中所有SCSI/SAS控制器的设备名称，如"LSI MegaRAID SAS 9300-8i"）
	CStringArray m_UsbController;               // USB控制器名称数组（存储系统中所有USB控制器的设备名称，如"Intel(R) USB 3.0 eXtensible Host Controller"）
	CString m_ControllerMap;                    // 控制器映射字符串（汇总所有控制器的关联关系，如“IDE控制器0->磁盘1,2；USB控制器1->磁盘3”，用于日志或显示）
	CStringArray m_BlackIdeController;          // IDE控制器黑名单数组（存储不支持SMART读取的IDE控制器名称，避免对这些控制器下的磁盘执行无效操作）
	CStringArray m_BlackScsiController;         // SCSI控制器黑名单数组（存储不支持SMART读取的SCSI控制器名称，过滤无效设备）
	CStringArray m_SiliconImageController;      // Silicon Image控制器名称数组（专门存储该品牌的IDE/SATA控制器，用于针对性兼容处理）
	CStringArray m_UASPController;              // UASP协议控制器数组（存储支持UASP协议的USB控制器，用于优化USB磁盘的SMART读取方式）
	CArray<DWORD, DWORD> m_SiliconImageControllerType;  // Silicon Image控制器类型数组（与m_SiliconImageController对应，存储每个控制器的具体型号/类型，用于细分兼容逻辑）
	CArray<INT, INT> m_BlackPhysicalDrive;      // 物理磁盘黑名单数组（存储不支持SMART或读取异常的磁盘ID，过滤这些磁盘以避免程序报错）

	// ******************************************************************************************
	// 功能开关与兼容性配置（控制程序核心逻辑与硬件兼容策略）
	// ******************************************************************************************
	BOOL IsAdvancedDiskSearch;                  // 是否启用高级磁盘搜索（TRUE=枚举所有类型磁盘，包括RAID下的物理盘、外部磁盘；FALSE=仅枚举基础内置磁盘）
	BOOL IsEnabledWmi;                          // 是否启用WMI接口（TRUE=通过WMI读取部分磁盘信息；FALSE=仅使用底层IO控制方式，兼容性更强）

	BOOL IsWorkaroundHD204UI;                   // 是否启用HD204UI磁盘兼容适配（TRUE=针对该型号磁盘的SMART读取bug进行修复；FALSE=不启用适配，可能导致读取失败）
	BOOL IsWorkaroundAdataSsd;                  // 是否启用威刚SSD兼容适配（TRUE=针对威刚部分SSD的SMART数据异常进行校正；FALSE=不启用适配，数据可能不准确）

	// ******************************************************************************************
	// 时间与状态标识（用于计时与硬件特性判断）
	// ******************************************************************************************
	ULONGLONG MeasuredGetTickCount;             // 计时戳（存储上次执行“检测时间单位”操作的系统Tick计数，用于控制检测频率，避免重复耗时操作）

	BOOL FlagNvidiaController;                  // 是否存在NVIDIA存储控制器（TRUE=系统中有NVIDIA芯片组的IDE/SATA控制器，需针对性兼容）
	BOOL FlagMarvellController;                 // 是否存在Marvell存储控制器（TRUE=系统中有Marvell芯片组的控制器，需特殊处理SMART读取）
	BOOL FlagUsbSat;                            // 是否存在USB-SATA桥接控制器（TRUE=系统中有USB转SATA的桥接设备，需按USB协议处理SMART）
	BOOL FlagUsbSunplus;                        // 是否存在Sunplus芯片的USB控制器（TRUE=针对该芯片的USB设备进行兼容优化）
	BOOL FlagUsbIodata;                         // 是否存在I-O DATA品牌的USB设备（TRUE=针对该品牌USB磁盘的SMART读取逻辑调整）
	BOOL FlagUsbLogitec;                        // 是否存在Logitec品牌的USB设备（TRUE=针对该品牌USB磁盘的兼容处理）
	BOOL FlagUsbProlific;                       // 是否存在Prolific芯片的USB控制器（TRUE=针对该芯片的USB转串口/并口设备进行过滤或适配）
	BOOL FlagUsbJmicron;                        // 是否存在JMicron芯片的USB控制器（TRUE=针对该芯片的USB桥接设备优化SMART读取）
	BOOL FlagUsbCypress;                        // 是否存在Cypress芯片的USB控制器（TRUE=针对该芯片的USB设备进行兼容处理）
	BOOL FlagUsbMemory;                         // 是否存在USB闪存盘（TRUE=系统中有USB闪存设备，这类设备通常不支持SMART，需过滤）
	// BOOL FlagUsbSat16;                         // （注释）是否存在16位USB-SATA桥接器（预留或废弃的兼容标识）
	BOOL FlagUsbNVMeJMicron;                    // 是否存在JMicron芯片的USB-NVMe设备（TRUE=针对该芯片的USB转NVMe设备优化SMART读取）
	BOOL FlagUsbNVMeASMedia;                    // 是否存在ASMedia芯片的USB-NVMe设备（TRUE=针对该芯片的USB转NVMe设备优化SMART读取）
	BOOL FlagUsbNVMeRealtek;                    // 是否存在Realtek芯片的USB-NVMe设备（TRUE=针对该芯片的USB转NVMe设备优化SMART读取）
	BOOL FlagMegaRAID;                          // 是否存在LSI MegaRAID阵列卡（TRUE=系统中有MegaRAID阵列卡，需使用MegaRAID专属协议读取物理盘SMART）
	BOOL FlagUsbASM1352R;                       // 是否存在ASMedia 1352R芯片的USB控制器（TRUE=针对该芯片的USB 3.1控制器优化SMART读取）

	// ******************************************************************************************
	// 控制器类型与函数（CSMI协议相关与磁盘状态检测）
	// ******************************************************************************************
	DWORD CsmiType;                              // CSMI（Common Storage Management Interface）协议类型（标识系统支持的CSMI版本，用于SAS/SATA控制器的SMART管理）

	// ******************************************************************************************
	// 函数：CheckDiskStatus
	// 功能：检测指定索引磁盘的健康状态（基于SMART属性、阈值及硬件特性，判断磁盘是否正常/警告/错误）
	// 参数：
	//  - index：磁盘索引（对应vars数组的下标，0开始，指定要检测的磁盘）
	// 返回值：DWORD类型，磁盘状态码（具体值需参考对应的状态枚举，如0=正常，1=警告，2=错误）
	// 说明：核心检测逻辑包括：SMART属性是否超过阈值、关键属性（如重映射扇区）是否异常、磁盘是否存在硬件错误等
	// ******************************************************************************************
	DWORD CheckDiskStatus(DWORD index);

protected:
	// ******************************************************************************************
	// 系统与配置变量（存储系统版本、磁盘标识、命令传输模式等核心配置）
	// ******************************************************************************************
	OSVERSIONINFOEX m_Os;                          // 系统版本信息结构体（存储当前操作系统的版本、构建号、服务包等，用于兼容性判断，如Win10/Win11差异化处理）
	CString m_SerialNumberA_Z[26];                 // 磁盘序列号按字母分类数组（A-Z共26个元素，分别存储以对应字母开头的磁盘序列号，用于快速检索）
	BOOL m_bAtaPassThrough;                        // 是否启用ATA命令透传（通用ATA命令透传开关，控制是否直接向ATA设备发送底层命令）
	BOOL m_bAtaPassThroughSmart;                   // 是否启用ATA命令透传读取SMART（专门控制SMART数据读取是否使用ATA透传方式，优先级高于通用透传）
	BOOL m_bNVMeStorageQuery;                      // 是否使用Storage Query接口读取NVMe信息（Win10及以上系统的NVMe设备专用接口开关，兼容性更强）

	// ******************************************************************************************
	// 磁盘枚举与管理核心函数（磁盘信息获取、添加、SMART属性校验）
	// ******************************************************************************************
	// 函数：GetDiskInfo
	// 功能：获取指定硬件标识磁盘的详细信息（根据物理ID、SCSI端口等定位磁盘，读取设备识别与SMART数据）
	// 参数：
	//  - physicalDriveId：物理磁盘ID（系统分配的磁盘唯一标识）
	//  - scsiPort：SCSI端口号（磁盘连接的控制器端口）
	//  - scsiTargetId：SCSI目标ID（总线上的设备编号）
	//  - interfaceType：接口类型（如SATA、NVMe，对应INTERFACE_TYPE枚举）
	//  - commandType：命令类型（如ATA、SCSI，对应COMMAND_TYPE枚举）
	//  - vendorId：厂商ID（磁盘或控制器的厂商编码）
	//  - productId：产品ID（可选，磁盘或控制器的产品编码，默认0）
	//  - scsiBus：SCSI总线号（可选，默认-1，多总线系统中定位总线）
	//  - siliconImageId：Silicon Image控制器ID（可选，默认0，针对该品牌控制器的兼容标识）
	//  - FlagNvidiaController：是否为NVIDIA控制器（可选，默认0，用于NVIDIA控制器的兼容处理）
	//  - FlagMarvellController：是否为Marvell控制器（可选，默认0，用于Marvell控制器的兼容处理）
	//  - pnpDeviceId：PnP设备ID（可选，默认空，用于系统级设备识别）
	//  - FlagNVMe：是否为NVMe磁盘（可选，默认FALSE，区分ATA与NVMe设备）
	//  - FlagUasp：是否支持UASP协议（可选，默认FALSE，USB磁盘的加速协议标识）
	// 返回值：BOOL类型，TRUE=信息获取成功，FALSE=失败
	// ******************************************************************************************
	BOOL GetDiskInfo(INT physicalDriveId, INT scsiPort, INT scsiTargetId, INTERFACE_TYPE interfaceType, COMMAND_TYPE commandType, VENDOR_ID vendorId, DWORD productId = 0, INT scsiBus = -1, DWORD siliconImageId = 0, BOOL FlagNvidiaController = 0, BOOL FlagMarvellController = 0, CString pnpDeviceId = _T(""), BOOL FlagNVMe = FALSE, BOOL FlagUasp = FALSE);
	// 函数：AddDisk
	// 功能：将获取到的磁盘信息添加到磁盘列表（vars数组），完成磁盘信息的初始化与存储
	// 参数：
	//  - PhysicalDriveId：物理磁盘ID
	//  - ScsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - scsiBus：SCSI总线号
	//  - target：目标设备标识（部分总线的设备标识）
	//  - commandType：命令类型（ATA/SCSI等）
	//  - identify：设备识别数据指针（指向IDENTIFY_DEVICE联合体，包含ATA/NVMe识别信息）
	//  - siliconImageType：Silicon Image控制器类型（可选，默认-1，无该控制器时为-1）
	//  - sasPhyEntity：SAS物理实体信息指针（可选，默认NULL，SAS磁盘专属信息）
	//  - pnpDeviceId：PnP设备ID（可选，默认空，系统设备ID）
	// 返回值：BOOL类型，TRUE=添加成功，FALSE=失败（如磁盘已存在、参数无效）
	// ******************************************************************************************
	BOOL AddDisk(INT PhysicalDriveId, INT ScsiPort, INT scsiTargetId, INT scsiBus, BYTE target, COMMAND_TYPE commandType, IDENTIFY_DEVICE* identify, INT siliconImageType = -1, PCSMI_SAS_PHY_ENTITY sasPhyEntity = NULL, CString pnpDeviceId = _T(""));
	// 函数：CheckSmartAttributeUpdate
	// 功能：对比两次SMART属性数据，检测属性是否有更新（判断磁盘状态是否变化）
	// 参数：
	//  - index：磁盘索引（对应vars数组下标，标识要检测的磁盘）
	//  - pre：旧SMART属性指针（更新前的属性数据）
	//  - cur：新SMART属性指针（更新后的属性数据）
	// 返回值：DWORD类型，更新状态码（如0=无更新，1=属性值变化，2=关键属性异常）
	// 说明：核心对比逻辑包括属性当前值、原始值是否变化，重点关注重映射扇区、温度等关键属性
	// ******************************************************************************************
	DWORD CheckSmartAttributeUpdate(DWORD index, SMART_ATTRIBUTE* pre, SMART_ATTRIBUTE* cur);
	// 函数：CheckSmartAttributeCorrect
	// 功能：校验两个磁盘的SMART属性数据是否一致（用于交叉验证SMART数据的准确性）
	// 参数：
	//  - asi1：第一个磁盘的SMART信息指针（ATA_SMART_INFO结构体）
	//  - asi2：第二个磁盘的SMART信息指针（用于对比的参考数据）
	// 返回值：BOOL类型，TRUE=数据一致（校验通过），FALSE=数据不一致（存在异常）
	// ******************************************************************************************
	BOOL CheckSmartAttributeCorrect(ATA_SMART_INFO* asi1, ATA_SMART_INFO* asi2);
	// ******************************************************************************************
	// 磁盘控制与数据处理辅助函数（唤醒、字节序转换、设备句柄获取）
	// ******************************************************************************************
	// 函数：WakeUp
	// 功能：唤醒指定物理磁盘（将休眠状态的磁盘唤醒，以便执行后续读取操作）
	// 参数：
	//  - physicalDriveId：物理磁盘ID（标识要唤醒的磁盘）
	// 返回值：VOID类型，无返回值
	// ******************************************************************************************
	VOID WakeUp(INT physicalDriveId);
	
	//	VOID InitAtaInfo();
	//	VOID InitAtaInfoByWmi();
	//	VOID InitStruct();
	
	// 函数：ChangeByteOrder
	// 功能：转换字符串的字节序（部分磁盘的序列号/型号存储为小端字节序，需转换为正常ASCII顺序）
	// 参数：
	//  - str：字符串指针（要转换的原始字符串）
	//  - length：字符串长度（字节数，需转换的字符个数）
	// 返回值：VOID类型，无返回值
	// ******************************************************************************************
	VOID ChangeByteOrder(PCHAR str, DWORD length);
	// 函数：CheckAsciiStringError
	// 功能：检查字符串是否为有效ASCII（过滤乱码或无效字符，确保磁盘型号/序列号显示正常）
	// 参数：
	//  - str：字符串指针（要检查的字符串）
	//  - length：字符串长度（字节数）
	// 返回值：BOOL类型，TRUE=存在无效字符（错误），FALSE=均为有效ASCII（正常）
	// ******************************************************************************************
	BOOL CheckAsciiStringError(PCHAR str, DWORD length);
	// 函数：GetIoCtrlHandle（重载1）
	// 功能：通过磁盘索引获取IO控制句柄（用于后续向磁盘发送IO控制命令）
	// 参数：
	//  - index：磁盘索引（对应vars数组下标，标识目标磁盘）
	// 返回值：HANDLE类型，IO控制句柄（成功则返回有效句柄，失败返回INVALID_HANDLE_VALUE）
	// ******************************************************************************************
	HANDLE GetIoCtrlHandle(BYTE index);
	// 函数：GetIoCtrlHandle（重载2）
	// 功能：通过SCSI端口与Silicon Image控制器类型获取IO控制句柄（针对该品牌控制器的磁盘）
	// 参数：
	//  - scsiPort：SCSI端口号
	//  - siliconImageType：Silicon Image控制器类型
	// 返回值：HANDLE类型，IO控制句柄（成功则返回有效句柄，失败返回INVALID_HANDLE_VALUE）
	// ******************************************************************************************
	HANDLE GetIoCtrlHandle(INT scsiPort, DWORD siliconImageType);
	// 函数：GetIoCtrlHandleCsmi
	// 功能：通过SCSI端口获取CSMI协议的IO控制句柄（用于CSMI协议下的SMART操作）
	// 参数：
	//  - scsiPort：SCSI端口号（对应CSMI控制器的端口）
	// 返回值：HANDLE类型，IO控制句柄（成功则返回有效句柄，失败返回INVALID_HANDLE_VALUE）
	// ******************************************************************************************
	HANDLE GetIoCtrlHandleCsmi(INT scsiPort);
	// 函数：GetIoCtrlHandleMegaRAID
	// 功能：通过SCSI端口获取MegaRAID阵列卡的IO控制句柄（用于MegaRAID下的磁盘操作）
	// 参数：
	//  - scsiPort：SCSI端口号（对应MegaRAID控制器的端口）
	// 返回值：HANDLE类型，IO控制句柄（成功则返回有效句柄，失败返回INVALID_HANDLE_VALUE）
	// ******************************************************************************************
	HANDLE GetIoCtrlHandleMegaRAID(INT scsiPort);
	// 函数：SendAtaCommand
	// 功能：向指定索引的磁盘发送ATA命令（执行底层ATA操作，如读取SMART、设置APM等）
	// 参数：
	//  - i：磁盘索引（对应vars数组下标）
	//  - main：ATA主命令码（如0x92=读取SMART数据）
	//  - sub：ATA子命令码（补充主命令的细分操作）
	//  - param：命令参数（根据命令类型传递的配置参数）
	// 返回值：BOOL类型，TRUE=命令发送成功，FALSE=失败（如句柄无效、命令不支持）
	// ******************************************************************************************
	BOOL SendAtaCommand(DWORD i, BYTE main, BYTE sub, BYTE param);
	// ******************************************************************************************
	// PD（Physical Drive）模式专用函数（直接操作物理磁盘的ATA命令）
	// ******************************************************************************************
	// 函数：DoIdentifyDevicePd
	// 功能：在PD模式下读取物理磁盘的设备识别信息（获取型号、序列号等基础信息）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - identify：设备识别数据指针（存储读取到的识别信息）
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL DoIdentifyDevicePd(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE* identify);
	// 函数：GetSmartAttributePd
	// 功能：在PD模式下读取物理磁盘的SMART属性数据
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - asi：SMART信息指针（存储读取到的SMART属性）
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL GetSmartAttributePd(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi);
	// 函数：GetSmartThresholdPd
	// 功能：在PD模式下读取物理磁盘的SMART阈值数据
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - asi：SMART信息指针（存储读取到的SMART阈值）
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL GetSmartThresholdPd(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi);
	// 函数：ControlSmartStatusPd
	// 功能：在PD模式下控制物理磁盘的SMART功能状态（启用/禁用SMART）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - command：控制命令（如启用SMART=0x10，禁用SMART=0x00）
	// 返回值：BOOL类型，TRUE=控制成功，FALSE=失败
	// ******************************************************************************************
	BOOL ControlSmartStatusPd(INT physicalDriveId, BYTE target, BYTE command);
	// 函数：SendAtaCommandPd
	// 功能：在PD模式下向物理磁盘发送ATA命令（带数据传输）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - main：ATA主命令码
	//  - sub：ATA子命令码
	//  - param：命令参数
	//  - data：数据缓冲区指针（存储命令的输入/输出数据）
	//  - dataSize：数据缓冲区大小（字节数）
	// 返回值：BOOL类型，TRUE=命令发送成功，FALSE=失败
	// ******************************************************************************************
	BOOL SendAtaCommandPd(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize);
	// 函数：ReadLogExtPd
	// 功能：在PD模式下读取物理磁盘的扩展日志（如SMART扩展日志、厂商自定义日志）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - logAddress：日志地址（标识要读取的日志类型）
	//  - logPage：日志页面（日志内的具体页面）
	//  - data：数据缓冲区指针（存储读取到的日志数据）
	//  - dataSize：数据缓冲区大小（字节数）
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL ReadLogExtPd(INT physicalDriveId, BYTE target, BYTE logAddress, BYTE logPage, PBYTE data, DWORD dataSize);
	// ******************************************************************************************
	// NVMe磁盘专用函数（针对不同厂商NVMe设备的兼容处理）
	// ******************************************************************************************
	// 函数：AddDiskNVMe
	// 功能：将NVMe磁盘信息添加到磁盘列表（专门处理NVMe设备的信息初始化）
	// 参数：
	//  - PhysicalDriveId：物理磁盘ID
	//  - ScsiPort：SCSI端口号（NVMe设备映射的端口）
	//  - scsiTargetId：SCSI目标ID
	//  - scsiBus：SCSI总线号
	//  - target：目标设备标识
	//  - commandType：命令类型（NVMe专属命令类型）
	//  - identify：设备识别数据指针（NVMe设备的识别信息）
	//  - diskSize：磁盘容量指针（可选，默认NULL，存储读取到的磁盘容量）
	//  - pnpDeviceId：PnP设备ID（可选，默认空）
	// 返回值：BOOL类型，TRUE=添加成功，FALSE=失败
	// ******************************************************************************************
	BOOL AddDiskNVMe(INT PhysicalDriveId, INT ScsiPort, INT scsiTargetId, INT scsiBus, BYTE target, COMMAND_TYPE commandType, IDENTIFY_DEVICE* identify, DWORD* diskSize = NULL, CString pnpDeviceId = _T(""));
	// 函数：DoIdentifyDeviceNVMeJMicron
	// 功能：读取JMicron芯片NVMe设备的识别信息（针对该芯片的NVMe设备兼容处理）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - scsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - identify：设备识别数据指针
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL DoIdentifyDeviceNVMeJMicron(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	// 函数：GetSmartAttributeNVMeJMicron（续）
	// 说明：针对JMicron芯片的NVMe设备，通过其专属协议或寄存器读取SMART数据，
	//       解决通用NVMe读取逻辑在该芯片上的兼容性问题（如数据偏移、属性标识差异）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - scsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - asi：SMART信息指针
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL GetSmartAttributeNVMeJMicron(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	// 函数：DoIdentifyDeviceNVMeASMedia
	// 功能：读取ASMedia（祥硕）芯片NVMe设备的识别信息（ASMedia芯片NVMe设备的识别数据格式特殊，需专属解析）
	// 参数：
	//  - physicalDriveId：物理磁盘ID（系统级磁盘标识）
	//  - scsiPort：SCSI端口号（NVMe设备映射到的SCSI端口）
	//  - scsiTargetId：SCSI目标ID（总线上的设备编号）
	//  - identify：设备识别数据指针（存储读取到的型号、序列号等信息）
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败（如端口无效、设备无响应）
	// ******************************************************************************************
	BOOL DoIdentifyDeviceNVMeASMedia(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeNVMeASMedia(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	BOOL DoIdentifyDeviceNVMeRealtek(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);
	// 函数：GetSmartAttributeNVMeRealtek
	// 功能：读取Realtek芯片NVMe设备的SMART属性数据（针对Realtek芯片SMART数据的解析逻辑优化）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - scsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - asi：SMART信息指针
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL GetSmartAttributeNVMeRealtek(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	BOOL DoIdentifyDeviceNVMeSamsung(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeNVMeSamsung(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL GetSmartAttributeNVMeSamsung951(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	// ******************************************************************************************
	// SCSI路径与Intel NVMe设备专用函数（解决Intel芯片组/NVMe设备的兼容性问题）
	// ******************************************************************************************
	// 函数：GetScsiPath
	// 功能：根据设备路径字符串获取SCSI路径（将系统设备路径转换为SCSI总线/端口/目标ID的标准化格式）
	// 参数：
	//  - Path：系统设备路径（如"\\.\PHYSICALDRIVE0"或注册表中的设备路径）
	// 返回值：CString类型，标准化的SCSI路径（如"SCSI\Bus0\Port1\Target0"）
	// ******************************************************************************************
	CString GetScsiPath(const TCHAR* Path);

	// 函数：DoIdentifyDeviceNVMeIntel
	// 功能：读取Intel NVMe设备的识别信息（适配Intel NVMe控制器的识别协议）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - scsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - data：设备识别数据指针
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL DoIdentifyDeviceNVMeIntel(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);
	BOOL GetSmartAttributeNVMeIntel(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	// 函数：GetScsiAddress
	// 功能：根据设备路径获取SCSI地址信息（解析路径中的SCSI端口、路径ID、目标ID、LUN）
	// 参数：
	//  - Path：系统设备路径（如"\\.\Scsi0\Disk0"）
	//  - PortNumber：输出参数，SCSI端口号
	//  - PathId：输出参数，SCSI路径ID
	//  - TargetId：输出参数，SCSI目标ID
	//  - Lun：输出参数，逻辑单元号（LUN）
	// 返回值：BOOL类型，TRUE=解析成功，FALSE=路径无效或解析失败
	// ******************************************************************************************
	BOOL GetScsiAddress(const TCHAR* Path, BYTE* PortNumber, BYTE* PathId, BYTE* TargetId, BYTE* Lun);
	// 函数：DoIdentifyDeviceNVMeIntelRst
	// 功能：读取Intel RST（快速存储技术）管控下的NVMe设备识别信息（穿透RST驱动的专属读取逻辑）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - scsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - data：设备识别数据指针
	//  - diskSize：输出参数，磁盘容量（单位：MB，可选，默认NULL）
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败（RST驱动不兼容或设备无响应）
	// 说明：Intel RST会屏蔽常规NVMe读取接口，需通过RST专属协议穿透读取
	// ******************************************************************************************
	BOOL DoIdentifyDeviceNVMeIntelRst(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data, DWORD* diskSize);
	BOOL GetSmartAttributeNVMeIntelRst(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	// 函数：DoIdentifyDeviceNVMeStorageQuery
	// 功能：通过Windows Storage Query接口读取NVMe设备识别信息（Win10及以上系统的通用NVMe接口）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - scsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - data：设备识别数据指针
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=系统版本不支持或接口调用失败
	// 说明：依赖Windows Storage API，兼容性优于底层IO控制，适合新系统
	// ******************************************************************************************
	BOOL DoIdentifyDeviceNVMeStorageQuery(INT physicalDriveId, INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* data);
	BOOL GetSmartAttributeNVMeStorageQuery(INT physicalDriveId, INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);

	// ******************************************************************************************
	// SCSI设备专用函数（针对SCSI/SAS协议磁盘的信息读取）
	// ******************************************************************************************
	// 函数：DoIdentifyDeviceScsi
	// 功能：读取SCSI设备的识别信息（通过SCSI INQUIRY命令获取设备型号、厂商等信息）
	// 参数：
	//  - scsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - identify：设备识别数据指针（将SCSI INQUIRY结果转换为统一的IDENTIFY_DEVICE格式）
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL DoIdentifyDeviceScsi(INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeScsi(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdScsi(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	// 函数：ControlSmartStatusScsi
	// 功能：控制SCSI设备的SMART功能状态（启用/禁用SMART，通过SCSI SMART ENABLE/DISABLE命令）
	// 参数：
	//  - scsiPort：SCSI端口号
	//  - scsiTargetId：SCSI目标ID
	//  - command：控制命令（如启用=0x01，禁用=0x00）
	// 返回值：BOOL类型，TRUE=控制成功，FALSE=失败
	// ******************************************************************************************
	BOOL ControlSmartStatusScsi(INT scsiPort, INT scsiTargetId, BYTE command);
	// 函数：SendAtaCommandScsi
   // 功能：通过SCSI协议向设备发送ATA命令（SCSI-ATA Translation，SAT协议，适配SATA磁盘模拟为SCSI设备的场景）
   // 参数：
   //  - scsiPort：SCSI端口号
   //  - scsiTargetId：SCSI目标ID
   //  - main：ATA主命令码
   //  - sub：ATA子命令码
   //  - param：命令参数
   // 返回值：BOOL类型，TRUE=命令发送成功，FALSE=失败
   // ******************************************************************************************
	BOOL SendAtaCommandScsi(INT scsiPort, INT scsiTargetId, BYTE main, BYTE sub, BYTE param);

	// ******************************************************************************************
	// SAT（SCSI-ATA Translation）协议专用函数（适配SATA磁盘通过SAT协议映射为SCSI设备的场景）
	// ******************************************************************************************
	// 函数：DoIdentifyDeviceSat
	// 功能：通过SAT协议读取SATA设备的识别信息（将ATA IDENTIFY命令封装为SCSI命令发送）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - identify：设备识别数据指针
	//  - commandType：命令类型（指定SAT协议版本或命令封装方式）
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL DoIdentifyDeviceSat(INT physicalDriveId, BYTE target, IDENTIFY_DEVICE* identify, COMMAND_TYPE commandType);
	// 函数：GetSmartAttributeSat
	// 功能：通过SAT协议读取SATA设备的SMART属性数据（封装ATA SMART命令为SCSI命令）
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - asi：SMART信息指针
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL GetSmartAttributeSat(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi);
	// 函数：GetSmartThresholdSat
	// 功能：通过SAT协议读取SATA设备的SMART阈值数据
	// 参数：
	//  - physicalDriveId：物理磁盘ID
	//  - target：目标设备标识
	//  - asi：SMART信息指针
	// 返回值：BOOL类型，TRUE=读取成功，FALSE=失败
	// ******************************************************************************************
	BOOL GetSmartThresholdSat(INT physicalDriveId, BYTE target, ATA_SMART_INFO* asi);
	BOOL ControlSmartStatusSat(INT physicalDriveId, BYTE target, BYTE command, COMMAND_TYPE commandType);
	BOOL SendAtaCommandSat(INT physicalDriveId, BYTE target, BYTE main, BYTE sub, BYTE param, COMMAND_TYPE commandType);
	BOOL ReadLogExtSat(INT physicalDriveId, BYTE target, BYTE logAddress, BYTE logPage, PBYTE data, DWORD dataSize, COMMAND_TYPE type);

	// ******************************************************************************************
	// Silicon Image（晶像）控制器专用函数（针对该品牌控制器下磁盘的兼容性读取逻辑）
	// 说明：Silicon Image控制器（如SiI 3114/3124等）对ATA/SATA磁盘的命令转发存在特殊处理，
	//       需通过专属接口读取设备识别信息与SMART数据，避免通用逻辑的兼容性问题
	// ******************************************************************************************

	// 函数：DoIdentifyDeviceSi
	// 功能：通过Silicon Image控制器读取磁盘的设备识别信息（获取型号、序列号、容量等基础参数）
	// 参数：
	//  - physicalDriveId：物理磁盘ID（系统分配的磁盘唯一标识，用于定位具体磁盘）
	//  - scsiPort：SCSI端口号（磁盘连接的Silicon Image控制器端口，区分同一控制器下的不同设备）
	//  - scsiBus：SCSI总线号（多总线系统中，标识磁盘所在的SCSI总线，确保精准定位）
	//  - siliconImageId：Silicon Image控制器ID（区分不同的Silicon Image控制器，如多控制器环境）
	//  - identify：设备识别数据指针（指向IDENTIFY_DEVICE联合体，存储读取到的识别信息）
	// 返回值：BOOL类型，TRUE=读取成功（识别信息已写入identify），FALSE=失败（如控制器无响应、参数无效）
	// 关键逻辑：通过Silicon Image控制器的私有IO控制接口，发送ATA IDENTIFY DEVICE命令，
	//           并按该控制器的协议格式解析返回数据，转换为统一的IDENTIFY_DEVICE结构
	// ******************************************************************************************
	BOOL DoIdentifyDeviceSi(INT physicalDriveId, INT scsiPort, INT scsiBus, DWORD siliconImageId, IDENTIFY_DEVICE* identify);
	// 函数：GetSmartAttributeSi
	// 功能：通过Silicon Image控制器读取磁盘的SMART属性数据（获取健康状态、温度、重映射扇区等关键属性）
	// 参数：
	//  - physicalDriveId：物理磁盘ID（定位目标磁盘）
	//  - asi：SMART信息指针（指向ATA_SMART_INFO结构体，存储读取到的SMART属性数组、有效性标识等）
	// 返回值：BOOL类型，TRUE=读取成功（SMART属性已写入asi->Attribute数组），FALSE=失败（如SMART不支持、命令超时）
	// 关键逻辑：
	//  1. 向Silicon Image控制器发送ATA SMART READ DATA命令，获取512字节原始SMART数据；
	//  2. 按ATA协议规范解析原始数据，提取各SMART属性（如05h重映射扇区、C5h待映射扇区）；
	//  3. 填充asi结构体的属性有效性标识（如IsSmartCorrect、IsCheckSumError），标记数据可靠性。
	// ******************************************************************************************
	BOOL GetSmartAttributeSi(INT physicalDriveId, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdSi(INT physicalDriveId, ATA_SMART_INFO* asi);

	// ******************************************************************************************
	// WMI（Windows Management Instrumentation，Windows管理规范）专用函数
	// 说明：通过Windows系统内置的WMI接口读取磁盘SMART信息，无需直接操作底层IO控制，
	//       兼容性更强（尤其适用于部分屏蔽底层接口的系统/驱动），但依赖系统WMI服务正常运行
	// ******************************************************************************************

	// 函数：GetSmartAttributeWmi
	// 功能：通过WMI接口读取磁盘的SMART属性数据（获取健康状态、温度、重映射扇区等核心属性）
	// 参数：
	//  - asi：SMART信息指针（指向ATA_SMART_INFO结构体，用于存储读取到的SMART属性及有效性标识）
	// 返回值：BOOL类型，TRUE=读取成功（属性数据已写入asi->Attribute数组），FALSE=失败（如WMI服务未启动、磁盘不支持WMI SMART）
	// 关键逻辑：
	//  1. 连接WMI服务的「root\WMI」命名空间，查询目标磁盘的「MSStorageDriver_ATASmartData」类；
	//  2. 从WMI返回的「VendorSpecific」属性中提取512字节原始SMART数据（与ATA协议的SMART READ DATA结果格式一致）；
	//  3. 解析原始数据，按属性ID（如05h重映射扇区、C2h温度）分类填充到asi->Attribute数组；
	//  4. 设置asi结构体的属性有效性标识（如IsSmartCorrect、IsCheckSumError），标记数据是否可靠（如校验和是否匹配）。
	// 注意：WMI返回的SMART属性可能因系统版本/磁盘驱动差异，缺失部分厂商自定义属性
	// ******************************************************************************************
	BOOL GetSmartAttributeWmi(ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdWmi(ATA_SMART_INFO* asi);
	BOOL GetSmartInfoWmi(DWORD type, ATA_SMART_INFO* asi);

	// ******************************************************************************************
	// CSMI（Common Storage Management Interface，通用存储管理接口）专用函数
	// 说明：CSMI是SAS/SATA控制器的标准化管理接口，主要用于读取SAS磁盘及控制器下物理盘的SMART信息，
	//       需通过控制器的CSMI IO控制接口发送命令，适用于LSI、Adaptec等支持CSMI协议的存储控制器
	// ******************************************************************************************

	// 函数：AddDiskCsmi
	// 功能：通过CSMI接口枚举指定SCSI端口下的磁盘，并将磁盘信息添加到磁盘列表（vars数组）
	// 参数：
	//  - scsiPort：SCSI端口号（对应支持CSMI协议的存储控制器端口，枚举该端口下所有可管理的物理磁盘）
	// 返回值：BOOL类型，TRUE=枚举并添加磁盘成功，FALSE=失败（如控制器不支持CSMI、端口无效）
	// 关键逻辑：
	//  1. 打开指定SCSI端口的CSMI设备句柄；
	//  2. 发送CSMI命令查询该端口下的物理磁盘数量及SAS物理实体信息（如槽位、SAS地址）；
	//  3. 对每个磁盘调用DoIdentifyDeviceCsmi获取设备识别信息，再通过AddDisk函数添加到vars数组；
	//  4. 关闭CSMI设备句柄，返回整体枚举结果。
	// ******************************************************************************************
	BOOL AddDiskCsmi(INT scsiPort);
	//	BOOL GetPhyInfo(INT scsiPort, CSMI_SAS_PHY_INFO & phyInfo);
	 // 函数：CsmiIoctl
	// 功能：向CSMI设备句柄发送IO控制命令（CSMI协议的核心底层接口，为其他CSMI函数提供命令发送能力）
	// 参数：
	//  - hHandle：CSMI设备句柄（已通过CreateFile打开的有效句柄，关联到指定存储控制器）
	//  - code：IO控制命令码（CSMI协议定义的命令标识，如读取SAS物理信息、发送ATA命令）
	//  - csmiBuf：CSMI命令缓冲区指针（指向SRB_IO_CONTROL结构体，包含命令参数、数据缓冲区地址等）
	//  - csmiBufSize：CSMI命令缓冲区大小（字节数，确保缓冲区足够存储命令参数与返回数据）
	// 返回值：BOOL类型，TRUE=IO控制命令发送成功，FALSE=失败（如句柄无效、命令不支持、缓冲区不足）
	// 说明：所有其他CSMI相关函数（如DoIdentifyDeviceCsmi、GetSmartAttributeCsmi）均需调用此函数发送底层命令
	// ******************************************************************************************
	BOOL CsmiIoctl(HANDLE hHandle, UINT code, SRB_IO_CONTROL* csmiBuf, UINT csmiBufSize);
	BOOL DoIdentifyDeviceCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, ATA_SMART_INFO* asi);
	BOOL ControlSmartStatusCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, BYTE command);
	// 函数：SendAtaCommandCsmi
	// 功能：通过CSMI接口向指定磁盘发送ATA命令（支持带数据传输的ATA操作，如读取日志、设置APM/AAM）
	// 参数：
	//  - scsiPort：SCSI端口号（CSMI控制器端口）
	//  - sasPhyEntity：SAS物理实体信息指针（定位目标磁盘）
	//  - main：ATA主命令码（如0x92=读取SMART数据，0xE2=设置APM）
	//  - sub：ATA子命令码（补充主命令的细分操作，无细分操作时为0x00）
	//  - param：命令参数（根据主命令类型传递的配置值，如APM的电源管理级别）
	//  - data：数据缓冲区指针（存储命令的输入/输出数据，如读取日志时的输出缓冲区、设置参数时的输入缓冲区）
	//  - dataSize：数据缓冲区大小（字节数，确保缓冲区足够存储数据）
	// 返回值：BOOL类型，TRUE=命令发送成功（数据已正确传输），FALSE=失败（如数据长度不匹配、命令执行错误）
	// 关键逻辑：
	//  1. 打开CSMI设备句柄，构造“发送ATA命令”的CSMI命令，封装main/sub/param及data缓冲区地址；
	//  2. 调用CsmiIoctl发送命令，根据命令类型（读/写）完成数据传输；
	//  3. 关闭句柄，返回命令执行结果（成功则data缓冲区已更新为目标数据）。
	// ******************************************************************************************
	BOOL SendAtaCommandCsmi(INT scsiPort, PCSMI_SAS_PHY_ENTITY sasPhyEntity, BYTE main, BYTE sub, BYTE param, PBYTE data, DWORD dataSize);

	// ******************************************************************************************
	// MegaRAID 专用函数（针对 LSI/Avago/Broadcom MegaRAID 阵列卡的磁盘管理逻辑）
	// 说明：MegaRAID 阵列卡会将物理磁盘（PD）封装为阵列卡可管理的设备，需通过 MegaRAID 专属协议（基于 MFI 命令集）
	//       穿透阵列卡驱动，读取物理磁盘的识别信息与 SMART 数据，适用于 RAID 环境下的物理盘健康监测
	// ******************************************************************************************

	// 函数：AddDiskMegaRAID
	// 功能：枚举指定 SCSI 端口（对应 MegaRAID 阵列卡端口）下的物理磁盘，并将磁盘信息添加到磁盘列表（vars 数组）
	// 参数：
	//  - scsiPort：SCSI 端口号（关联到 MegaRAID 阵列卡的物理端口，枚举该端口下所有可访问的物理磁盘）
	// 返回值：BOOL 类型，TRUE=枚举并添加磁盘成功，FALSE=失败（如阵列卡不支持、端口无效、无物理磁盘）
	// 关键逻辑：
	//  1. 打开 MegaRAID 阵列卡的设备句柄；
	//  2. 发送 MFI 命令（如 MFI_CMD_DCMD + MFI_DCMD_PD_GET_LIST）查询该端口下的物理磁盘列表；
	//  3. 对每个物理磁盘，调用 DoIdentifyDeviceMegaRAID 获取设备识别信息，再通过 AddDisk 函数添加到 vars 数组；
	//  4. 关闭设备句柄，返回整体枚举结果。
	// ******************************************************************************************
	BOOL AddDiskMegaRAID(INT scsiPort);
	BOOL DoIdentifyDeviceMegaRAID(INT scsiPort, INT scsiTargetId, IDENTIFY_DEVICE* identify);
	BOOL GetSmartAttributeMegaRAID(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL GetSmartThresholdMegaRAID(INT scsiPort, INT scsiTargetId, ATA_SMART_INFO* asi);
	BOOL ControlSmartStatusMegaRAID(INT scsiPort, INT scsiTargetId, BYTE command);
	BOOL SendAtaCommandMegaRAID(INT scsiPort, INT scsiTargetId, BYTE main, BYTE sub, BYTE param);
	BOOL SendDCommandMegaRAID(HANDLE hHandle, ULONG opcode, void* buf, size_t bufsize, BYTE* mbox, size_t mboxlen);
	BOOL SendPassThroughCommandMegaRAID(INT scsiPort, INT scsiTargetId, void* buf, size_t bufsize, const UCHAR Cdb[], UCHAR CdbLength);

	// ******************************************************************************************
	// 磁盘参数解析与SMART数据处理函数（负责传输模式、版本信息解析及SMART数据填充）
	// ******************************************************************************************

	// 函数：GetTransferMode
	// 功能：解析磁盘的传输模式信息（当前传输模式、最大支持模式、接口类型）
	// 参数：
	//  - w63：ATA IDENTIFY数据的第63字（存储传输模式相关标识）
	//  - w76：ATA IDENTIFY数据的第76字（补充传输模式参数）
	//  - w77：ATA IDENTIFY数据的第77字（补充传输模式参数）
	//  - w88：ATA IDENTIFY数据的第88字（接口类型与传输模式扩展标识）
	//  - currentTransferMode：输出参数，当前传输模式字符串（如"UDMA Mode 6"、"SATA III 6.0 Gb/s"）
	//  - maxTransferMode：输出参数，最大支持传输模式字符串（如"UDMA Mode 7"、"SATA III 6.0 Gb/s"）
	//  - Interface：输出参数，接口类型字符串（如"SATA II"、"PCIe 3.0 x4"）
	//  - interfaceType：输出参数，接口类型枚举（指向INTERFACE_TYPE变量，用于程序内逻辑判断）
	// 返回值：DWORD类型，传输模式解析状态码（0=解析成功，非0=解析失败或参数无效）
	// 关键逻辑：
	//  1. 根据w63/w76/w77判断UDMA/PIO传输模式的当前值与最大值；
	//  2. 通过w88识别接口类型（SATA/PCIe等）及高速传输协议（如SATA 6Gb/s）；
	//  3. 将解析结果转换为易读字符串（如currentTransferMode），并同步更新interfaceType枚举值。
	// ******************************************************************************************
	DWORD GetTransferMode(WORD w63, WORD w76, WORD w77, WORD w88, CString& currentTransferMode, CString& maxTransferMode, CString& Interface, INTERFACE_TYPE* interfaceType);
	// 函数：GetTransferModePCIe
	// 功能：解析PCIe接口磁盘的传输模式信息（针对NVMe/PCIe SSD的传输速度）
	// 参数：
	//  - current：输出参数，当前PCIe传输模式字符串（如"PCIe 3.0 x2"）
	//  - max：输出参数，最大支持PCIe传输模式字符串（如"PCIe 4.0 x4"）
	//  - slotspeed：PCIe插槽速度信息（包含链路宽度、版本等，如SlotMaxCurrSpeed结构体的Width/Gen字段）
	// 返回值：VOID类型，无返回值
	// 关键逻辑：根据slotspeed的链路世代（Gen）与宽度（Width），组合为标准传输模式字符串（如"PCIe 4.0 x4"）。
	// ******************************************************************************************
	VOID GetTransferModePCIe(CString& current, CString& max, SlotMaxCurrSpeed slotspeed);
	DWORD GetTimeUnitType(CString model, CString firmware, DWORD major, DWORD transferMode);
	// 函数：GetAtaMajorVersion
	// 功能：解析ATA磁盘的主版本号（如ATA-7、ATA-8）
	// 参数：
	//  - w80：ATA IDENTIFY数据的第80字（存储ATA版本标识位）
	//  - majorVersion：输出参数，主版本号字符串（如"ATA-8"、"ATA-8-ACS"）
	// 返回值：DWORD类型，版本解析状态码（0=解析成功，非0=解析失败）
	// 关键逻辑：根据w80的二进制位组合，匹配ATA标准定义的版本对照表（如bit12置位表示ATA-8），生成版本字符串。
	// ******************************************************************************************
	DWORD GetAtaMajorVersion(WORD w80, CString& majorVersion);
	VOID  GetAtaMinorVersion(WORD w81, CString& minor);

	//	DWORD GetMaxtorPowerOnHours(DWORD currentValue, DWORD rawValue);
	// 函数：FillSmartData
	// 功能：填充SMART属性数据（将原始SMART数据解析为结构化的SMART_ATTRIBUTE数组）
	// 参数：
	//  - asi：SMART信息指针（指向ATA_SMART_INFO结构体，其SmartReadData存储原始SMART数据，Attribute数组用于存储解析结果）
	// 返回值：BOOL类型，TRUE=填充成功（Attribute数组已更新），FALSE=填充失败（如原始数据无效、校验和错误）
	// 关键逻辑：
	//  1. 校验SmartReadData的校验和（确保原始数据未损坏）；
	//  2. 按ATA协议解析原始数据的属性区（每个属性占12字节，包含ID、当前值、阈值、原始值等）；
	//  3. 将解析后的属性依次存入asi->Attribute数组，更新asi->AttributeCount（属性总数）；
	//  4. 设置asi->IsSmartCorrect标识（标记属性数据是否可靠）。
	// ******************************************************************************************
	BOOL FillSmartData(ATA_SMART_INFO* asi);
	BOOL FillSmartThreshold(ATA_SMART_INFO* asi);

	// ******************************************************************************************
	// SSD识别函数（通过磁盘型号、厂商、SMART属性等特征，判断是否为SSD及具体SSD类型）
	// 说明：不同厂商SSD的硬件特征与SMART属性定义差异较大，需针对性识别以确保后续寿命计算、健康判断准确。
	// ******************************************************************************************

	// 函数：CheckSsdSupport
	// 功能：综合判断磁盘是否为SSD（调用各类厂商专属SSD识别函数，更新asi的IsSsd标识）
	// 参数：
	//  - asi：引用类型的SMART信息（ATA_SMART_INFO，最终IsSsd标识会更新为判断结果）
	// 返回值：VOID类型，无返回值
	// 关键逻辑：
	//  1. 依次调用IsSsdOld、IsSsdIntel、IsSsdSamsung等厂商专属识别函数；
	//  2. 若任一识别函数返回TRUE（匹配对应厂商SSD特征），则设置asi->IsSsd=TRUE；
	//  3. 若所有厂商函数均返回FALSE，调用IsSsdGeneral（通用SSD识别逻辑）最终判断。
	// ******************************************************************************************
	VOID CheckSsdSupport(ATA_SMART_INFO& asi);
	// 函数：IsSsdOld
	// 功能：识别早期老旧SSD（如早期Mtron、Indilinx主控SSD，无统一厂商标识，需通过型号特征判断）
	// 参数：
	//  - asi：引用类型的SMART信息（通过Model、FirmwareRev等字段匹配早期SSD特征）
	// 返回值：BOOL类型，TRUE=为早期SSD，FALSE=非早期SSD
	// ******************************************************************************************
	BOOL IsSsdOld(ATA_SMART_INFO& asi);
	// 函数：IsSsdMtron
	// 功能：识别Mtron品牌SSD（通过型号前缀、厂商标识匹配，如"MTRON"开头的型号）
	// 参数：- asi：引用类型的SMART信息（依赖Model字段判断）
	// 返回值：BOOL类型，TRUE=为Mtron SSD，FALSE=非Mtron SSD
	// ******************************************************************************************
	BOOL IsSsdMtron(ATA_SMART_INFO& asi);
	// 函数：IsSsdIndlinx
	// 功能：识别Indilinx主控SSD（早期常见主控，通过型号中的"Indilinx"标识或SMART属性特征判断）
	// 参数：- asi：引用类型的SMART信息（Model或SmartAttribute字段）
	// 返回值：BOOL类型，TRUE=为Indilinx主控SSD，FALSE=非该类型
	// ******************************************************************************************
	BOOL IsSsdIndlinx(ATA_SMART_INFO& asi);
	BOOL IsSsdJMicron60x(ATA_SMART_INFO& asi);
	BOOL IsSsdJMicron61x(ATA_SMART_INFO& asi);
	BOOL IsSsdJMicron66x(ATA_SMART_INFO& asi);
	// 函数：IsSsdIntel/IsSsdIntelDc
	// 功能：识别Intel消费级SSD/Intel数据中心级（DC）SSD（通过型号前缀如"INTEL SSDSC"、"INTEL SSDP"判断）
	// 参数：- asi：引用类型的SMART信息（Model字段是核心判断依据）
	// 返回值：BOOL类型，TRUE=为对应类型Intel SSD，FALSE=非该类型
	// ******************************************************************************************
	BOOL IsSsdIntel(ATA_SMART_INFO& asi);
	BOOL IsSsdIntelDc(ATA_SMART_INFO& asi);
	BOOL IsSsdSamsung(ATA_SMART_INFO& asi);
	BOOL IsSsdSandForce(ATA_SMART_INFO& asi);
	BOOL IsSsdMicronMU02(ATA_SMART_INFO& asi);
	BOOL IsSsdMicron(ATA_SMART_INFO& asi);
	BOOL IsSsdOcz(ATA_SMART_INFO& asi);
	BOOL IsSsdSsstc(ATA_SMART_INFO& asi);
	BOOL IsSsdPlextor(ATA_SMART_INFO& asi);
	BOOL IsSsdSanDisk(ATA_SMART_INFO& asi);
	BOOL IsSsdKingston(ATA_SMART_INFO& asi);
	BOOL IsSsdOczVector(ATA_SMART_INFO& asi);
	BOOL IsSsdToshiba(ATA_SMART_INFO& asi);
	BOOL IsSsdRealtek(ATA_SMART_INFO& asi);
	BOOL IsSsdSKhynix(ATA_SMART_INFO& asi);
	BOOL IsSsdKioxia(ATA_SMART_INFO& asi);
	BOOL IsSsdApacer(ATA_SMART_INFO& asi);
	BOOL IsSsdSiliconMotion(ATA_SMART_INFO& asi);
	BOOL IsSsdPhison(ATA_SMART_INFO& asi);
	BOOL IsSsdSeagate(ATA_SMART_INFO& asi);
	BOOL IsSsdMarvell(ATA_SMART_INFO& asi);
	BOOL IsSsdMaxiotek(ATA_SMART_INFO& asi);
	BOOL IsSsdYmtc(ATA_SMART_INFO& asi);
	BOOL IsSsdGeneral(ATA_SMART_INFO& asi);

	//	INT CheckPlextorNandWritesUnit(ATA_SMART_INFO &asi);
	 
	// 函数：Compare（静态函数）
	// 功能：提供排序比较逻辑（通常用于磁盘列表、SMART属性数组的排序，如按磁盘ID、属性ID升序/降序排列）
	// 参数：
	//  - p1：指向第一个待比较元素的指针（需根据排序目标强转为对应类型，如ATA_SMART_INFO*、DWORD*）
	//  - p2：指向第二个待比较元素的指针（与p1类型一致）
	// 返回值：int类型，比较结果（-1=p1小于p2，0=p1等于p2，1=p1大于p2）
	// 说明：
	//  1. 静态函数意味着该函数属于当前类/模块，不依赖实例即可调用；
	//  2. 具体比较逻辑需结合实际排序场景，例如：
	//     - 若排序对象为磁盘ID（DWORD），则直接比较*(DWORD*)p1与*(DWORD*)p2；
	//     - 若排序对象为SMART属性（SMART_ATTRIBUTE），则比较属性ID（p1->AttributeId与p2->AttributeId）。
	//  3. 通常配合标准库排序函数（如qsort）使用，作为自定义比较器。
	// ******************************************************************************************
	static int Compare(const void* p1, const void* p2);

	// 函数：GetModelSerial
	// 功能：处理磁盘型号（Model）与序列号（SerialNumber），返回格式化后的组合字符串（或修正后的独立字符串）
	// 参数：
	//  - model：输入输出参数，磁盘型号字符串（可能包含空格、乱码，函数会进行清洗，如去除首尾空格、过滤无效ASCII）
	//  - serialNumber：输入输出参数，磁盘序列号字符串（同样进行清洗，确保格式规范）
	// 返回值：CString类型，格式化后的组合结果（如“Model: ST1000DM010 | Serial: Z9A2XXXX”，具体格式需看实现逻辑）
	// 关键逻辑：
	//  1. 清洗型号与序列号：调用CheckAsciiStringError检查无效字符，调用ChangeByteOrder修正字节序（部分磁盘存储为小端）；
	//  2. 去除冗余字符：如型号中的“ ”（连续空格）、“_”（下划线），序列号中的特殊符号；
	//  3. 组合字符串：按预设格式拼接型号与序列号，便于日志输出或UI显示。
	// ******************************************************************************************
	CString GetModelSerial(CString& model, CString& serialNumber);

	// （注释）预留/未启用函数：通过GPL（可能为厂商自定义协议或通用寿命计算规则）获取磁盘寿命
	//	BOOL GetLifeByGpl(ATA_SMART_INFO& asi);
};
