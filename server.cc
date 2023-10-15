/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <mysql.h>
#include <iostream>
#include <memory>
#include <string>
#include "common.h"
#include <pthread.h>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

//#ifdef BAZEL_BUILD
//#include "examples/protos/prototype.grpc.pb.h"
//#else
#include "./cmake/build/prototype.grpc.pb.h"
//#endif
#include "./repository.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;


using TDNSYSpkg::InitalRequest;
using TDNSYSpkg::InitalReplay;
using TDNSYSpkg::OneVarRequest;
using TDNSYSpkg::OneVarReply;
using TDNSYSpkg::TwoVarRequest;
using TDNSYSpkg::TwoVarReply;
using TDNSYSpkg::ThreeVarRequest;
using TDNSYSpkg::ThreeVarReply;
using TDNSYSpkg::FourVarRequest;
using TDNSYSpkg::FourVarReply;

using TDNSYSpkg::TDNSYSservice;

// Logic and data behind the server's behavior.
class TDNSYSServiceImpl final : public TDNSYSservice::Service {
public:
		TDNSYSServiceImpl() :
			TDNSYSservice::Service ()
	{
		 TRACE("Executing TDNSYSServiceImpl constructor%s", "\n");
	}
	~TDNSYSServiceImpl() 
	{
   		 TRACE("Executing TDNSYSServiceImpl destructor%s", "\n");
	}

 Status InitialIssue(ServerContext* context, const InitalRequest* request,
      InitalReplay* reply) override {

    TRACE("Enter Server GetTDNplay%s", "\n");

    std::string  amount = request->amount();
    std::string  pin =	request->pin();
    std::string  cert =	request->cert();
    std::string note = request->note();
    std::string TDNsgn;
    std::string  ret = InitialIssueDB(TDNsgn,   amount,   pin,   cert,  note ) ;

    reply->set_tdnsign(TDNsgn);
	reply->set_ret(ret);

    TRACE("Exit Server GetTDNplay%s" , "\n");
    TRACE("=================%s", "\n");
    return Status::OK;
  }
   
   Status RedeemTDN(ServerContext* context, const OneVarRequest* request,
   OneVarReply* reply) override {
   TRACE("Enter server Redeem TDN%s" , "\n");

		std::string TDNsgn = request->var();
		std::string ret;
	std::cout <<"Enter Redeem TDN " << TDNsgn ;
		ret = RedeemTDNdb(TDNsgn);
		if ( ret == "OK")
		{
		TRACE( "Exit server Redeem TDN%s", "\n");
		TRACE( "=================%s", "\n");

		}
		return Status::OK;
   }
  
  Status ValidateTDN(ServerContext* context, const OneVarRequest* request,
      TwoVarReply* reply) override {
      TRACE("Enter Server ValidateTDN%s" , "\n");

      std::string TDNsgn = request->var();
      std::string amt;
      std::string status;
      std::string ret;

	  ret = ValidateTDNdb(TDNsgn, amt, status);
	  if ( ret == "OK")
	  {
		reply->set_var1(amt);
		reply->set_var2(status);
//		TRACE( "amount %s \n", amt );
		TRACE( "Exit Server ValidateTDN%s", "\n");
		TRACE( "=================%s", "\n");
		return Status::OK;
	  }
	  else
	  {
		reply->set_var1("");
		reply->set_var2("");
		Status stat(grpc::NOT_FOUND , ret);

               TRACE("Exit Server ValidateTDN%s", "\n");
               TRACE("=================%s", "\n");
               return stat;
	  }
  }
  Status SplitTDN(ServerContext* context, const ThreeVarRequest* request,
      FourVarReply* reply) override {
      TRACE("Enter Server SplitTDN%s", "\n");
      std::string tdn1 = request->var1();
      std::string value1 = request->var2();
      std::string pin = request->var3();
      std::string tdn2;
      std::string value2;
      std::cout << "TDN before"  << tdn1 << std::endl;
      std::cout << "Amount  " << value1 <<"  PIN  " << pin <<  std::endl;

    std::string ret = SplitTDNdb(tdn1, value1, pin, tdn2, value2);
	std::cout << "Return value " << ret << std::endl;
      std::cout << "TDN after"  << tdn1 << std::endl;
      std::cout << "Amount   " << value1 <<"  PIN  " << pin <<  std::endl;
      std::cout << "TDN2     "  << tdn2 << std::endl;
      std::cout << "Amount2  " << value2 <<"  PIN  " << pin <<  std::endl;


      reply->set_var1(tdn1);
      reply->set_var2(value1);
      reply->set_var3(tdn2);
      reply->set_var4(value2);

      TRACE( "Exit Server SplitTDN%s", "\n");
      TRACE( "=================%s", "\n");
      return Status::OK;
  }
  Status ConsolidateTDN(ServerContext* context, const FourVarRequest* request,
      ThreeVarReply* reply) override {
      TRACE( "Enter Server ConsolidateTDN%s", "\n");
      std::string tdn1 = request->var1();
      std::string pin1 = request->var2();
      std::string tdn2 = request->var3();
      std::string pin2 = request->var4();
      std::string value;

      ConsolidateTDNdb(tdn1, pin1, tdn2, pin2, value);

      reply->set_var1(tdn1);
      reply->set_var2(pin1);
      reply->set_var3(value);
//      TRACE( "Exit Server ConsolidateTDN%s", "\n");
//      TRACE( "=================%s", "\n")
      return Status::OK;
  }
  Status ReqestTDNOwnership(ServerContext* context, const TwoVarRequest* request,
      OneVarReply* reply) override {
      TRACE( "Server ReqestTDNOwnership%s", "\n");
      std::string TDNsgn = request->var1();
      std::string pin = request->var2();
      ReqestTDNOwnershipDB(TDNsgn, pin);
      reply->set_var(TDNsgn);
      TRACE( "Exit Server ReqestTDNOwnership%s", "\n");
      TRACE( "=================%s", "\n");
      return Status::OK;
  }
  Status SetTDNPin(ServerContext* context, const ThreeVarRequest* request,
      OneVarReply* reply) override {
//      TRACE( "Server SetTDNPin%s", "\n")
      std::string TDNsgn = request->var1();
      std::string pin = request->var2();

      std::string pinOld;
      std::string pinNew;
      std::string cert;
      std::string note("Update PIN");;
      SetTDNPinDB(TDNsgn,
		 pinOld,
		 pinNew,
		 cert,
		 note);


	  reply->set_var(pin);
 //     TRACE("Exit Server SetTDNPin%s", "\n");
 //     TRACE( "=================%s", "\n");
      return Status::OK;
  }

 };

 ///// Kill thread
pthread_t tid;
void* doShutdown(void  *server)
{

	getchar();
// Press enter to shutdown the server

	((Server*)(server))->Shutdown();
////	std::cout << "Server is shutting down. "<< std::endl;
	return NULL;
}
void RunServer() {
  std::string server_address("0.0.0.0:50051");
  TDNSYSServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());

  //////////////  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
     int err = pthread_create(&(tid), NULL, &doShutdown, &(*server));
     if (err != 0)
         TRACE("Can't create Shutdown thread :[%s]", strerror(err));
     else
//////	 std::cout << "Press <ENTER> key to shutdown the server." << std::endl;

  server->Wait();


}

int main(int argc, char** argv) {

  //open database
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
   //   fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }


  if (mysql_real_connect(con, "92.249.45.2", "u757650154_GeorgeD", "W3Se$Xdr%sql",
          "u757650154_TDNSYS", 0, NULL, 0) == NULL)
  {
    //  finish_with_error(con);
  }

  RunServer();

  return 0;
}
