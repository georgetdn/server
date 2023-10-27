
// repository.cpp : Defines the functions for the static library.
//

#include <iostream>
#include <mysql.h>

#include <openssl/rsa.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <chrono>
#include <sstream>

#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

#include "common.h"
#include "repository.h"
#include "dbtrans.h"
// TODO: This is an example of a library function

std::string  InitialIssueDB(std::string& TDNsgn, std::string  amount, std::string  pin, std::string  cert, std::string note ) 
{
	TRACE("Enter Repository  InitialIssueDB.  %s", "\n");
	MYSQL *  con  = GetConnection ();
	if(!con)
	{
		ReleaseConnection( con);
		return mysql_error(con);
	}
	std::string  trId;
	std::string ret = GetTransId(con, trId);
	if(ret != "OK")
	{
		ReleaseConnection( con);
		TRACE("Error  Repository  InitialIssueDB.  %s\n", ret.c_str());
		return ret;
	}

    	/////
	ret = InsertTDNDB(con, trId, TDNsgn, amount,   pin,   cert, note);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		TRACE("Error Repository  InitialIssueDB.  %s\n", ret.c_str());
		return ret;
	}
	/////
	
	UpdateBankAcc(con, trId, TDNsgn, amount,"0", note);
	
	ReleaseConnection( con);
	TRACE("Exit Repository  InitialIssueDB.  %s", "\n");
	return "OK";
} 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string  RedeemTDNdb(std::string& TDNsgn)
{
	TRACE(" Enter Repository  RedeemTDNdb  %s", "\n");
	MYSQL *  con  = GetConnection ();
	if(!con)
	{
		ReleaseConnection( con);
		return mysql_error(con);
	}
	std::string  trId;
	std::string ret = GetTransId(con, trId);
	if(ret != "OK")
	{
		ReleaseConnection( con);
		TRACE("Error  Repository  RedeemTDNdb.  %s\n", ret.c_str());
		return ret;
	}

	ret = CancelTDN( con, trId,  trType, TDNsgn);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	/////
	std::string  note;
	std::string amount = TDNsgn.substr(TDNSING_LEN - AMOUNT_LEN);
	UpdateBankAcc(con, trId, TDNsgn, "0", amount, note);
	
	ReleaseConnection( con);

	TRACE(" Exit Repository  RedeemTDNdb  %s", "\n");
	return "OK";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string  ValidateTDNdb(std::string& TDNsgn, std::string& amount, std::string& status)
{
	TRACE(" Enter Repository  ValidateTDNdb  %s", "\n");

	MYSQL *  con  = GetConnection ();
	if(!con)
	{
		ReleaseConnection( con);
		TRACE("Exit Repository ValidateTDNdb  %s", "\n");
		return mysql_error(con);
	}
	std::string pin; // just a placeholder of validate
	std::string cert; // just a placeholder of validate
	//////
	std::string ret;
	ret = GetTDNdata(con, TDNsgn, amount,  status, pin, cert);
	if (ret != "OK")
	{
		TRACE("Exit Repository ValidateTDNdb  %s", "\n");
		ReleaseConnection( con);
		return ret;
	}
	/////
	ReleaseConnection( con);
        
        amount.insert(0, 10 - amount.length(), '0');
	TRACE("Return Amount %s  \n", amount.c_str());
	TRACE("       Status   %s  \n", status.c_str() );
 	TRACE("Exit Repository ValidateTDNdb  %s", "\n");
	return "OK";
}

std::string  SplitTDNdb(std::string& TDNsgn1, std::string& amount1, std::string& pin, std::string& TDNsgn2, std::string& amount2)
{
	TRACE("Enter Repository SplitTDNdb  %s", "\n");
	MYSQL *  con  = GetConnection ();
	if(!con)
	{
		ReleaseConnection( con);
		return mysql_error(con);
	}
	std::string amount;
	std::string status;
	std::string pinTmp;
	std::string cert;
	std::string note; // to do, pass as parameter
	//////  should be a transaction

	std::string ret;
	ret = GetTDNdata(con, TDNsgn1, amount,  status, pinTmp, cert);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	/////

	if (pinTmp != pin)
	{
		TRACE("Invalid PIN %s", "\n");
		ReleaseConnection( con);
		return("Invalid PIN");
	}

	if (status != "1")
	{

 		ReleaseConnection( con);
		TRACE("Invalid status %s", "\n");
		return("Invalid status");
	}


	long  amt  = stol(amount) - stol(amount1);


        amount2  = std::to_string( amt);

        amount2.insert(0, 10 - amount2.length(), '0');
	std::string  trId;
	ret = GetTransId(con, trId);
	std::string  trType = std::to_string(SPLIT);

	ret = CancelTDN( con, trId,  trType, TDNsgn1);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}

	ret = InsertTDNDB(con, trId, trType, TDNsgn1, amount1,   pin,   cert,  note );
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}

	ret = InsertTDNDB(con, trId, trType, TDNsgn2, amount2,   pin,   cert  , note);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}

	ReleaseConnection( con);

	TRACE("Exit Repository SplitTDNdb  %s", "\n");
	return "OK";
}
std::string  ConsolidateTDNdb(std::string& TDNsgn1, std::string& pin1, std::string& TDNsgn2, std::string& pin2, std::string& amount)
{
  	TRACE("Exit Repository ConsolidateTDNdb  %s", "\n");
	MYSQL *  con  = GetConnection ();
	if(!con)
	{
		ReleaseConnection( con);
		return mysql_error(con);
	}
	std::string amount1;
	std::string amount2;
	std::string status;
	std::string pinTmp;
	std::string cert;
	std::string note("Consolidate");
	std::string ret;
	ret = GetTDNdata(con, TDNsgn2, amount2,  status, pinTmp, cert);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	/////
	if (!pinTmp.empty() && pinTmp != pin2)
	{
		ReleaseConnection( con);
		TRACE("Invalid second TDN PIN %s", "\n");
		return("Invalid second TDN PIN");
	}
	if (status != "1")
	{
		ReleaseConnection( con);
		TRACE("Invalid second TDN status %s", "\n");
		return("Invalid second TDN status");
	}

	ret = GetTDNdata(con, TDNsgn1, amount1,  status, pinTmp, cert);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	/////
	if (!pinTmp.empty() && pinTmp != pin1)
	{
		ReleaseConnection( con);
		TRACE("Invalid first TDN PIN %s", "\n");
		return("Invalid first TDN PIN");
	}
	if (status != "1")
	{
		ReleaseConnection( con);
		TRACE("Invalid first TDN status %s", "\n");
		return("Invalid first TDN status");
	}

	long amt = std::stol(amount1) + std::stol( amount2);
	amount  = std::to_string( amt);
        amount.insert(0, 10 - amount.length(), '0');


	std::string  trId;
	ret = GetTransId(con, trId);
	std::string  trType = std::to_string(CONSOLIDATE);

	ret = CancelTDN( con, trId,  trType, TDNsgn1);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	ret = CancelTDN( con, trId,  trType, TDNsgn2);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	ret = InsertTDNDB(con, trId, trType, TDNsgn1, amount,   pin1,   cert , note);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	TRACE("Return Amount %s  \n", amount.c_str());
	TRACE("       Status   %s  \n", status.c_str() );
	TRACE("Exit Repository SplitTDNdb  %s", "\n");

	TRACE("Exit Repository ConsolidateTDNdb  %s", "\n");
	return "OK";
}
std::string  ReqestTDNOwnershipDB(std::string& TDNsgn, std::string& pin)
{
	TRACE("Exit Repository ReqestTDNOwnershipDB  %s", "\n");
	MYSQL *  con  = GetConnection ();
	if(!con)
	{
		ReleaseConnection( con);
		return mysql_error(con);
	}
	std::string amount;
	std::string status;
	std::string pinTmp;
	std::string cert;
	std::string note("Reissued");
	//////  should be a transaction
	std::string ret;
	ret = GetTDNdata(con, TDNsgn, amount,  status, pinTmp, cert);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	/////
        amount = std::string(10 - amount.length(), '0') + amount;
        if ( ret  != "OK")
        {
        //      mysql_close(con);
                ReleaseConnection( con);
                TRACE("Error  Repository  InitialIssueDB.  %s\n", ret.c_str());
                return ret;
        };
 	if (!pinTmp.empty() && pinTmp != pin)
	{
		ReleaseConnection( con);
		TRACE("Invalid  TDN PIN %s", "\n");
		return("Invalid  TDN PIN");
	}

	std::string  trId;
	ret = GetTransId(con, trId);
	std::string  trType = std::to_string(REISSUANCE);

	ret = CancelTDN( con, trId,  trType, TDNsgn);
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
	ret = InsertTDNDB(con, trId, trType, TDNsgn, amount,   pin,   cert, note );
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}
//	TRACE("Return Amount %s  \n", amount.c_str());
//	TRACE("       Status   %s  \n", status.c_str() );
//	TRACE("Exit Repository SplitTDNdb  %s", "\n");

	ReleaseConnection( con);
	TRACE("Exit Repository ReqestTDNOwnershipDB  %s", "\n");
	return "OK";

}
std::string  SetTDNPinDB(std::string& TDNsgn,
	std::string& pinOld,
	std::string& pinNew,
	std::string& cert,
	std::string& note)
{
	TRACE("Exit Repository ReqestTDNOwnershipDB  %s", "\n");
	MYSQL *  con  = GetConnection ();
	if(!con)
	{
		ReleaseConnection( con);
		return mysql_error(con);
	}
	std::string amount;
	std::string status;
	std::string pinTmp;
	//////  should be a transaction
	std::string ret;
	ret = GetTDNdata(con, TDNsgn, amount,  status, pinTmp, cert);
	if (ret != "OK" )
	{
		ReleaseConnection( con);
		return ret;
	}
	/////
	if (pinOld != "" || pinTmp != pinOld)
	{
		ReleaseConnection( con);
		TRACE("Invalid PIN %s", "\n");
		return("Invalid  PIN");
	}
	ret = UpdateTDN(con, TDNsgn,   pinNew,   cert, note );
	if (ret != "OK")
	{
		ReleaseConnection( con);
		return ret;
	}	
//	TRACE("Return Amount %s  \n", amount.c_str());
//	TRACE("       Status   %s  \n", status.c_str() );
//	TRACE("Exit Repository SplitTDNdb  %s", "\n");
	
	ReleaseConnection( con);
	TRACE("Exit Repository ReqestTDNOwnershipDB  %s", "\n");
	return "OK";

}

