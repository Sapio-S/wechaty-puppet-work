// All Rights Reserved.
// *File �� WeWorkFinanceSdk_C.h
// @Brief����ȡ��ҵ�����¼��ý����Ϣsdkͷ�ļ�

#pragma once
//������	����˵��
//10000	�������������������
//10001	������������������
//10002	���ݽ���ʧ��
//10003	ϵͳʧ��
//10004	��Կ�����¼���ʧ��
//10005	fileid����
//10006	����ʧ��
//10007 �Ҳ�����Ϣ���ܰ汾��˽Կ����Ҫ���´���˽Կ��
//10008 ����encrypt_key����
//10009 ip�Ƿ�
//10010 ���ݹ���

typedef struct WeWorkFinanceSdk_t WeWorkFinanceSdk_t;

// ����
typedef struct Slice_t {
    char* buf;
    int len;
} Slice_t;

typedef struct MediaData {
    char* outindexbuf;
    int out_len;
    char* data;    
    int data_len;
    int is_finish;
} MediaData_t;

//#ifdef __cplusplus
extern "C"  {
//#endif   

	 WeWorkFinanceSdk_t* NewSdk();
                               

	/**
	 * ��ʼ������
	 * Returnֵ=0��ʾ��API���óɹ�
	 * 
	 * @param [in]  sdk			NewSdk���ص�sdkָ��
	 * @param [in]  corpid      ������ҵ����ҵid�����磺wwd08c8exxxx5ab44d����������ҵ΢�Ź����--�ҵ���ҵ--��ҵ��Ϣ�鿴
	 * @param [in]  secret		�������ݴ浵��Secret����������ҵ΢�Ź����--�������--�������ݴ浵�鿴
	 *						
	 *
	 * @return �����Ƿ��ʼ���ɹ�
	 *      0   - �ɹ�
	 *      !=0 - ʧ��
	 */
	 int Init(WeWorkFinanceSdk_t* sdk, const char* corpid, const char* secret);

	/**
	 * ��ȡ�����¼����
	 * Returnֵ=0��ʾ��API���óɹ�
	 * 
	 *
	 * @param [in]  sdk				NewSdk���ص�sdkָ��
	 * @param [in]  seq				��ָ����seq��ʼ��ȡ��Ϣ��ע����Ƿ��ص���Ϣ��seq+1��ʼ���أ�seqΪ֮ǰ�ӿڷ��ص����seqֵ���״�ʹ����ʹ��seq:0
	 * @param [in]  limit			һ����ȡ����Ϣ���������ֵ1000��������1000���᷵�ش���
	 * @param [in]  proxy			ʹ�ô����������Ҫ�����������ӡ��磺socks5://10.0.0.1:8081 ���� http://10.0.0.1:8081
	 * @param [in]  passwd			�����˺����룬��Ҫ���������˺����롣�� user_name:passwd_123
	 * @param [in]  timeout			��ʱʱ�䣬��λ��
	 * @param [out] chatDatas		���ر�����ȡ��Ϣ�����ݣ�slice�ṹ��.���ݰ���errcode/errmsg���Լ�ÿ����Ϣ���ݡ�ʾ�����£�

	 {"errcode":0,"errmsg":"ok","chatdata":[{"seq":196,"msgid":"CAQQ2fbb4QUY0On2rYSAgAMgip/yzgs=","publickey_ver":3,"encrypt_random_key":"ftJ+uz3n/z1DsxlkwxNgE+mL38H42/KCvN8T60gbbtPD+Rta1hKTuQPzUzO6Hzne97MgKs7FfdDxDck/v8cDT6gUVjA2tZ/M7euSD0L66opJ/IUeBtpAtvgVSD5qhlaQjvfKJc/zPMGNK2xCLFYqwmQBZXbNT7uA69Fflm512nZKW/piK2RKdYJhRyvQnA1ISxK097sp9WlEgDg250fM5tgwMjujdzr7ehK6gtVBUFldNSJS7ndtIf6aSBfaLktZgwHZ57ONewWq8GJe7WwQf1hwcDbCh7YMG8nsweEwhDfUz+u8rz9an+0lgrYMZFRHnmzjgmLwrR7B/32Qxqd79A==","encrypt_chat_msg":"898WSfGMnIeytTsea7Rc0WsOocs0bIAerF6de0v2cFwqo9uOxrW9wYe5rCjCHHH5bDrNvLxBE/xOoFfcwOTYX0HQxTJaH0ES9OHDZ61p8gcbfGdJKnq2UU4tAEgGb8H+Q9n8syRXIjaI3KuVCqGIi4QGHFmxWenPFfjF/vRuPd0EpzUNwmqfUxLBWLpGhv+dLnqiEOBW41Zdc0OO0St6E+JeIeHlRZAR+E13Isv9eS09xNbF0qQXWIyNUi+ucLr5VuZnPGXBrSfvwX8f0QebTwpy1tT2zvQiMM2MBugKH6NuMzzuvEsXeD+6+3VRqL"}]}

	 *
	 * @return �����Ƿ���óɹ�
	 *      0   - �ɹ�
	 *      !=0 - ʧ��	
	 */		
	 int GetChatData(WeWorkFinanceSdk_t* sdk, unsigned long long seq, unsigned int limit, const char *proxy,const char* passwd, int timeout,Slice_t* chatDatas);

	/**
     * @brief ��������.��ҵ΢�����н�������
     * @param [in]  encrypt_key, getchatdata���ص�encrypt_random_key,ʹ����ҵ�Գֶ�Ӧ�汾��ԿRSA���ܺ������
     * @param [in]  encrypt_msg, getchatdata���ص�encrypt_chat_msg
     * @param [out] msg, ���ܵ���Ϣ����
	 * @return �����Ƿ���óɹ�
	 *      0   - �ɹ�
	 *      !=0 - ʧ��
     */
	 int DecryptData(const char* encrypt_key, const char* encrypt_msg, Slice_t* msg);

	/**
	 * ��ȡý����Ϣ����
	 * Returnֵ=0��ʾ��API���óɹ�
	 * 
	 *
	 * @param [in]  sdk				NewSdk���ص�sdkָ��
	 * @param [in]  sdkFileid		��GetChatData���ص�������Ϣ�У�ý����Ϣ������sdkfileid
	 * @param [in]  proxy			ʹ�ô����������Ҫ�����������ӡ��磺socks5://10.0.0.1:8081 ���� http://10.0.0.1:8081
	 * @param [in]  passwd			�����˺����룬��Ҫ���������˺����롣�� user_name:passwd_123
	 * @param [in]  indexbuf		ý����Ϣ��Ƭ��ȡ����Ҫ����ÿ����ȡ��������Ϣ���״β���Ҫ��д��Ĭ����ȡ512k������ÿ�ε���ֻ��Ҫ���ϴε��÷��ص�outindexbuf���뼴�ɡ�
	 * @param [in]  timeout			��ʱʱ�䣬��λ��
	 * @param [out] media_data		���ر�����ȡ��ý������.MediaData�ṹ��.���ݰ���data(��������)/outindexbuf(�´�����)/is_finish(��ȡ��ɱ��)
	 
	 *
	 * @return �����Ƿ���óɹ�
	 *      0   - �ɹ�
	 *      !=0 - ʧ��
	 */
	 int GetMediaData(WeWorkFinanceSdk_t* sdk, const char* indexbuf,
                     const char* sdkFileid,const char *proxy,const char* passwd, int timeout, MediaData_t* media_data);

    /**
     * @brief �ͷ�sdk����NewSdk�ɶ�ʹ��
     * @return 
     */
	 void DestroySdk(WeWorkFinanceSdk_t* sdk);


    //--------------����ӿ�Ϊ��������������python�ȵ���c�ӿڣ�����ʹ��--------------
	 Slice_t* NewSlice();

    /**
     * @brief �ͷ�slice����NewSlice�ɶ�ʹ��
     * @return 
     */
	 void FreeSlice(Slice_t* slice);

    /**
     * @brief Ϊ���������ṩ��ȡ�ӿ�
     * @return ����bufָ��
     *     !=NULL - �ɹ�
     *     NULL   - ʧ��
     */
	 char* GetContentFromSlice(Slice_t* slice);
	 int GetSliceLen(Slice_t* slice);

	// ý���¼��ع���

	 MediaData_t*  NewMediaData();

	 void FreeMediaData(MediaData_t* media_data);

	 char* GetOutIndexBuf(MediaData_t* media_data);
	 char* GetData(MediaData_t* media_data);
	 int GetIndexLen(MediaData_t* media_data);
	 int GetDataLen(MediaData_t* media_data);
	 int IsMediaDataFinish(MediaData_t* media_data);

//#ifdef __cplusplus
}
//#endif 
