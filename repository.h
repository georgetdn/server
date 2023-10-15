#pragma once

#include <limits>
#include <iostream>
#include <string>

std::string  InitialIssueDB(std::string & TDNsgn, 
											std::string  amount, 
											std::string  pin, 
											std::string  cert,
											std::string note) ;;
std::string  RedeemTDNdb(std::string& TDNsgn);
std::string  ValidateTDNdb(std::string&  TDNsgn, std::string& amt, std::string& status);
std::string  SplitTDNdb(std::string& tdn1, std::string& amount1, std::string& pin, std::string& tdn2, std::string& amount2);
std::string  ConsolidateTDNdb(std::string& tdn1, std::string& pin1, std::string& tdn2, std::string& pin2, std::string& amount);
std::string  ReqestTDNOwnershipDB(std::string& TDNsgn, std::string& pin);
std::string  SetTDNPinDB(std::string& TDNsgn, 
										std::string& pinOld, 
										std::string& pinNew, 
										std::string& cert,
										std::string& note);