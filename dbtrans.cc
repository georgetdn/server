// dbtrans.cpp : Defines the access to database.
//
#include <iostream>
#include <mysql.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

#include <chrono>
#include <sstream>

#include "common.h"
#include "dbtrans.h"
// TODO: This is an example of a library function
static int GetNewTDNsgn(std::string amount, std::string& TDNsgn);

std::string InsertTDNDB(MYSQL *  con,
			const std::string trId,
			const std::string  trType,
			std::string& TDNsgn,
			std::string amount,
			std::string  pin,
			std::string  cert,
			std::string note)
{
	TRACE ("Enter InsertTDNDB%s", "\n");


	GetNewTDNsgn( amount, TDNsgn)	;

	if (con == NULL)
	{
		TRACE("DB Error %s\n", mysql_error(con)) ;
		return mysql_error(con);
	}
	
    const std::string q =  "INSERT INTO `tokenmast`(`tdn_id`, `tdn_signature`, `tdn_status`, `tdn_dollar_value`, `tdn_pin`, `tdn_cert`, `tdn_trans_create_id`, `tdn_trans_ip`,  `tdn_trans_create_type`, `tdn_note`) VALUES "\
                                                              "('"+   TDNsgn.substr(0, 20) +"','"+  TDNsgn +"','1','"+  amount +"','"+ pin+"','"+ cert+"','"+trId+"','999.999.999.999','"+trType+"','"+note+"') "; 
//	TRACE("\nQuery  %s\n", q.c_str()) ;
	if (mysql_query(con, q.c_str()))
	{
		TRACE("DB Error %s\n", mysql_error(con)) ;
		return mysql_error(con);
	}
	TRACE ("Exit InsertTDNDB%s", "\n");

  	return "OK";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string UpdateBankAcc(MYSQL *  con,
			const std::string trId,
			const std::string  trType,  // debit or credit
			std::string& TDNsgn,
			std::string amount,
			std::string note)
{
	if (trType == std::to_string(INITIAL_ISSUE))
	{
		TRACE ("Debiting TDNSYS Co account%s", "\n");
		const std::string qq =  "INSERT INTO `TDNSYS`.`tdnsysco` (`debit`,`transactionNo`) VALUES ('"+ amount +"','"+trId+"') ";
		//   TRACE("\nQuery  %s\n", qq.c_str()) ;
		if (mysql_query(con, qq.c_str()))
		{
			TRACE("DB Error %s\n", mysql_error(con)) ;
			return mysql_error(con);
		}
		TRACE ("Debiting bank TDN inventory account%s", "\n");
		const std::string qqq =  "INSERT INTO `TDNSYS`.`bankinventory` (`tdn_signature`,`debit`,`transactionNo`) VALUES ('"+ TDNsgn +"','"+ amount +"','"+trId+"') ";
		//   TRACE("\nQuery  %s\n", qqq.c_str()) ;
		if (mysql_query(con, qqq.c_str()))
		{
			TRACE("DB Error %s\n", mysql_error(con)) ;
			return mysql_error(con);
		}	
	
	}
	else
	{
		TRACE ("Crediting TDNSYS Co account%s", "\n");
		const std::string qq =  "INSERT INTO `TDNSYS`.`tdnsysco` (`credit`,`transactionNo`) VALUES ('"+ amount +"','"+trId+"') ";
		//   TRACE("\nQuery  %s\n", qq.c_str()) ;
		if (mysql_query(con, qq.c_str()))
		{
			TRACE("DB Error %s\n", mysql_error(con)) ;
			return mysql_error(con);
		}
		TRACE ("Crediting bank TDN inventory account%s", "\n");
		const std::string qqq =  "INSERT INTO `TDNSYS`.`bankinventory` (`tdn_signature`,`credit`,`transactionNo`) VALUES ('"+ TDNsgn +"','"+ amount +"','"+trId+"') ";
		//   TRACE("\nQuery  %s\n", qqq.c_str()) ;
		if (mysql_query(con, qqq.c_str()))
		{
			TRACE("DB Error %s\n", mysql_error(con)) ;
			return mysql_error(con);
		}	
		
	}

  	return "OK";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string GetTDNdata( MYSQL *   con,  
			std::string &TDNsgn, 
			std::string& amount, 
			std::string& status,
			std::string& pin,
			std::string& cert)
{
	TRACE ("Enter GetTDNdata%s", "\n");
	// validate TDN signature
	for (int i = 0; i < 20; i++)
	{
		if	(
				(TDNsgn[i]  < 48) || 
				(TDNsgn[i]  > 57 && TDNsgn[i] < 65) || 
				(TDNsgn[i]  > 90 && TDNsgn[i] < 97) || 
				(TDNsgn[i]  > 122)
			)
		{

			status =  "3";
			TRACE("%s\n", "Invalid TDN signature input") ;
			TRACE ("Exit GetTDNdata%s", "\n");
			return ("OK");
		}
	}
//	TRACE(Before query "  << mysql_stat(con) << std::endl;
	std::string q =  "SELECT `tdn_signature`, `tdn_status`, `tdn_dollar_value`, `tdn_pin` ,`tdn_cert` FROM `tokenmast` WHERE `tdn_id` =  "; 

	q = q + "'"+ TDNsgn.substr(0, 20) + "'";

   if (mysql_query(con, q.c_str()))
   {
	    TRACE("After  query %s",mysql_stat(con));
		TRACE("DB Error %s\n", mysql_error(con)) ;
		TRACE ("Exit GetTDNdata%s", "\n");
		return mysql_error(con);
   }
  // 	TRACE(After  query "  << mysql_stat(con)<< std::endl;
//	TRACE ("after mysql_query%s", "\n");

	MYSQL_RES *result = mysql_store_result(con);

//	TRACE(After  MYSQL_RES *result = mysql_store_result(con); "  << mysql_stat(con)<< std::endl;

   if   (result == NULL)
   {
		TRACE("DB Error %s\n", mysql_error(con)) ;
		TRACE ("Exit GetTDNdata%s", "\n");
		return mysql_error(con);
	}
  // TRACE ("after MYSQL_RES *result %s", "\n");
	MYSQL_ROW row;

	row = mysql_fetch_row(result);
//	    	TRACE(After  ow = mysql_fetch_row(result); "  << mysql_stat(con)<< std::endl;
	if (!row || TDNsgn != row[0])
	{
		status =    "3";
		TRACE("%s\n", "Invalid TDN signature") ;
		TRACE ("Exit GetTDNdata%s", "\n");
		return ("OK");
	}
  // TRACE ("after mysql_fetch_row%s", "\n");
	status =  row[1] ;
	amount = row[2] ;
	pin = row[3];
	cert = row[4];

	TRACE ("Exit GetTDNdata%s", "\n");
	return "OK";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CancelTDN( MYSQL * con,
				const std::string trId,
				const std::string  trType,
				std::string & TDNsgn)
{
    TRACE ("Enter CancelTDN%s", "\n");

    std::string q =  "SELECT `tdn_signature` , `tdn_status` FROM `tokenmast` WHERE `tdn_id` =  ";
    q = q + '"'+ TDNsgn.substr(0, 20) + '"';
	   if (mysql_query(con, q.c_str()))
    {
	TRACE("DB Error 2 - %s\n", mysql_error(con)) ;
	return mysql_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if   (result == NULL)
    {
	TRACE("DB Error 3 %s\n", mysql_error(con)) ;
	return mysql_error(con);
    }

    MYSQL_ROW row;

    row = mysql_fetch_row(result);
    if (TDNsgn != row[0])
    {
	TRACE("%s\n", "Invalid TDN signature") ;
	return ("Invalid TDN signature");
    }
    if(std::string{row[1] } != "1")
    {
	TRACE("%s\n", "Invalid TDN status") ;
	return ("Invalid TDN statis");
    }
    q =  "UPDATE `tokenmast` SET `tdn_status`= 2, `tdn_trans_cancel_id`='" + trId+"',  `tdn_trans_cancel_type`='" +trType + "'  ,`tdn_cancel_timestamp`=CURRENT_TIMESTAMP   WHERE  `tdn_id` =  "; 
    q = q + '"'+ TDNsgn.substr(0, 20) + '"';

    if (mysql_query(con, q.c_str()))
    {
    	TRACE("DB Error %s\n", mysql_error(con)) ;
	return mysql_error(con);
    }

    TRACE ("Exit CancelTDN%s", "\n");
    return "OK";
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string UpdateTDN( MYSQL * con, 
			std::string & TDNsgn,
			std::string pin,
			std::string  cert,
			std::string  note)		
{
    TRACE ("Enter UpdateTDN%s", "\n");

    std::string q =  "SELECT `tdn_signature` , `tdn_status` FROM `tokenmast` WHERE `tdn_id` =  "; 
    q = q + '"'+ TDNsgn.substr(0, 20) + '"';
    if (mysql_query(con, q.c_str()))
    {
	TRACE("DB Error 2 - %s\n", mysql_error(con)) ;
	return mysql_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

   if   (result == NULL)
   {
	TRACE("DB Error 3 %s\n", mysql_error(con)) ;
	return mysql_error(con);
    }

    MYSQL_ROW row;

    row = mysql_fetch_row(result);
    if (TDNsgn != row[0])
    {
	TRACE("%s\n", "Invalid TDN signature") ;
	return ("Invalid TDN signature");
    }
    if(std::string{row[1] } != "1")
    {
    	TRACE("%s\n", "Invalid TDN status") ;
	TRACE("Disonnected%s", "\n");
	return ("Invalid TDN statis");
    }
    q =  "UPDATE `tokenmast` SET `tdn_pin`='"+pin +"',`tdn_cert`='"+cert+"'`tdn_note`='"+note+"'  WHERE  `tdn_id` =  ";
    q = q + '"'+ TDNsgn.substr(0, 20) + '"';

    if (mysql_query(con, q.c_str()))
    {
	TRACE("DB Error %s\n", mysql_error(con)) ;
	return mysql_error(con);
    }
    TRACE ("Exit UpdateTDN%s", "\n");
    return "OK";
}/*//////////////////////////////////////////////////////////////////////////////////////////////////////
std::string FormatAmount(std::string& amount)
{
	TRACE("Amount IN %s\n", amount.c_str());


	bool flgCntLead = true;
	bool flgDotFound = false;
	trim(amount);
	size_t ln = amount.length();
	//Check for nonnumerical
	for (int i = 0; i < ln; i++)
	{
		if (int(amount.c_str()[i]) < 48 || int(amount.c_str()[i]) > 57)
		{
			if (amount.c_str()[i] != '.')
			{
				TRACE("Amount contains non numerical values.%s\n", "\n");
				return "Amount contains non numerical values.";
			}
			else
			{
				if (flgDotFound)
				{
					TRACE("Amount contains two dots.%s\n", "\n");
					return "Amount contains two dots.";
				}
				else
				{
					flgDotFound = true;
				}
			}
		}

	}
	long double amt{ stold(amount) };
	if (amt < .001)
	{
		TRACE("Invalid amount input (zero value) %s\n", "\n");
		return "Invalid amount input  (zero value) ";

	}
	if (amt > MAX_AMOUNT)
	{
		TRACE("Invalid amount input (amount too large)%s\n", "\n");
		return  "Invalid amount input  (amount too large)";

	}
	if(( long(amt * 1000) - ((long)(amt * 100.0)) * 10) > 0)
	{
		TRACE("Invalid amount input (more then two decimals) %s\n", "\n");
		return "Invalid amount input  (more then two decimals) ";
	}
	unsigned  i = amt * 100;
	char buff[AMOUNT_LEN+1];
	buff[AMOUNT_LEN] = 0;
	sprintf(buff, "%010u", i);
	amount = buff;
	return "OK";
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////
int GetNewTDNsgn( std::string amount, std::string& TDNsgn)
{
// expexts amount in cents left padded with 0
    TRACE ("Enter GetTDNsgn %s \n",  " \n");
    TRACE ("TDN Amount %s\n", amount.c_str()); 
    int keylen;
    char pem_key[2048];
    BIGNUM* bn;
    bn = BN_new();
    BN_set_word(bn, RSA_F4);

	// get timestamp

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
	TDNsgn = ss.str();   //timestamp
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
	TDNSgnTmp = TDNSgnTmp.substr(142, RANDOMNO_LEN);
	

	if (TDNsgn.length() != DATETIME_LEN)
	{
		if (TDNsgn.length() < DATETIME_LEN)
		{
			TDNsgn += TDNSgnTmp.substr(80, DATETIME_LEN - TDNsgn.length());
		}
		else
		{
			TDNsgn = TDNsgn.substr(1, DATETIME_LEN);
		}
	}
	TDNsgn += TDNSgnTmp;
	
    TDNsgn += "TDBank~~~~194.195.86.212::50051~";
	TDNsgn += amount;
    TRACE ("TDNsgn %s\n", TDNsgn.c_str()); 
	std::cout <<   "TDNsgn length " <<  TDNsgn.length() << std::endl;

    BIO_free_all(bio);
    RSA_free(rsa);
    TRACE ("Exit GetTDNsgn  %s\n",  "\n");
    return 0;

}

std::string GetTransId(MYSQL *con, std::string& trId)
{
    if (mysql_query(con, "UPDATE  `transactId`  SET id=LAST_INSERT_ID(id+1);"))
    {
       TRACE("DB Error %s\n", mysql_error(con)) ;
       return mysql_error(con);
    }

    if (mysql_query(con, "SELECT LAST_INSERT_ID();"))
    {
       TRACE("DB Error %s\n", mysql_error(con)) ;
       return mysql_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if   (result == NULL)
    {
	TRACE("DB Error %s\n", mysql_error(con)) ;
	return mysql_error(con);
    }

    MYSQL_ROW row;

    row = mysql_fetch_row(result);
    trId=  row[0] ;
    TRACE("Transaction ID %s\n", trId.c_str()) ;
    return "OK";

}
static  MYSQL *con = NULL;
MYSQL * GetConnection (void)
{
	TRACE ("Enter GetConnection%s", "\n");
	con = mysql_init(NULL);
	if (mysql_real_connect(con, "localhost", "georged", "W3Se$Xdr%george",
			"TDNSYS", 0, NULL, 0) == NULL)
	{
		TRACE("DB Error 1 -%s\n", mysql_error(con)) ;
		mysql_close(con);
		TRACE ("Exit GetConnection%s", "\n");
		return NULL;
	}

	TRACE("Connected%s", "\n");
	TRACE ("Exit GetConnection%s", "\n");
	return con;

}
void ReleaseConnection( MYSQL *  con)
{
	TRACE ("Enter ReleaseConnection%s", "\n");
     mysql_close(con);
 	TRACE("Disonnected%s", "\n");
	TRACE ("Exit ReleaseConnection%s", "\n");
}
