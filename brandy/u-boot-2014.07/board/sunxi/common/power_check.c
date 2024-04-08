#include <common.h>
//#include <asm/arch/drv_display.h>
#include <sys_config.h>
#include <asm/arch/timer.h>
#include <power/sunxi/pmu.h>
#include <power/sunxi/power.h>
#include <sunxi_board.h>

DECLARE_GLOBAL_DATA_PTR;

typedef enum __BOOT_POWER_STATE
{
	STATE_SHUTDOWN_DIRECTLY = 0,
	STATE_SHUTDOWN_CHARGE,
	STATE_ANDROID_CHARGE,
	STATE_NORMAL_BOOT
}BOOT_POWER_STATE_E;


int __sunxi_bmp_display(char* name)
{
	return 0;
}


int sunxi_bmp_display(char * name)
	__attribute__((weak, alias("__sunxi_bmp_display")));



static void UpdateChargeVariable(void)
{
	int ChargeMode = 0;
	if(0 == script_parser_fetch("charging_type", "charging_type", &ChargeMode, 1))
	{
		gd->chargemode = 1;
	}
}


static void EnterShutDownMode(void)
{
	printf("battery ratio is low without  dc or ac, should be ShowDown\n");
	sunxi_bmp_display("bat\\low_pwr.bmp");
	__msdelay(3000);
	sunxi_board_shutdown();
	for(;;);
}

static void EnterShutDownWithChargeMode(void)
{
	printf("battery low power and vol with dc or ac, should charge longer\n");
	sunxi_bmp_display("bat\\bempty.bmp");
	__msdelay(3000);
	sunxi_board_shutdown();
	for(;;);
}

static void EnterAndroidChargeMode(void)
{
	printf("sunxi_bmp_charger_display\n");
	sunxi_bmp_display("bat\\battery_charge.bmp");
	UpdateChargeVariable();
}

static void EnterNormalBootMode(void)
{
	printf("sunxi_bmp_logo_display\n");
	sunxi_bmp_display("bootlogo.bmp");
}



int ProbePreSystemMode(void)
{
    int  PreSysMode = 0;

    PreSysMode = axp_probe_pre_sys_mode();
    if(PreSysMode == PMU_PRE_SYS_MODE )
    {
        printf("pre sys mode\n");
        return PMU_PRE_SYS_MODE;
    }
    else if(PreSysMode == PMU_PRE_BOOT_MODE)
    {
        printf("pre boot mode\n");
        return PMU_PRE_BOOT_MODE;
    }
    else if(PreSysMode == PMU_PRE_FASTBOOT_MODE)
    {
        printf("pre fastboot mode\n");
        return PMU_PRE_FASTBOOT_MODE;
    }

    return 0;
}

static int ProbeStartupCause(void)
{
	uint PowerOnCause = 0;
	PowerOnCause = axp_probe_startup_cause();
	if(PowerOnCause == AXP_POWER_ON_BY_POWER_KEY)
	{
		printf("key trigger\n");
	}
	else if(PowerOnCause == AXP_POWER_ON_BY_POWER_TRIGGER)
	{
		printf("power trigger\n");
	}

	return PowerOnCause;
}


//check battery and voltage
static int GetBatteryRatio( void)
{
	int  Ratio ;
	Ratio = axp_probe_rest_battery_capacity();
	if(Ratio < 1)
	{
		axp_set_coulombmeter_onoff(0);
		axp_set_coulombmeter_onoff(1);
		
	}
	return Ratio;
}

/*
*function : GetStateOnLowBatteryRatio
*@PowerBus           :   0: power  not exist   other:  power  exist
*@LowVoltageFlag :   0:high voltage  1: low voltage
*@PowerOnCause  :   Power is inserted OR   power_key is pressed
*note:  Decide which state should enter when battery ratio is low
*/
static BOOT_POWER_STATE_E GetStateOnLowBatteryRatio(int PowerBus,int LowVoltageFlag,int PowerOnCause)
{
	BOOT_POWER_STATE_E BootPowerState;
	int PowerStart = 0;

	//power_start
	//0:  not allow boot by insert dcin: press power key in long time & pre state is system state(Battery ratio shouled enough)
	//1: allow boot directly  by insert dcin:( Battery ratio shouled enough)
	//2: not allow boot by insert dcin:press power key in long time & pre state is system state(do not check battery ratio)
	//3: allow boot directly  by insert dcin:( do not check battery ratio)
	//
	script_parser_fetch(PMU_SCRIPT_NAME, "power_start", &PowerStart, 1);

	do {
		//power  not exist,shutdown directly
		if(PowerBus == 0)
		{
			BootPowerState = STATE_SHUTDOWN_DIRECTLY;
			break;
		}

		//----------------power  exist: dcin or vbus------------------
		//user config is 3, allow boot directly  by insert dcin
		if(PowerStart == 3)
		{
			BootPowerState = STATE_NORMAL_BOOT;
			break;
		}

		//low voltage
		if(LowVoltageFlag)
		{
			BootPowerState = STATE_SHUTDOWN_CHARGE;
		}
		//high voltage
		else
		{
			BootPowerState = (PowerOnCause == AXP_POWER_ON_BY_POWER_TRIGGER) ? \
				STATE_ANDROID_CHARGE:STATE_SHUTDOWN_CHARGE;
		}
	}while(0);
	return BootPowerState;
}

static BOOT_POWER_STATE_E GetStateOnHighBatteryRatio(int PowerBus,int LowVoltageFlag,int PowerOnCause)
{
	BOOT_POWER_STATE_E BootPowerState;
	//battery ratio enougth
	//note : 
	//1) enter android charge :  power is  inserted 
	//2) enter normal  boot     :  power key  is pressed  
	BootPowerState = (PowerOnCause==AXP_POWER_ON_BY_POWER_TRIGGER) ? \
	                 STATE_ANDROID_CHARGE : STATE_NORMAL_BOOT;
	return BootPowerState;
}



//function : PowerCheck
//para: null
//note:  Decide whether to boot
int PowerCheck(void)
{
	uint BatExist = 0;
	uint PowerBus=0;
	uint BatVol=0;
	uint BatRatio=0;
	int SafeVol =0;
	int PowerOnCause = 0;
	int Ret = 0;
	int LowVoltageFlag = 0;
	int LowBatRatioFlag =0;
	BOOT_POWER_STATE_E BootPowerState;

	//clear  power key 
	axp_probe_key();

	//check battery
	BatExist = axp_probe_battery_exist();
	if(BatExist <= 0)
	{
		printf("no battery exist\n");
		EnterNormalBootMode();
		return 0;
	}

	//check power bus
	PowerBus = axp_probe_power_source();
	printf("PowerBus = %x(0: not exist 1:vBus 2:acBus 3:vBus&acBus)\n", PowerBus);

	//check battery ratio
	BatRatio = GetBatteryRatio();
	BatVol = axp_probe_battery_vol();
	printf("Battery Voltage=%d, Ratio=%d\n", BatVol, BatRatio);

	Ret = script_parser_fetch(PMU_SCRIPT_NAME, "pmu_safe_vol", &SafeVol, 1);
	if((Ret) || (SafeVol < 3000))
	{
		SafeVol = 3500;
	}

	LowBatRatioFlag =  (BatRatio<1) ? 1:0;
	LowVoltageFlag  =  (BatVol<SafeVol) ? 1:0;
	PowerOnCause = ProbeStartupCause();


	if(LowBatRatioFlag)
	{
		//low battery ratio
		BootPowerState = GetStateOnLowBatteryRatio(PowerBus,LowVoltageFlag,PowerOnCause);
	}
	else
	{
		//high battery ratio
		BootPowerState = GetStateOnHighBatteryRatio(PowerBus,LowVoltageFlag,PowerOnCause);
	}

	switch(BootPowerState)
	{
		case STATE_SHUTDOWN_DIRECTLY:
			EnterShutDownMode();
			break;
		case STATE_SHUTDOWN_CHARGE:
			EnterShutDownWithChargeMode();
			break;
		case STATE_ANDROID_CHARGE:
			EnterAndroidChargeMode();
			break;
		case STATE_NORMAL_BOOT:
			EnterNormalBootMode();
			break;
		default:
			printf("%s: error boot mode\n",__func__);
			break;
	}
	return 0;

}