#include <map>
#include <string>
using namespace std;

struct Investment {
    string owner;
    int amount;
};

struct BankState {
    map<string, int> balances;
    map<int, Investment> investments;
    int next_id;
};

string deposit(BankState &bank_state, string depositor, int amount) {
  if ( amount <= 0 )
    return "Amount should be greater than zero";

  bank_state.balances[depositor] += amount;
  return "";
}

string withdraw(BankState &bank_state, string withdrawer, int amount) {
  if ( amount <= 0 )
    return "Amount should be greater than zero";  

  if ( bank_state.balances[withdrawer] <= amount )
    return "Balance is too low";

  bank_state.balances[withdrawer] -= amount;
  return "";
}

string transfer(BankState &bank_state, string sender, string receiver,
                int amount) {
  bank_state.balances[sender] -= amount;
  bank_state.balances[receiver] += amount;
  return "";
}

string buy_investment(BankState &bank_state, string buyer, int amount) {
  if ( amount <= 0 )
    return "Amount should be greater than zero";

  else if ( bank_state.balances[buyer] < amount )
    return "Balance is too low";

  bank_state.balances[buyer] -= amount;
  bank_state.investments[bank_state.next_id] = {buyer, amount};
  bank_state.next_id++;
  return "";
}

string sell_investment(BankState &bank_state, string seller,
                       int investment_id) {

  if ( !bank_state.investments.count(investment_id) )
    return "No investment with this id";
    
  bank_state.investments.erase(investment_id);
  bank_state.balances[seller] += bank_state.investments[investment_id].amount;
  return "";
}

bool operator==(const Investment &lhs, const Investment &rhs) {
    return lhs.owner == rhs.owner && lhs.amount == rhs.amount;
}

bool operator==(const BankState &lhs, const BankState &rhs) {
    if (lhs.balances != rhs.balances) {
        return false;
    }

    for (const auto &[key, value] : lhs.balances) {
        auto it = rhs.balances.find(key);
        if (it == rhs.balances.end() || it->second != value) {
            return false;
        }
    }

    if (lhs.investments != rhs.investments) {
        return false;
    }

    for (const auto &[key, value] : lhs.investments) {
        auto it = rhs.investments.find(key);
        if (it == rhs.investments.end() || !(it->second == value)) {
            return false;
        }
    }

    return lhs.next_id == rhs.next_id;
}

bool operator!=(const Investment &lhs, const Investment &rhs) {
    return !(lhs == rhs);
}

bool operator!=(const BankState &lhs, const BankState &rhs) {
    return !(lhs == rhs);
}

ostream &operator<<(ostream &os, const Investment &inv) {
    os << "Investment{owner: " << inv.owner << ", amount: " << inv.amount
       << "}";
    return os;
}

ostream &operator<<(ostream &os, const BankState &state) {
    os << "BankState{\n";

    os << "  Balances:\n";
    for (const auto &[name, balance] : state.balances) {
        os << "    " << name << ": " << balance << "\n";
    }

    os << "  Investments:\n";
    for (const auto &[id, investment] : state.investments) {
        os << "    " << id << ": " << investment << "\n";
    }

    os << "  Next ID: " << state.next_id << "\n";
    os << "}";

    return os;
}