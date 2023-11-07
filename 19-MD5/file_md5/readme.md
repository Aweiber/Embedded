/*关于md5的使用方法说明*/

1.使用"mbedtls_md5_context"创建结构体实例
	如：static mbedtls_md5_context md5Context = {0};

2.将实例清空
	如：memset(md5Context, 0, sizeof(mbedtls_md5_context));

3.使用函数"mbedtls_md5_init"初始化实例
	如：mbedtls_md5_init(&md5Context);
	
4.使用函数"mbedtls_md5_starts"开始md5计算
	如：mbedtls_md5_starts(&md5Context);
	
5.使用函数"mbedtls_md5_update"开始分块计算
	如：mbedtls_md5_update(&md5Context, (const char *)data, datalenth);
	其中md5Context为存放结果的实例，data为数据缓冲区，datalenth为缓冲区计算数据长度

6.使用函数"mbedtls_md5_finish"结束计算
	如：uint8_t md5Str[16];
		char md5Value[33];
		mbedtls_md5_finish(&md5Context, md5Str);
		sprintf(md5Value, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", md5Str[0], md5Str[1], md5Str[2],
        md5Str[3], md5Str[4], md5Str[5], md5Str[6], md5Str[7], md5Str[8], md5Str[9], md5Str[10], md5Str[11], md5Str[12], md5Str[13], md5Str[14], md5Str[15]);
	将得到的十六进制数md5Str，将每个字节转为两个十六进制字符，并保存到md5Value中
	因此md5Value为计算所得的md5哈希值