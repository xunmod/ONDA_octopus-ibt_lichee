#ifndef __KEY_H__
#define __KEY_H__

typedef struct EFUSE_KEY
{
	char name[32];
	int key_index;
	int store_max_bit;
	int show_bit_offset;
	int burned_bit_offset;
	int reserve[4];
}
efuse_key_map_t;

typedef struct
{
    //以下信息重复，表示每个key的信息
    char  name[64];    //key的名称
    u32 len;           //key数据段的总长度
    u8  *data_addr;
	u8  *key_data;   //这是一个数组，存放key的全部信息，数据长度由len指定
}
sunxi_efuse_key_info_t;


#endif /*__KEY_H__*/

