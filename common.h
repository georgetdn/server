#pragma once

//#define NDEBUG
#if defined NDEBUG
#define TRACE( format, ... )
#else
#define TRACE( format, ... )   printf( "+++  %s::%s(%d) +++ " format, __FILE__, __FUNCTION__,  __LINE__, __VA_ARGS__ )
#endif

//TDN Signature layuout
#define TDNSING_LEN     160
#define DATETIME_LEN     20
#define RANDOMNO_LEN    98
#define BANKNAME_LEN     10 // right padded with ~
#define SERVERIP_LEN     22 // right padded with ~
#define AMOUNT_LEN       10 // cents left padded with 0
#define MAX_AMOUNT   9999999.99

#define  INITIAL_ISSUE  1	// TDN Initial Issuance 
#define  REDEMPTION  	2 	// TDN Redemption 
#define  REISSUANCE     3 	// TDN Reissuance
#define  CONSOLIDATE    4  	// TDN Consolidation
#define  SPLIT   	    5	// TDN Split

#include <algorithm>
#include <cctype>
#include <locale>
 
std::string  FormatAmount(std::string& amount);

// trim from start (in place)
 inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
 inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
 inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}
