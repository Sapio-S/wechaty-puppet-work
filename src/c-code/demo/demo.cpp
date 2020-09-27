// demo.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif
	__declspec(dllexport) char* getchat(int, int, int);
	__declspec(dllexport) void initial(char*, char*);
	__declspec(dllexport) void destroy();
	__declspec(dllexport) int getMedia(int, char*, char*, char*);
	__declspec(dllexport) int removeFile(char*);
	__declspec(dllexport) char* getMedia2(int, char*);
#ifdef __cplusplus
}
#endif

#include "md5sum.c"

#include "WeWorkFinanceSdk_C.h"
#include <stdio.h>
#include <stdint.h>
// #include <ctype.h>
#include <stdlib.h>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <windows.h>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"libcrypto.lib")

using std::string;
using namespace rapidjson;

static char* priKey;

static WeWorkFinanceSdk_t* sdk;
static string cache_content;


#define STR_VALUE(val) #val
#define STR(name) STR_VALUE(name)

#define PATH_LEN 256
#define MD5_LEN 32

/*int CalcFileMD5(char *file_name, char *md5_sum)
{
#define MD5SUM_CMD_FMT "md5sum %." STR(PATH_LEN) "s 2>/dev/null"
	char cmd[PATH_LEN + sizeof(MD5SUM_CMD_FMT)];
	sprintf(cmd, MD5SUM_CMD_FMT, file_name);
#undef MD5SUM_CMD_FMT

	FILE *p = popen(cmd, "r");
	if (p == NULL) return 0;

	int i, ch;
	for (i = 0; i < MD5_LEN && isxdigit(ch = fgetc(p)); i++) {
		*md5_sum++ = ch;
	}

	*md5_sum = '\0';
	pclose(p);
	return i == MD5_LEN;
}*/

RSA * createRSA(const char* key, int publickey)
{
	RSA *rsa = NULL;
	BIO *keybio;
	keybio = BIO_new_mem_buf(key, -1);
	if (keybio == NULL) {
		printf("Failed to create key BIO");
		return 0;
	}
	if (publickey) {
		PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
	}
	else {
		PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
	}
	if (rsa == NULL) {
		std::cout << key << std::endl;
		printf("Failed to create RSA");
	}
	return rsa;
}

int private_decrypt(unsigned char* enc_data, int data_len, const char* key, unsigned char*decrypted) {
	RSA * rsa = createRSA(key, 0);
	int result = RSA_private_decrypt(data_len, enc_data, decrypted, rsa, RSA_PKCS1_PADDING);
	return result;
}

char * Base64Decode(char * input, int length, bool with_new_line)
{
	BIO * b64 = NULL;
	BIO * bmem = NULL;
	char * buffer = (char *)malloc(length);
	memset(buffer, 0, length);

	b64 = BIO_new(BIO_f_base64());
	if (!with_new_line) {
		BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	}
	bmem = BIO_new_mem_buf(input, length);
	bmem = BIO_push(b64, bmem);
	BIO_read(bmem, buffer, length);

	BIO_free_all(bmem);

	return buffer;
}

std::string StringToUtf(std::string strValue)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, strValue.c_str(), -1, NULL, 0);
	wchar_t * pwBuf = new wchar_t[nwLen + 1];//加上末尾'\0'
	memset(pwBuf, 0, nwLen * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strValue.c_str(), strValue.length(), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr = pBuf;
	delete[]pBuf;
	delete[]pwBuf;
	return retStr;
}

void initial(char * a, char* b) {
	// new sdk api
	sdk = NewSdk();
	std::ifstream file("private_key.txt", std::ios::in);
	file.seekg(0, std::ios::end);    // go to the end  
	int length = file.tellg();           // report location (this is the length)  
	file.seekg(0, std::ios::beg);
	priKey = (char*)malloc(sizeof(char) * 10240);//10k
	file.read(priKey, length);
	file.close();
	
	// init api
	int ret = Init(sdk, a, b);
	if (ret != 0) {
		printf("Wechat Work Error: init sdk err ret:%d\n", ret);
	}
}

void destroy() {
	DestroySdk(sdk);
}

char* getchat(int firstTime, int count, int seq_) {
	// 重试发送
	if (firstTime != 0) {
		return (char*)cache_content.c_str();
	}
	int ret = 0;

	Slice_t* chatDatas = NewSlice();
	ret = GetChatData(sdk, seq_, count, nullptr, nullptr, 5, chatDatas);
	if (ret != 0) {
		std::cout << "Wechat Work Error: error occurs when trying to get chat data, ret: " << ret << std::endl;
		std::cout << "please check https://work.weixin.qq.com/api/doc/90000/90135/91552 for more information\n";
	}
	string strJsonContent = chatDatas->buf;

	Document root;
	root.Parse((char*)strJsonContent.c_str());

	// 返回时的JSON
	cache_content = "";
	if (root["errcode"].GetInt() == 0) {
		if (root["chatdata"].IsArray()) {
			const rapidjson::Value& array = root["chatdata"];
			size_t len = array.Size();

			// 传递的JSON字符串，形式为{"1":"str", "2":"str"}

			for (size_t i = 0; i < len; i++) {
				const rapidjson::Value& object = array[i];
				if (object.IsObject()) {
					unsigned char decrypted[4098] = {};

					int publickey_ver;
					string msgid, encrypt_random_key, encrypt_chat_msg;
					int seq;
					if (object.HasMember("seq") && object["seq"].IsInt()) {
						seq = object["seq"].GetInt();
					}
					if (object.HasMember("msgid") && object["msgid"].IsString()) {
						msgid = object["msgid"].GetString();
					}
					if (object.HasMember("publickey_ver") && object["publickey_ver"].IsInt()) {
						publickey_ver = object["publickey_ver"].GetInt();
					}
					if (object.HasMember("encrypt_random_key") && object["encrypt_random_key"].IsString()) {
						encrypt_random_key = object["encrypt_random_key"].GetString();
					}
					if (object.HasMember("encrypt_chat_msg") && object["encrypt_chat_msg"].IsString()) {
						encrypt_chat_msg = object["encrypt_chat_msg"].GetString();
					}
					char * randomKey = (char*)encrypt_random_key.data();

					// 解密第一步，base64decode
					char* base_decode = Base64Decode(randomKey, strlen(randomKey), false);

					// 解密第二步，企业rsa解密
					int ret = private_decrypt((unsigned char*)base_decode, 256, priKey, decrypted);

					// RSA解密成功
					if (ret > 0)
					{
						ret = DecryptData((const char*)(char*)decrypted, encrypt_chat_msg.c_str(), chatDatas);
						// 微信SDK解密成功
						if (ret == 0 && seq != -1) {
							string str = GetContentFromSlice(chatDatas);
							cache_content += str;
						}
					}
					else {
						std::cout << "Wechat Work Error: rsa decode fail\n";
					}
				}
			}
		}
		else {
			std::cout << "Wechat Work Error\n";
		}
	}
	else {
		std::cout << "Wechat Work Error: code: " << root["errcode"].GetInt() 
			<< " "<< root["errmsg"].GetString() << std::endl;
	}
	return (char*)cache_content.c_str();
}


static char* indexbuf;
static bool sendempty = false;

int getMedia(int size, char* sdkFileid, char* tmp, char* md5) {

	MediaData_t media;

	int ret = GetMediaData(sdk, nullptr, sdkFileid, nullptr, nullptr, 100, &media);
	FILE* f = fopen(tmp, "wb");
	if (ret != 0) {
		std::cout << "Wechat Work Error: error occurs when trying to get media data, ret: " << ret << std::endl;
		std::cout << "please check https://work.weixin.qq.com/api/doc/90000/90135/91552 for more information\n";
		return -1;
	}
	
	while (!media.is_finish) {
		std::cout << "fecthing media data"<<std::endl;
		if (ret != 0) {
			std::cout << "Wechat Work Error: error occurs when trying to get media data, ret: " << ret << std::endl;
			std::cout << "please check https://work.weixin.qq.com/api/doc/90000/90135/91552 for more information\n";
			return -1;
		}
		else {
			fwrite(media.data, sizeof(char), media.data_len, f);
			ret = GetMediaData(sdk, media.outindexbuf, sdkFileid, nullptr, nullptr, 100, &media);
		}
	}

	fwrite(media.data, sizeof(char), media.data_len, f);
	fclose(f);

	/*unsigned char* filemd5 = realrun(tmp);
	for (int i = 0; i < 16; i++)
	{
		printf("%hhu", filemd5[i]);
	}
	if (strcmp(reinterpret_cast<char*>(filemd5), md5) == 0) {
		return 0;
	}
	else {
		std::cout << "media file " << tmp << " md5sum test failed!" << std::endl;
		//removeFile(tmp);
	}*/
}

char* getMedia2(int size, char* sdkFileid) {

	MediaData_t media;
	char* data = new char[size];
	int ret = GetMediaData(sdk, nullptr, sdkFileid, nullptr, nullptr, 100, &media);
	if (ret != 0) {
		std::cout << "Wechat Work Error: error occurs when trying to get media data, ret: " << ret << std::endl;
		std::cout << "please check https://work.weixin.qq.com/api/doc/90000/90135/91552 for more information\n";
		return nullptr;
	}

	while (!media.is_finish) {
		std::cout << "fecthing media data" << std::endl;
		if (ret != 0) {
			std::cout << "Wechat Work Error: error occurs when trying to get media data, ret: " << ret << std::endl;
			std::cout << "please check https://work.weixin.qq.com/api/doc/90000/90135/91552 for more information\n";
			return nullptr;
		}
		else {
			strcat(data, media.data);
			ret = GetMediaData(sdk, media.outindexbuf, sdkFileid, nullptr, nullptr, 100, &media);
		}
	}

	strcat(data, media.data);
	return data;
}

int removeFile(char* tmp) {
	return std::remove(tmp);
}