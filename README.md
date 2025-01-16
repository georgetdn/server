__This is the source code for TDNSYS server, running on Linux.  The code usees gRPS to comunicate with the clients.__    
 
 ___TDNSYS Infrastructure___  
TDNSYS system is centralized, based on the client server model. The infrastructure supporting the TDNSYS relies on existing, reliable and proven state of the art technologies in the Financial market infrastructures (FMIs). It is very similar with the infrastructure required for Credit or Debit Cards. The communications between the Server and the Clients takes place over private or public networks.
The Server side is implemented and maintained by the Central Bank and exposes a public API supporting all the transactions related to TDNs.
The Clients consist of a Central Bank website and hardware/software systems developed by IT companies based on the API. Holders of TDNs can request ownership, split, consolidate and assign a PIN or PKI to these TDNs using the Central Bank website or the proprietary hardware/software clients.

___Server___  
Implementing the server software is not a technical challenge. It consists of an ISAM database, ACID compliant, containing the main TDNs table and administrative tables. The TDNs table supports only insert and update operations. Records are never deleted. The administrative tables include information available when a transaction is performed. They also support a case management system used for storing investigation information regarding complaints and fraud.  

Each TDN has one record in the repository. At the minimum the record contains the TDN Signature, the associated value, date when issued and the date when the record was updated. The update date, PIN and PKI may be null, all the other fields must have a value. The record is inserted when the TDN is issued by the Central Bank.  

The TDN Signature is a string of numbers containing a timestamp, a long unique number and the value zero padded. The status can have the following values: ACTIVE, CANCELED or BLOCKED. The status of a TDN can be accessed by anybody with access to the TDN Signature. The PIN and PKI support TDN security and can be set and updated   using the Central Bank website or any other available application. If the PIN is set it has to be included in the API request and it is validated against the store value before a transaction is performed. If the PKI is set the security test is performed by encrypting a string using the public key stored in the record and compare it with the decrypted value returned by the holder of the TDN. PIN and PKI cannot be both set.
If privacy is not a concern of the TDN holder the PKI field can contain a digital certificate issued by the Central bank. This may even be required if the holder of the TDN accepts payments. The banks must always sign the TDNs with a certificate issued by the Central Bank.  

The TDNSYS must support great volumes of transactions and store very large number of records. It has to be very fast, redundant, with high availability with no down time. There is no need to get into the details as such systems are routinely deployed in the financial industry.

___Clients___  
A TDN client consists of hardware, software and means of communication with the TDNSYS server and with other clients.

The means of communication already exists. The Internet is widely available and the banks have their private networks ready for use by the TDN applications.

TDN client hardware can be any of the shelf digital devices or specially design machines. They may be PCs, MACs, smart phones, tablets, smart watches, etc. Special hardware is needed for cash registers, bank teller terminals, ATMs and vending machines. This hardware may already be in used and only software updates may be necessary in order to support TDN transactions.

Hardware platforms use different technologies for performing input/output operations and TDNs storage.

