/*
 * (C) Copyright 2007-2013
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Jerry Wang <wangflord@allwinnertech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <malloc.h>
#include <sys_config.h>
#include <sunxi_display2.h>


DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SUNXI_DISPLAY

static __u32 screen_id = 0;
static __u32 disp_para = 0;
extern __s32 disp_delay_ms(__u32 ms);
extern long disp_ioctl(void *hd, unsigned int cmd, void *arg);

int board_display_layer_request(void)
{
	gd->layer_hd = 0;
	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_layer_release(void)
{
	return 0;

}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_wait_lcd_open(void)
{
	int ret;
	int timedly = 5000;
	int check_time = timedly/50;
	uint arg[4] = { 0 };
	uint cmd = 0;

	cmd = DISP_LCD_CHECK_OPEN_FINISH;

	do
	{
    	ret = disp_ioctl(NULL, cmd, (void*)arg);
		if(ret == 1)		//open already
		{
			break;
		}
		else if(ret == -1)  //open falied
		{
			return -1;
		}
		__msdelay(50);
		check_time --;
		if(check_time <= 0)
		{
			return -1;
		}
	}
	while(1);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_wait_lcd_close(void)
{
	int ret;
	int timedly = 5000;
	int check_time = timedly/50;
	uint arg[4] = { 0 };
	uint cmd = 0;

	cmd = DISP_LCD_CHECK_CLOSE_FINISH;

	do
	{
    	ret = disp_ioctl(NULL, cmd, (void*)arg);
		if(ret == 1)		//open already
		{
			break;
		}
		else if(ret == -1)  //open falied
		{
			return -1;
		}
		__msdelay(50);
		check_time --;
		if(check_time <= 0)
		{
			return -1;
		}
	}
	while(1);

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_set_exit_mode(int lcd_off_only)
{
	uint arg[4] = { 0 };
	uint cmd = 0;
	cmd = DISP_SET_EXIT_MODE;

	if(lcd_off_only)
	{
		arg[0] = DISP_EXIT_MODE_CLEAN_PARTLY;
		disp_ioctl(NULL, cmd, (void *)arg);
	}
	else
	{
		cmd = DISP_LCD_DISABLE;
		disp_ioctl(NULL, cmd, (void *)arg);
		board_display_wait_lcd_close();
	}

	return 0;
}
/*
*******************************************************************************
*                     board_display_layer_open
*
* Description:
*    打开图层
*
* Parameters:
*    Layer_hd    :  input. 图层句柄
*
* Return value:
*    0  :  成功
*   !0  :  失败
*
* note:
*    void
*
*******************************************************************************
*/
int board_display_layer_open(void)
{
	uint arg[4];
	disp_layer_config *config = (disp_layer_config *)gd->layer_para;

	arg[0] = screen_id;
	arg[1] = (unsigned long)config;
	arg[2] = 1;
	arg[3] = 0;

	disp_ioctl(NULL,DISP_LAYER_GET_CONFIG,(void*)arg);
	config->enable = 1;
	disp_ioctl(NULL,DISP_LAYER_SET_CONFIG,(void*)arg);

	return 0;
}


/*
*******************************************************************************
*                     board_display_layer_close
*
* Description:
*    关闭图层
*
* Parameters:
*    Layer_hd    :  input. 图层句柄
*
* Return value:
*    0  :  成功
*   !0  :  失败
*
* note:
*    void
*
*******************************************************************************
*/
int board_display_layer_close(void)
{
	uint arg[4];
	disp_layer_config *config = (disp_layer_config *)gd->layer_para;

	arg[0] = screen_id;
	arg[1] = (unsigned long)config;
	arg[2] = 1;
	arg[3] = 0;

	disp_ioctl(NULL,DISP_LAYER_GET_CONFIG,(void*)arg);
	config->enable = 0;
	disp_ioctl(NULL,DISP_LAYER_SET_CONFIG,(void*)arg);



    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_layer_para_set(void)
{
	uint arg[4];

	arg[0] = screen_id;
	arg[1] = gd->layer_para;
	arg[2] = 1;
	arg[3] = 0;
	disp_ioctl(NULL,DISP_LAYER_SET_CONFIG,(void*)arg);

    return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_show_until_lcd_open(int display_source)
{
	printf("%s\n", __func__);
	if(!display_source)
	{
		board_display_wait_lcd_open();
	}
	board_display_layer_para_set();
	board_display_layer_open();

	return 0;
}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_show(int display_source)
{
	board_display_layer_para_set();
	board_display_layer_open();

	return 0;

}
/*
************************************************************************************************************
*
*                                             function
*
*    name          :
*
*    parmeters     :
*
*    return        :
*
*    note          :
*
*
************************************************************************************************************
*/
int board_display_framebuffer_set(int width, int height, int bitcount, void *buffer)
{

	disp_layer_config *layer_para;
	uint screen_width, screen_height;
	uint arg[4];
	int full = 0;

	if(!gd->layer_para)
	{
		layer_para = (disp_layer_config *)malloc(sizeof(disp_layer_config));
		if(!layer_para)
		{
			tick_printf("sunxi display error: unable to malloc memory for layer\n");

			return -1;
		}
	}
	else
	{
		layer_para = (disp_layer_config *)gd->layer_para;
	}

	if(script_parser_fetch("boot_disp", "output_full", &full, 1) < 0)
	{
		tick_printf("fetch script data boot_disp.output_disp fail\n");
		full = 0;
	}

	arg[0] = screen_id;
	screen_width = disp_ioctl(NULL, DISP_GET_SCN_WIDTH, (void*)arg);
	screen_height = disp_ioctl(NULL, DISP_GET_SCN_HEIGHT, (void*)arg);
	tick_printf("screen_id =%d, screen_width =%d, screen_height =%d\n", screen_id, screen_width, screen_height);
	memset((void *)layer_para, 0, sizeof(disp_layer_config));
	layer_para->info.fb.addr[0]		= (uint)buffer;
	tick_printf("frame buffer address %x\n", (uint)buffer);
	layer_para->channel = 1;
	layer_para->layer_id = 0;
	layer_para->info.fb.format		= (bitcount == 24)? DISP_FORMAT_RGB_888:DISP_FORMAT_ARGB_8888;
	layer_para->info.fb.size[0].width	= width;
	layer_para->info.fb.size[0].height	= height;
	layer_para->info.fb.crop.x	= 0;
	layer_para->info.fb.crop.y	= 0;
	layer_para->info.fb.crop.width	= ((unsigned long long)width) << 32;
	layer_para->info.fb.crop.height	= ((unsigned long long)height) << 32;
	layer_para->info.fb.flags = DISP_BF_NORMAL;
	layer_para->info.fb.scan = DISP_SCAN_PROGRESSIVE;
	debug("bitcount = %d\n", bitcount);
	layer_para->info.mode     = LAYER_MODE_BUFFER;
	layer_para->info.alpha_mode    = 1;
	layer_para->info.alpha_value   = 0xff;
	if(full) {
		layer_para->info.screen_win.x	= 0;
		layer_para->info.screen_win.y	= 0;
		layer_para->info.screen_win.width	= screen_width;
		layer_para->info.screen_win.height	= screen_height;
	} else {
		layer_para->info.screen_win.x	= (screen_width - width) / 2;
		layer_para->info.screen_win.y	= (screen_height - height) / 2;
		layer_para->info.screen_win.width	= width;
		layer_para->info.screen_win.height	= height;
	}
	layer_para->info.b_trd_out		= 0;
	layer_para->info.out_trd_mode 	= 0;
	gd->layer_para = (uint)layer_para;

	return 0;
}

void board_display_set_alpha_mode(int mode)
{
	if(!gd->layer_para)
	{
		return;
	}

	disp_layer_config *layer_para;
	layer_para = (disp_layer_config *)gd->layer_para;
	layer_para->info.alpha_mode = mode;

}

int board_display_framebuffer_change(void *buffer)
{
	return 0;
}

int board_display_device_open(void)
{

	int  value = 1;
	int  ret = 0;
	__u32 output_type = 0;
	__u32 output_mode = 0;
	//__u32 auto_hpd = 0;
	__u32 err_count = 0;
	unsigned long arg[4] = {0};

	debug("De_OpenDevice\n");

//channel
	if(script_parser_fetch("boot_disp", "output_disp", &value, 1) < 0)
	{
		tick_printf("fetch script data boot_disp.output_disp fail\n");
		err_count ++;
		value = 0;
	}
	else
	{
		tick_printf("boot_disp.output_disp=%d\n", value);
		screen_id = value;
	}

//screen0_output_type
	if(script_parser_fetch("boot_disp", "output_type", &value, 1) < 0)
	{
		tick_printf("fetch script data boot_disp.output_type fail\n");
		err_count ++;
		value = 0;
	}
	else
	{
		tick_printf("boot_disp.output_type=%d\n", value);
	}

	if(value == 0)
	{
		output_type = DISP_OUTPUT_TYPE_NONE;
	}
	else if(value == 1)
	{
		output_type = DISP_OUTPUT_TYPE_LCD;
	}
	else if(value == 2)
	{
		output_type = DISP_OUTPUT_TYPE_TV;
	}
	else if(value == 3)
	{
		output_type = DISP_OUTPUT_TYPE_HDMI;
	}
	else if(value == 4)
	{
		output_type = DISP_OUTPUT_TYPE_VGA;
	}
	else
	{
		tick_printf("invalid screen0_output_type %d\n", value);
		return -1;
	}
	//screen0_output_mode
	if(script_parser_fetch("boot_disp", "output_mode", &value, 1) < 0)
	{
		tick_printf("fetch script data boot_disp.output_mode fail\n");
		err_count ++;
		value = 0;
	}
	else
	{
		tick_printf("boot_disp.output_mode=%d\n", value);
	}

	if(output_type == DISP_OUTPUT_TYPE_TV || output_type == DISP_OUTPUT_TYPE_HDMI)
	{
		output_mode = value;
	}
	else if(output_type == DISP_OUTPUT_TYPE_VGA)
	{
		output_mode = value;
	}

	//auto hot plug detect
	if(script_parser_fetch("boot_disp", "auto_hpd", &value, 1) < 0)
	{
		tick_printf("fetch script data boot_disp.auto_hpd fail\n");
		err_count ++;
		value = 0;
	}else
	{
		//auto_hpd = value;
		tick_printf("boot_disp.auto_hpd=%d\n", value);
	}

	if(err_count == 4)//no boot_disp config
	{
		if(script_parser_fetch("lcd0_para", "lcd_used", &value, 1) < 0)
		{
			tick_printf("fetch script data lcd0_para.lcd_used fail\n");
			value = 0;
		}else
		{
			tick_printf("lcd0_para.lcd_used=%d\n", value);
		}

		if(value == 1) //lcd available
		{
			output_type = DISP_OUTPUT_TYPE_LCD;
		}
		

	}
	else//has boot_disp config
	{

	}
	printf("disp%d device type(%d) enable\n", screen_id, output_type);

	arg[0] = screen_id;
	arg[1] = output_type;
	arg[2] = output_mode;
	disp_ioctl(NULL, DISP_DEVICE_SWITCH, (void *)arg);



	disp_para = ((output_type << 8) | (output_mode)) << (screen_id*16);
	return ret;
}

void board_display_setenv(char *data)	
{
	if (!data)
		return;

	sprintf(data, "%x", disp_para);
}

int borad_display_get_screen_width(void)
{
	unsigned long arg[4] = {0};
	arg[0] = screen_id;

	return disp_ioctl(NULL, DISP_GET_SCN_WIDTH, (void*)arg);

}

int borad_display_get_screen_height(void)
{
	unsigned long arg[4] = {0};

	arg[0] = screen_id;
	return disp_ioctl(NULL, DISP_GET_SCN_HEIGHT, (void*)arg);

}

#else
int board_display_layer_request(void)
{
	return 0;
}

int board_display_layer_release(void)
{
	return 0;
}
int board_display_wait_lcd_open(void)
{
	return 0;
}
int board_display_wait_lcd_close(void)
{
	return 0;
}
int board_display_set_exit_mode(int lcd_off_only)
{
	return 0;
}
int board_display_layer_open(void)
{
	return 0;
}

int board_display_layer_close(void)
{
	return 0;
}

int board_display_layer_para_set(void)
{
	return 0;
}

int board_display_show_until_lcd_open(int display_source)
{
	return 0;
}

int board_display_show(int display_source)
{
	return 0;
}

int board_display_framebuffer_set(int width, int height, int bitcount, void *buffer)
{
	return 0;
}

void board_display_set_alpha_mode(int mode)
{
	return ;
}

int board_display_framebuffer_change(void *buffer)
{
	return 0;
}
int board_display_device_open(void)
{
	return 0;
}

int borad_display_get_screen_width(void)
{
	return 0;
}

int borad_display_get_screen_height(void)
{
	return 0;
}

void board_display_setenv(char *data)	
{
	return;
}

#endif