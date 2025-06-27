#include <stdint.h>


struct initialization_data_t
{
	wchar_t anti_cheat_driver_name[64];
	uint64_t* offsets;
	uint64_t number_of_process_names;
	char** process_names;
};

//enum class kernel_offset_position_t
//{
//	DxgkEscape,
//	ExGetPoolTagInfo,
//	ExGetBigPoolInfo,
//	KeInsertQueueApc,
//	NtDeviceIoControlFile,
//	NtQueryVolumeInformationFile,
//	NtQuerySystemInformation,
//	NtUserFindWindowEx,
//	NtUserQueryWindow,
//	NtUserBuildHwndList,
//	NtUserGetForegroundWindow,
//	WmipRawSMBiosTableHandler,
//	IopLoadDriverImage,
//	PnpCallDriverEntry,
//	NtUserWindowFromPoint,
//	NtCallEnclave,
//	RtlWalkFrameChain,
//	max_element
//}; //old!

enum class kernel_offset_position_t
{
    DxgkEscape,
    ExGetPoolTagInfo,
    ExGetBigPoolInfo,
    KeInsertQueueApc,
    NtDeviceIoControlFile,
    NtQueryVolumeInformationFile,
    NtQuerySystemInformation,
    NtUserFindWindowEx,
    NtUserQueryWindow,
    NtUserBuildHwndList,
    NtUserGetForegroundWindow,
    WmipRawSMBiosTableHandler,
    IopLoadDriverImage,
    PnpCallDriverEntry,
    NtUserWindowFromPoint,
    NtCallEnclave,
    RtlWalkFrameChain,
    NtTerminateProcess,
    ExpGetProcessInformation,
    NtUserSetFocus,
    max_element
};