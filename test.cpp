#include <assert.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include "bank.hpp"

using json = nlohmann::json;
using namespace std;


enum class Action {
    Init,
    Deposit,
    Withdraw,
    Transfer,
    BuyInvestment,
    SellInvestment,
    Unknown
};

Action stringToAction(const std::string &actionStr) {
    static const std::unordered_map<std::string, Action> actionMap = {
        {"init", Action::Init},
        {"deposit_action", Action::Deposit},
        {"withdraw_action", Action::Withdraw},
        {"transfer_action", Action::Transfer},
        {"buy_investment_action", Action::BuyInvestment},
        {"sell_investment_action", Action::SellInvestment}};

    auto it = actionMap.find(actionStr);
    if (it != actionMap.end()) {
        return it->second;
    } else {
        return Action::Unknown;
    }
}

int int_from_json(json j) {
    string s = j["#bigint"];
    return stoi(s);
}

map<string, int> balances_from_json(json j) {
    map<string, int> m;
    for (auto it : j["#map"]) {
        m[it[0]] = int_from_json(it[1]);
    }
    return m;
}

map<int, Investment> investments_from_json(json j) {
    map<int, Investment> m;
    for (auto it : j["#map"]) {
        m[int_from_json(it[0])] = {.owner = it[1]["owner"],
                                   .amount = int_from_json(it[1]["amount"])};
    }
    return m;
}

BankState bank_state_from_json(json state) {
    map<string, int> balances = balances_from_json(state["balances"]);
    map<int, Investment> investments =
        investments_from_json(state["investments"]);
    int next_id = int_from_json(state["next_id"]);
    return {
        .balances = balances, .investments = investments, .next_id = next_id};
}

int main() {
    ofstream output_file("saida.txt");
    for (int i = 0; i < 100; i++) {
        output_file << "Trace #" << i << endl;
        std::ifstream f("traces/out" + to_string(i) + ".itf.json");
        json data = json::parse(f);

        BankState bank_state =
            bank_state_from_json(data["states"][0]["bank_state"]);

        auto states = data["states"];
        for (auto state : states) {

            string action = state["action_taken"];
            json nondet_picks = state["nondet_picks"];

            string error = "";

            switch (stringToAction(action)) {
            case Action::Init: {
                output_file << "initializing" << endl;
                break;
            }
            case Action::Deposit: {
                string depositor = nondet_picks["depositor"]["value"];
                int amount = int_from_json(nondet_picks["amount"]["value"]);
                error = deposit(bank_state, depositor, amount);
                break;
            }
            case Action::Withdraw: {
                string withdrawer = nondet_picks["withdrawer"]["value"];
                int amount = int_from_json(nondet_picks["amount"]["value"]);
                error = withdraw(bank_state, withdrawer, amount);
                break;
            }
            case Action::Transfer: {
                string sender = nondet_picks["sender"]["value"];
                string receiver = nondet_picks["receiver"]["value"];
                int amount = int_from_json(nondet_picks["amount"]["value"]);
                error = transfer(bank_state, sender, receiver, amount);
                break;
            }
            case Action::BuyInvestment: {
                string buyer = nondet_picks["buyer"]["value"];
                int amount = int_from_json(nondet_picks["amount"]["value"]);
                error = buy_investment(bank_state, buyer, amount);
                break;
            }
            case Action::SellInvestment: {
                string seller = nondet_picks["seller"]["value"];
                int id = int_from_json(nondet_picks["id"]["value"]);
                error = sell_investment(bank_state, seller, id);
                break;
            }
            default: {
                error = "";
                break;
            }
            }

            BankState expected_bank_state =
                bank_state_from_json(state["bank_state"]);
            if (expected_bank_state != bank_state) {
                output_file << "--- Erro: Estado do banco inconsistente ---" << endl;
                output_file << "Estado do modelo: " << endl << expected_bank_state << endl;
                output_file << "Estado do banco: " << endl << bank_state << endl;
                output_file << "Última ação tomada: " << action; 
                output_file << endl << endl;
            }

            string expected_error =
                string(state["error"]["tag"]).compare("Some") == 0
                    ? state["error"]["value"]
                    : "";
            

            if (expected_error != error) {
                output_file << "--- Erro: Erro inconsistente ---" << endl;
                output_file << "Erro esperado: " << expected_error << endl;
                output_file << "Erro obtido: " << error << endl;
                output_file << "Última ação tomada: " << action << endl;
                output_file << "Estado do modelo: " << endl << expected_bank_state << endl;
                output_file << "Estado do banco:" << endl << bank_state;
                output_file << endl << endl;
            }
        }
    }
    output_file.close();
    return 0;
}
