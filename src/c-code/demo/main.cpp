#include "WeWorkFinanceSdk_C.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <json/json.h>

#include <windows.h>
#include <fstream>
#include <assert.h>
#include <time.h>

#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"libcrypto.lib")

using std::string;
// 私钥内容
static const char* priKey = 
"-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDjRtT31qGaLP304i\n"
"CtMD/N0CUiBXNFbTchB46rKO/OPYHxsIhRRBPESgZOHJq/MRyjcg8iL2t8k9UkPRjr\n"
"kyShk3sqhL3GTkA+IAnqN53FTWx2Sxukh+fCkkoVSwtR3e8g18waIEeilXiAon+tLn\n"
"FzJCXkjkzVlGRllYh0LwGzvRhtyAt+sb657Bq/fYM4/hY3vI4wm07UVG4lDBmQRM1m\n"
"PmmPpSPkJlNu9Rve5al1u+xojAOq7LF/I9Nyz0KVUcC8AAdChbFQTnYjCV+nVq+ELH\n"
"Dt437+FYl3WrVU2oTTyU3Gfj9Pc5jR6BC0CYiqGxQI68FY7MKPVnxcPgILM9r9AgMB\n"
"AAECggEALOqavEBWv7mUlwj5nECk22//ZrHdDiN/ea+cqob0nbXaMIHK7wSdQ3W9uU\n"
"I98vspxcOM6hylCYHN5dONUUFLRAN5m/WP69kbKSv7yaYkMBIVFWvSkrmpO/LdYUfr\n"
"fMp3r+32EFPA1Ijlv8ZdpURAd5j1RC47URp6nPjods2zcGnXSYTEnSgXfMtgzXDZPq\n"
"nY8U8syzsMuDz/t7T/RBDN2Og90hQ12GqXapMCEbs3S8o7mcIH+Kw8VsjyUaUhxAom\n"
"ifS4O26MkFrGJrLJDepmiIqkuzUASenO628gDeN2d/gA4cugR30GCmcEf/vkw66NU+\n"
"gQkJ6Co2JJWGycArmGoQKBgQDzcCyXxaptpPrfQ+T2a1XuiifjO2sjW5ZVLZniHs/M\n"
"02vXQtEu/GTucQ27rmEF/arAec91t6uXF81t+EY/bzxR63WG1KUQZMZ3JDDJ6m/Di4\n"
"OmVShs042FXHJx3QHWwGUSO6KrwnT7Jp0DLBHAMNUYwjYpny93tCVIvpW/p9VhKwKB\n"
"gQDvASn1B10+U6kEmSHTfmBgELiZW9AITbMHo2PZBe639ik/Ze3j5Q8JLcRWxEIMe1\n"
"uASn6JT6w2A+6ld4qvKaSkBUoaR3pS1EzjS4RPhIrOgJ8KrboNmh/MBrTOCtSkC9T0\n"
"+/vzT7JKqo+KoCcgPXAFqedQFScDCABoDxJFkFMQdwKBgARHdIE2eEh1HfPOvbeXK2\n"
"uGAi18XeXBNmNI6euPnxd6n+qowgSRazTFx/gEmLACRX4cpgjrsZh2UqYcNx/mVKkb\n"
"+nTkSAKBwvtVJ4ZlGJHXYfkKhitjjJlLhrd5qw6bUyt+4N0kDepcQ2ZK4AUA1r4Yq9\n"
"sPcUYpZH5i2z8DND0BAoGAH36LxNofk2L61rxHFNOGZ6JMcQkENNPoT41cTvDVMDCj\n"
"5tLLskgw3wjE4poiCfsHZ7RpUDbLfqKuA+fowoQ+Sc7YoPXiTJ/lMd51pPTWRbf1PT\n"
"b/WWOB5STNFome068rdMzvunQ6DlTDbYUFIt51sIlfDdJ7vY6EXzH9OmV2TKkCgYAq\n"
"9CSyuhBFM1XWGebRhurDDQrLAyHpZqi4itFZaAFaLOpBJguHIIfb4PLSjA1rEtHMGw\n"
"8Z+QM3RGgEjSltMUExy+fw5EmH+mXp+HSyx1EzeSPdwUX5sU9dT7CKeXpBlrWV7gJU\n"
"6/ww3TsoM/nFpHuLNuHe+oY0+ouHUPj1iSRuHw==\n"
"-----END RSA PRIVATE KEY-----\n";

RSA * createRSA(const char* key, int publickey)
{
	RSA *rsa = NULL;
	BIO *keybio;
	keybio = BIO_new_mem_buf(key, -1);
	if (keybio == NULL){
		printf("Failed to create key BIO");
		return 0;
	}
	if (publickey){
		PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
	}
	else{
		PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
	}
	if (rsa == NULL){
		printf("Failed to create RSA");
	}
	return rsa;
}

int private_decrypt(unsigned char* enc_data, int data_len, const char* key, unsigned char*decrypted){
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

int main(int argc, char *argv[])
{
	int ret = 0;
	// new sdk api
	WeWorkFinanceSdk_t* sdk = NewSdk();
	// init api
	ret = Init(sdk, "ww5ecc1acd5dce6e9d", "Gd-6TWsVQRL0DHM_TePEYtONRG0XTV-HioEWJUu-lsQ");
	if (ret != 0) {
		printf("init sdk err ret:%d\n", ret);
	}
	printf("after init, %d\n", ret);
	Slice_t* chatDatas = NewSlice();
	Json::Reader reader;
	Json::Value root;
	ret = GetChatData(sdk, 0, 5, nullptr, nullptr, 100, chatDatas);
	std::string strJsonContent = chatDatas->buf;
	printf("GetChatData len:%d data:%s\n", chatDatas->len, GetContentFromSlice(chatDatas));
	if (reader.parse(strJsonContent.c_str(), root)) {
		if (root["errcode"].asInt() == 0) {
			if (root["chatdata"].isArray()) {
				int nArraySize = root["chatdata"].size();
				for (int i = 0; i < nArraySize; i++)
				{
					unsigned char decrypted[4098] = {};
					int seq = root["chatdata"][i]["seq"].asInt();
					std::string msgid = root["chatdata"][i]["msgid"].asString();
					int publickey_ver = root["chatdata"][i]["publickey_ver"].asInt();
					std::string encrypt_random_key = root["chatdata"][i]["encrypt_random_key"].asString();
					char * randomKey = (char*)encrypt_random_key.data();
					std::string encrypt_chat_msg = root["chatdata"][i]["encrypt_chat_msg"].asString();

					// 解密第一步，base64decode
					char* base_decode = Base64Decode(randomKey, strlen(randomKey), false);
					// 解密第二步，企业rsa解密
					int ret = private_decrypt((unsigned char*)base_decode, 256, priKey, decrypted);

					// RSA解密成功
					if (ret > 0)
					{
						ret = DecryptData((const char*)(char*)decrypted, encrypt_chat_msg.c_str(), chatDatas);
						// 微信SDK解密成功
						if (ret == 0) {
							std::string chardata = GetContentFromSlice(chatDatas);
							Json::Reader datareader;
							Json::Value dataroot;
							std::cout<<chardata<<std::endl;
							// 解析json数据
							if (datareader.parse(chardata.c_str(), dataroot)) {
								std::string msgid = dataroot["msgid"].asString();
								std::string from = dataroot["from"].asString();
								double msgtime = dataroot["msgtime"].asDouble();
								std::string msgtype = dataroot["msgtype"].asString();
								if (msgtype == "text")// 文本类型
								{
									Json::Value  &text = dataroot["text"];
									std::string content = text["content"].asString();
									content = StringToUtf(content);
									std::cout << content << std::endl;
								}
							}
						}
						else {
							printf("can't decypt the message via wechat SDK correctly...\n");
						}
					}
					else {
						printf("can't decypt the message via rsa correctly...\n");
					}
				}
			}
		}
		else
		{
			printf("PraseChatData err ret\n");
			return -1;
		}
	}
	DestroySdk(sdk);
	return ret;
}