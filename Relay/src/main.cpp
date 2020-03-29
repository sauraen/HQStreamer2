#include "main.hpp"
#include "relay.hpp"

#include <iostream>
#include <string>

int main(){
    HQS2Relay relay;
    //Input loop
    bool wantexit = false;
    while(!wantexit){
        std::cout << "> ";
        std::string in_s;
        std::getline(std::cin, in_s);
        String input(in_s);
        String command = input.upToFirstOccurrenceOf(" ", false, true).toLowerCase();
        String args = input.fromFirstOccurrenceOf(" ", false, true);
        if(command.isEmpty()){
            std::cout << "(No command entered.)\n";
        }else if(command == "exit" || command == "quit"){
            wantexit = true;
        }else if(command == "setpassphrase"){
            relay.SetPassphrase(args);
        }else if(command == "sessions"){
            relay.PrintSessionsInfo();
        }else if(command == "status"){
            if(args.isEmpty()){
                std::cout << "Usage: status sessionname\n";
            }else{
                relay.PrintSessionInfo(args);
            }
        }else if(command == "kick"){
            if(args.isEmpty()){
                std::cout << "Usage: kick sessionname\n";
            }else{
                relay.KickSession(args);
            }
        }else if(command == "help"){
            std::cout << "Commands: exit/quit, setpassphrase, sessions, status, kick\n";
        }else{
            std::cout << "Invalid command \"" << command << "\", please type \"help\"\n";
        }
    }
}
