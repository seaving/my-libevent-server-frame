#include "sys_inc.h"
#include "log_trace.h"
#include "event_ssl.h"

void event_ssl_lib_init()
{
	ERR_load_BIO_strings();
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
}

SSL_CTX *event_ssl_server_init(char *ca_file, char *crt_file, char *key_file)
{
	SSL_CTX *ctx =  NULL;
	EC_KEY *ecdh = NULL;

	if (ca_file == NULL 
		|| crt_file == NULL 
		|| key_file == NULL)
	{
		return NULL;
	}

	ctx = SSL_CTX_new(SSLv23_server_method());
	if (ctx == NULL)
	{
		LOG_TRACE_NORMAL("SSL_CTX_new eror !\n");
		goto error;
	}

	//要求校验对方证书，若不需要则设置为SSL_VERIFY_NONE
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

	if (SSL_CTX_load_verify_locations(ctx, ca_file, NULL) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_load_verify_locations error !\n");
		goto error;
	}

	SSL_CTX_set_options(ctx, 
		SSL_OP_SINGLE_DH_USE 
		| SSL_OP_SINGLE_ECDH_USE 
		| SSL_OP_NO_SSLv2);

	ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
	if (ecdh == NULL)
	{
		LOG_TRACE_NORMAL("EC_KEY_new_by_curve_name error !\n");
		goto error;
	}

	if (SSL_CTX_set_tmp_ecdh(ctx, ecdh) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_set_tmp_ecdh error !\n");
		goto error;
	}

  	if (SSL_CTX_use_certificate_chain_file(ctx, crt_file) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_use_certificate_chain_file error !\n");
		goto error;
	}

  	if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_use_PrivateKey_file error !\n");
		goto error;
	}

  	if (SSL_CTX_check_private_key(ctx) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_check_private_key error !\n");
		goto error;
	}

	return ctx;

error:
	SSL_CTX_free(ctx);
	return NULL;
}

SSL_CTX *event_ssl_client_init(char *ca_file, char *crt_file, char *key_file)
{
	SSL_CTX *ctx =  NULL;

	if (ca_file == NULL 
		|| crt_file == NULL 
		|| key_file == NULL)
	{
		return NULL;
	}

	ctx = SSL_CTX_new(SSLv23_server_method());
	if (ctx == NULL)
	{
		LOG_TRACE_NORMAL("SSL_CTX_new eror !\n");
		goto error;
	}

	//要求校验对方证书，若不需要则设置为SSL_VERIFY_NONE
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

	if (SSL_CTX_load_verify_locations(ctx, ca_file, NULL) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_load_verify_locations error !\n");
		goto error;
	}

  	if (SSL_CTX_use_certificate_file(ctx, crt_file, SSL_FILETYPE_PEM) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_use_certificate_file error !\n");
		goto error;
	}

#if 0
	//加载自己的私钥目的是每次连接服务器都需要密码
	//也就是说服务器也要验证本客户端的证书，双向认证
	if (SSL_CTX_use_PrivateKey_file (ctx, key_file, SSL_FILETYPE_PEM) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_use_PrivateKey_file error !\n");
		goto error;
	}

	if (SSL_CTX_check_private_key(ctx) != 1)
	{
		LOG_TRACE_NORMAL("SSL_CTX_check_private_key error !\n");
		goto error;
	}
#endif

	return ctx;

error:
	SSL_CTX_free(ctx);
	return NULL;
}

