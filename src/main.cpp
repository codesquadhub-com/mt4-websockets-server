// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <iostream>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include "RSJparser.h"

using namespace std;
int runServer() {
	ix::WebSocketServer server(8080, "0.0.0.0");

	server.setOnConnectionCallback(
		[&server](std::shared_ptr<ix::WebSocket> webSocket,
			std::shared_ptr<ix::ConnectionState> connectionState) {
		webSocket->setOnMessageCallback(
			[webSocket, connectionState, &server](const ix::WebSocketMessagePtr msg) {
			if (msg->type == ix::WebSocketMessageType::Open)
			{
				std::cerr << "New connection" << std::endl;

//				std::cerr << "id: " << connectionState->getId() << std::endl;
//				std::cerr << "Uri: " << msg->openInfo.uri << std::endl;

				std::cerr << "Headers:" << std::endl;
				for (auto it : msg->openInfo.headers)
				{
					std::cerr << it.first << ": " << it.second << std::endl;
				}

				webSocket->setUrl(msg->openInfo.headers["channels"]);
			}
			else if (msg->type == ix::WebSocketMessageType::Message)
			{

				std::cerr << msg->str << std::endl;

				RSJresource msg_json(msg->str);
				string channel(msg_json["channel"].as<string>());				

				std::set<std::shared_ptr<ix::WebSocket>> clients = server.getClients();

				for (auto it : clients) {
					string channels(it->getUrl());
					if( channels.find(channel) != channels.npos) {
						it->sendText(msg->str);
					}
				}

			}
		}
		);
	}
	);

	auto res = server.listen();
	if (!res.first)
	{
		// Error handling
		return 1;
	}

	// Run the server in the background. Server can be stoped by calling server.stop()
	server.start();

	// Block until server.stop() is called.
	server.wait();

	getchar();

	server.stop();

	return 0;
}

int main(int argc, char *argv[])
{
	ix::initNetSystem();

	runServer();
}
