#pragma once

std::string InsertTDNDB( MYSQL *  con,  
											const std::string trId, 
											std::string& TDNsgn, 
											std::string amount, 
											std::string  pin, 
											std::string  cert, 
											std::string note);
std::string UpdateBankAcc(MYSQL *  con,
			const std::string trId,
			std::string& TDNsgn,
			std::string debitAmt,
			std::string creditAmt,
			std::string note);
std::string GetTDNdata( MYSQL *   con,  
											std::string & TDNsgn, 
											std::string& amount, 
											std::string& status,
											std::string& pin,
											std::string& cert);
std::string CancelTDN( MYSQL *  con,  
										const std::string trId, 
										std::string & TDNsgn);											
std::string UpdateTDN( MYSQL * con,  
										std::string & TDNsgn,
										std::string pin,
										std::string  cert,
										std::string  note);		
MYSQL * GetConnection (void);
void ReleaseConnection( MYSQL *  con);
std::string GetTransId(MYSQL *con, std::string& trId);
