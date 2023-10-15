
/open database
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
      std::cout <<  mysql_error(con) << std::endl;
      return "Error DB";
  }


  if (mysql_real_connect(con, "92.249.45.2", "u757650154_GeorgeD", "W3Se$Xdr%sql",
          "u757650154_TDNSYS", 0, NULL, 0) == NULL)
  {
      std::cout <<  mysql_error(con) << std::endl;
    mysql_close(con);
    return "Error DB";
  }

	std::cout << "Connected" <<  std::endl;
	const std::string q = 
	"INSERT INTO `tokenmast`(`tdn_id`, `tdn_create_timestamp`, `tdn_signature`, `tdn_status`, `tdn_dollar_value`, `tdn_cancel_timestamp`, `tdn_public_key`, `tdn_pin`, `tdn_cert`, `tdn_connection_info`, `tdn_note`, `tdn_trans_create_id`, `tdn_trans_ip`, `tdn_trans_cancel_id`, `tdn_private_key`, `tdn_fed_transaction`, `tdn_trans_cancel_type`, `tdn_trans_create_type`) VALUES "\
	                                               "('" + tdn.substr(0, 20) +"','"+ tdn.substr(0, 10) +"','" + tdn +"','[value-4]','[value-5]','[value-6]','[value-7]','[value-8]','[value-9]','[value-10]','[value-11]','[value-12]','[value-13]','[value-14]','[value-15]','[value-16]','[value-17]','[value-18]')";
  
  if (mysql_query(con, q.c_str()))
  {
       return "Error DB";
  }

  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
  {
      return "Error DB";
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;

  while ((row = mysql_fetch_row(result)))
  {
      for(int i = 0; i < num_fields; i++)
      {
          printf("%s ", row[i] ? row[i] : "NULL");
      }

      printf("\n");
  }
  mysql_free_result(result);
  mysql_close(con);
  	std::cout << "Disconnected" <<  std::endl;
////////////////////






#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <chrono>
#include <sstream>

#include <assert.h>
#include <string.h>

int genrateTDNSgn(std::string& TDNsgn)
{
    int keylen;
    char pem_key[2048];
    BIGNUM* bn;
    bn = BN_new();
    BN_set_word(bn, RSA_F4);

    RSA* rsa;
    rsa = RSA_new();
    RSA_generate_key_ex(
        rsa,  /* pointer to the RSA structure */
        1024, /* number of bits for the key - 2048 is a good value */
        bn,   /* exponent allocated earlier */
        NULL /* callback - can be NULL if progress isn't needed */
    );

    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);

    keylen = BIO_pending(bio);
    //   pem_key =(char *)( calloc(keylen + 1, 1)); /* Null-terminate */

    BIO_read(bio, pem_key, keylen);

    for (int i = 142; i < 280; i++)
        if (pem_key[i] < 48 or pem_key[i] > 122 or
            (pem_key[i] > 57 and pem_key[i] < 65) or
            (pem_key[i] > 90 and pem_key[i] < 97))
            pem_key[i] = 103;

    std::string  TDNSgnTmp = pem_key;
    TDNSgnTmp = TDNSgnTmp.substr(142, 138);
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();

    typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>>::type> Days; /* UTC: +8:00 */

    Days days = std::chrono::duration_cast<Days>(duration);
    duration -= days;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    duration -= milliseconds;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    duration -= microseconds;
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    int d = days.count();
    std::ostringstream ss;
    ss << days.count() << hours.count() << minutes.count() << seconds.count() << milliseconds.count() << microseconds.count() << nanoseconds.count();
    TDNsgn = ss.str();
    TDNsgn += TDNSgnTmp;
    TDNsgn = TDNsgn.substr(0, 128);
    BIO_free_all(bio);
    RSA_free(rsa);

    return 0;



    return 0;
}
