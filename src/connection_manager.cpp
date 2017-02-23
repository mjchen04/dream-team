//
// Created by Usama Riaz on 1/24/17.
// Copyright © 2017 Riaz. All rights reserved.
//

#include <iostream>
#include "connection_manager.h"

const std::string PROTOCOL_VERSION = "HTTP/1.1";
const std::string CONTENT_TYPE_HEADER = "Content-Type";

ConnectionManager::ConnectionManager(BasicServerConfig* parsed_config) {
  parsed_config_ = parsed_config;
}

// Boost usage inspired by https://github.com/egalli64/thisthread/blob/master/asio/tcpIpCs.cpp
void ConnectionManager::RunTcpServer() {
  std::cout << "Starting server" << std::endl;
  
  unsigned port = parsed_config_->GetPortNumber();
  boost::asio::io_service aios;
  boost::asio::ip::tcp::acceptor acceptor(aios, 
					  boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

  while (true) {
    boost::asio::ip::tcp::socket socket(aios);
    acceptor.accept(socket);
       
    std::stringstream message_stream;
    boost::asio::streambuf buffer;
    boost::system::error_code error;
    size_t len = read_until(socket, buffer, REQUEST_DELIMITER, error);      

    if (len) {
      message_stream.write(boost::asio::buffer_cast<const char *>(buffer.data()), len);
      
      std::string raw_request = message_stream.str();
      std::cout << raw_request << std::endl;

      std::unique_ptr<Request> req = Request::Parse(raw_request);       
    }
  }
}

RequestHandler::Status HandleRequest(const Request& req, Response* response) {
  return RequestHandler::OK;  
}

void StreamHttpResponse(boost::asio::ip::tcp::socket& socket, const Response& resp) {
  
}
